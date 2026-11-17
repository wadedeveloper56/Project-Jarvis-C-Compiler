#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(RelocationTests)
	{
	public:
		TEST_METHOD(OverflowRelocationsAtBoundaryAndInArchivesAreLinked)
		{
			for (const uint32_t count : { 65534u, 65535u, 65536u })
			{
				Workspace workspace;
				TestSection data{ ".data", Bytes(static_cast<size_t>(count) * 4, 0), 0xc0000040 };
				for (uint32_t i = 0; i < count; ++i) data.relocations.push_back({ i * 4, 0, 3 });
				const auto member = count == 65536 ? makeBigObjObject({ data }, { TestSymbol{ "target", 0, 1, 2,{} } }) : makeObject({ data }, { TestSymbol{ "target", 0, 1, 2,{} } });
				TestSection text{ ".text", Bytes(4, 0) };
				text.relocations = { { 0, 1, 3 } };
				workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "target", 0, 0, 2,{} } }));
				workspace.write(L"members.lib", makeArchive({ { "member.obj", member } }));
				assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:linked.exe", L"input.obj", L"members.lib" }));
				const auto image = workspace.read(L"linked.exe");
				const auto header = sectionHeader(image, ".data");
				const auto raw = get32(image, header + 20);
				const auto rva = get32(image, header + 12);
				Assert::AreEqual<uint32_t>(rva, get32(image, raw), L"first relocation is applied");
				Assert::AreEqual<uint32_t>(rva, get32(image, raw + (count - 1) * 4), L"last relocation is applied");
				Assert::AreEqual<uint32_t>(0, get32(image, header + 36) & 0x01000000u, L"COFF overflow flag is not an image section flag");
			}
		}

		TEST_METHOD(OrdinaryMaximumCountAndAbsoluteNoOpAreAccepted)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(4, 0), 0xc0000040 };
			data.relocations.resize(65535, TestRelocation{ 0xffffffffu, 0xffffffffu, 0 });
			auto object = makeObject({ data, TestSection{ ".text", { 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			const auto record = get32(object, 20 + 24);
			object.erase(object.begin() + record, object.begin() + record + 10);
			put32(object, 8, get32(object, 8) - 10);
			put32(object, 20 + 36, 0xc0000040);
			assertSuccess(linkObject(workspace, object));
		}

		TEST_METHOD(MalformedOverflowRelocationTablesAreRejected)
		{
			TestSection data{ ".data", Bytes(4, 0), 0xc0000040 };
			data.relocations.resize(65535, TestRelocation{ 0, 0, 0 });
			const auto original = makeObject({ data, TestSection{ ".text", { 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			const auto record = get32(original, 20 + 24);
			for (int variant = 0; variant < 7; ++variant)
			{
				Workspace workspace;
				auto object = original;
				if (variant == 0) put16(object, 20 + 32, 1);
				if (variant == 1) put32(object, 20 + 24, 0);
				if (variant == 2) put32(object, record, 65535);
				if (variant == 3) put32(object, record + 4, 1);
				if (variant == 4) put16(object, record + 8, 1);
				if (variant == 5) put32(object, record, 0xffffffffu);
				if (variant == 6) put32(object, 20 + 24, static_cast<uint32_t>(object.size() - 5));
				const auto result = linkObject(workspace, object);
				assertFailureContains(result, variant == 5 ? "relocation table" : "relocation overflow");
				assertNoOutput(workspace);
			}
		}

		TEST_METHOD(RealMsvcAndMasmRelocationsAreLinked)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"ml64.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"cl.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; real-object relocation audit skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"overflow.asm", L"option casemap:none\n.data\npublic target\ntarget dq 0\npublic table\ntable label byte\nrept 65535\n dq target\nendm\n.code\npublic main\nmain proc\n mov eax, 42\n ret\nmain endp\nend\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"ml64.exe", { L"/nologo", L"/c", L"/Fooverflow.obj", L"overflow.asm" }));
			const auto object = workspace.read(L"overflow.obj");
			bool foundOverflow = false;
			for (uint16_t i = 0; i < get16(object, 2); ++i)
			{
				const auto header = 20 + static_cast<size_t>(i) * 40;
				if ((get32(object, header + 36) & 0x01000000u) == 0) continue;
				foundOverflow = true;
				Assert::AreEqual<uint16_t>(0xffff, get16(object, header + 32));
				Assert::AreEqual<uint32_t>(65536, get32(object, get32(object, header + 24)));
			}
			Assert::IsTrue(foundOverflow, L"MASM emitted an overflow table");
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:overflow.exe", L"overflow.obj" }));
			const auto image = workspace.read(L"overflow.exe");
			const auto data = sectionHeader(image, ".data");
			const auto raw = get32(image, data + 20);
			const auto dataRva = get32(image, data + 12);
			const auto target = get64(image, optionalHeader(image) + 24) + dataRva;
			Assert::AreEqual<uint64_t>(target, get64(image, raw + 8));
			Assert::AreEqual<uint64_t>(target, get64(image, raw + 8 + 65534 * 8));
			assertHasBaseReloc(image, dataRva + 8 + 65534 * 8, 10);
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"overflow.exe"), {}).exitCode);

			workspace.writeText(L"tls.c", L"__declspec(thread) int values[2] = { 7, 9 };\nint main(void) { return values[1]; }\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"cl.exe", { L"/nologo", L"/c", L"/Z7", L"/O1", L"/GS-", L"/Fotls.obj", L"tls.c" }));
			const auto tlsObject = workspace.read(L"tls.obj");
			uint32_t secRelOffset = 0, expectedOffset = 0;
			bool foundSecRel = false;
			for (uint16_t i = 0; i < get16(tlsObject, 2); ++i)
			{
				const auto header = 20 + static_cast<size_t>(i) * 40;
				const string name(reinterpret_cast<const char*>(tlsObject.data() + header), 8);
				if (name.rfind(".text", 0) != 0) continue;
				const auto relocs = get32(tlsObject, header + 24);
				for (uint16_t r = 0; r < get16(tlsObject, header + 32); ++r)
				{
					const auto entry = relocs + static_cast<size_t>(r) * 10;
					if (get16(tlsObject, entry + 8) != 11) continue;
					foundSecRel = true;
					secRelOffset = get32(tlsObject, entry);
					expectedOffset = get32(tlsObject, get32(tlsObject, 8) + static_cast<size_t>(get32(tlsObject, entry + 4)) * 18 + 8) + get32(tlsObject, get32(tlsObject, header + 20) + secRelOffset);
				}
			}
			Assert::IsTrue(foundSecRel, L"MSVC emitted runtime SECREL for TLS");
			workspace.write(L"index.obj", makeObject({ TestSection{ ".data", Bytes(4, 0), 0xc0000040 }, TestSection{ ".tls", Bytes(16, 0), 0xc0000040 } }, { TestSymbol{ "_tls_index", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/NODEFAULTLIB", L"/ENTRY:main", L"/OUT:tls.exe", L"tls.obj", L"index.obj" }));
			const auto tlsImage = workspace.read(L"tls.exe");
			Assert::AreEqual<uint32_t>(expectedOffset + 16, get32(tlsImage, get32(tlsImage, sectionHeader(tlsImage, ".text") + 20) + secRelOffset), L"MSVC TLS reference includes the preceding contribution");
		}

		TEST_METHOD(RuntimeSectionAndSecrelUseFinalOutputSection)
		{
			Workspace workspace;
			TestSection references{ ".data", Bytes(10, 0), 0xc0000040 };
			put32(references.data, 2, 3);
			put32(references.data, 6, 0xffffffffu);
			references.relocations = { { 0, 1, 10 }, { 2, 1, 11 }, { 6, 1, 11 } };
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, references, TestSection{ ".target$Z", { 0x44 }, 0x40000040 } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "target", 0, 0, 2,{} } }));
			workspace.write(L"target.obj", makeObject({ TestSection{ ".target$A", { 0, 0x11, 0x22 }, 0x40000040 } }, { TestSymbol{ "target", 1, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:linked.exe", L"input.obj", L"target.obj" }));
			const auto image = workspace.read(L"linked.exe");
			const auto targetHeader = sectionHeader(image, ".target");
			const auto dataHeader = sectionHeader(image, ".data");
			const auto dataRaw = get32(image, dataHeader + 20);
			const auto expectedSection = static_cast<uint16_t>((targetHeader - sectionTable(image)) / 40 + 1);
			Assert::AreEqual<uint16_t>(expectedSection, get16(image, dataRaw), L"SECTION is the output PE section number");
			Assert::AreEqual<uint32_t>(4, get32(image, dataRaw + 2), L"SECREL includes sorted contribution offset, symbol value, and addend");
			Assert::AreEqual<uint32_t>(0, get32(image, dataRaw + 6), L"SECREL supports a negative encoded addend");
		}

		TEST_METHOD(SectionRelativeRelocationsValidateTargetsAndWidths)
		{
			for (int variant = 0; variant < 4; ++variant)
			{
				Workspace workspace;
				TestSection data{ ".data", Bytes(variant == 0 ? 1 : variant == 1 ? 3 : 4, 0), 0xc0000040 };
				data.relocations = { { 0, 1, static_cast<uint16_t>(variant == 0 || variant == 3 ? 10 : 11) } };
				const auto object = makeObject({ TestSection{ ".text", { 0xc3 } }, data }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "absolute", 7, -1, 3,{} } });
				const auto result = linkObject(workspace, object);
				if (variant < 2) assertFailureContains(result, "outside initialized section data");
				else if (variant == 2) assertFailureContains(result, "target has no output section");
				else
				{
					assertSuccess(result);
					const auto image = workspace.read(L"linked.exe");
					Assert::AreEqual<uint16_t>(static_cast<uint16_t>(get16(image, peHeader(image) + 6) + 1), get16(image, get32(image, sectionHeader(image, ".data") + 20)));
				}
			}
		}

		TEST_METHOD(Rel32ThroughRel32FiveAreAppliedWithCorrectPlaceBias)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(24, 0) };
			for (uint16_t type = 4; type <= 9; ++type) text.relocations.push_back({ static_cast<uint32_t>((type - 4) * 4), 0, type });
			const auto object = makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto section = sectionHeader(image, ".text");
			const auto raw = get32(image, section + 20);
			const auto sectionRva = get32(image, section + 12);
			for (uint16_t type = 4; type <= 9; ++type)
			{
				const auto offset = static_cast<size_t>((type - 4) * 4);
				const auto extra = static_cast<int64_t>(type - 4);
				const auto expected = static_cast<int32_t>(-static_cast<int64_t>(offset + 4) - extra);
				Assert::AreEqual<uint32_t>(static_cast<uint32_t>(expected), get32(image, raw + offset), L"REL32-family displacement");
			}
			Assert::AreEqual<uint32_t>(sectionRva, get32(image, optionalHeader(image) + 16), L"entry remains at section start");
		}

		TEST_METHOD(AbsoluteRelocationsCreateDir64AndHighLowBaseRelocations)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(12, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 },{ 8, 0, 2 } };
			TestSection text{ ".text",{ 0xc3 } };
			const auto object = makeObject({ data, text }, { TestSymbol{ "main", 0, 2, 2,{} } });
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/BASE:0x10000");
			assertSuccess(linkObject(workspace, object, args));
			const auto image = workspace.read(L"linked.exe");
			const auto base = get64(image, optionalHeader(image) + 24);
			const auto dataHeader = sectionHeader(image, ".data");
			const auto dataRva = get32(image, dataHeader + 12);
			const auto dataRaw = get32(image, dataHeader + 20);
			const auto textRva = get32(image, sectionHeader(image, ".text") + 12);
			Assert::AreEqual<uint64_t>(base + textRva, get64(image, dataRaw), L"ADDR64 writes an absolute VA");
			Assert::AreEqual<uint32_t>(static_cast<uint32_t>(base + textRva), get32(image, dataRaw + 8), L"ADDR32 writes a 32-bit VA");
			assertHasBaseReloc(image, dataRva, 10);
			assertHasBaseReloc(image, dataRva + 8, 3);
		}

		TEST_METHOD(Addr32NbWritesAnImageRelativeAddress)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 0, 3 } };
			const auto object = makeObject({ text }, { TestSymbol{ "main", 4, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto section = sectionHeader(image, ".text");
			Assert::AreEqual<uint32_t>(get32(image, section + 12) + 4, get32(image, get32(image, section + 20)), L"ADDR32NB uses an RVA");
		}

		TEST_METHOD(FixedImageOmitsRelocationsAndMarksThemStripped)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 } };
			const auto object = makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/FIXED");
			assertSuccess(linkObject(workspace, object, args));
			const auto image = workspace.read(L"linked.exe");
			Assert::IsTrue((get16(image, 0x96) & 1) != 0, L"relocations-stripped file characteristic");
			Assert::AreEqual<uint32_t>(0, get32(image, optionalHeader(image) + 152), L"fixed image relocation RVA");
			Assert::AreEqual<uint32_t>(0, get32(image, optionalHeader(image) + 156), L"fixed image relocation size");
			Assert::IsTrue((get16(image, optionalHeader(image) + 70) & 0x40) == 0, L"fixed image must not advertise ASLR");
		}

	};
}
