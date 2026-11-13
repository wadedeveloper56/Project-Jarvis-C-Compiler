#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(ObjectAndSectionTests)
	{
	public:
		TEST_METHOD(BigObjSupportsWideSectionNumbersSymbolsAndRelocations)
		{
			Workspace workspace;
			vector<TestSection> sections(32768, TestSection{ ".data",{ 0x5a }, 0xc0000040 });
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			sections.push_back(move(text));
			const auto object = makeBigObjObject(move(sections), {
				TestSymbol{ "main", 0, 32769, 2,{} }, TestSymbol{ "targetFunction", 2, 32769, 2,{} }
				});
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto textHeader = sectionHeader(image, ".text");
			Assert::AreEqual<uint32_t>(get32(image, textHeader + 12), get32(image, optionalHeader(image) + 16), L"32-bit section index resolves the entry point");
			Assert::AreEqual<uint32_t>(static_cast<uint32_t>(-2), get32(image, get32(image, textHeader + 20)), L"20-byte BigObj symbol records resolve AMD64 relocations");
			Assert::AreEqual<uint32_t>(32768, get32(image, sectionHeader(image, ".data") + 8), L"all BigObj sections are parsed and merged");
		}

		TEST_METHOD(BigObjSectionAuxiliaryRecordSelectsComdatWinner)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeBigObjComdatObject("bigObjComdat", { 0x41 }));
			workspace.write(L"second.obj", makeBigObjComdatObject("bigObjComdat", { 0x42 }));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x41) != image.begin() + raw + get32(image, text + 16), L"first BigObj COMDAT candidate is selected");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x42) == image.begin() + raw + get32(image, text + 16), L"losing BigObj COMDAT candidate is discarded");
		}

		TEST_METHOD(BigObjArchiveMemberCanSatisfyExternalSymbol)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "bigObjHelper", 0, 0, 2,{} } }));
			workspace.write(L"bigobj.lib", makeArchive({ { "bighelper.obj", makeBigObjObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "bigObjHelper", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"bigobj.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(sectionHeader(workspace.read(L"linked.exe"), ".text") != 0, L"BigObj archive member is selected and linked");
		}

		TEST_METHOD(InvalidBigObjClassIdentifierIsRejected)
		{
			Workspace workspace;
			auto object = makeBigObjObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			object[12] ^= 0x01;
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "unsupported anonymous COFF object signature");
			assertNoOutput(workspace);
		}

		TEST_METHOD(SubsectionsAreOrderedStablyAcrossObjectsAndArchives)
		{
			Workspace workspace;
			const uint32_t flags = 0x40000040;
			TestSection references{ ".data", Bytes(24, 0), 0xc0000040 };
			references.relocations = { { 0, 1, 1 }, { 8, 2, 1 }, { 16, 3, 1 } };
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, references, TestSection{ ".order$Z",{ 0x7a }, flags }, TestSection{ ".order$M",{ 0x31 }, flags } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "early", 0, 0, 2,{} }, TestSymbol{ "late", 0, 3, 2,{} }, TestSymbol{ "tie2", 0, 0, 2,{} } }));
			workspace.write(L"second.obj", makeObject({ TestSection{ ".order$M",{ 0x32 }, flags }, TestSection{ ".order",{ 0x30 }, flags } }, { TestSymbol{ "tie2", 0, 1, 2,{} } }));
			workspace.write(L"extras.lib", makeArchive({ { "early.obj", makeObject({ TestSection{ ".order$A",{ 0x41 }, flags | 0x00500000 } }, { TestSymbol{ "early", 0, 1, 2,{} } }) } }));
			const vector<wstring> args{ L"/ENTRY:main", L"/OUT:ordered.exe", L"entry.obj", L"second.obj", L"extras.lib" };
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"ordered.exe");
			const auto orderHeader = sectionHeader(image, ".order");
			const auto raw = get32(image, orderHeader + 20);
			const auto base = get64(image, optionalHeader(image) + 24) + get32(image, orderHeader + 12);
			Assert::AreEqual<uint8_t>(0x30, image[raw]);
			Assert::AreEqual<uint8_t>(0x41, image[raw + 16], L"archive subsection honors 16-byte alignment");
			Assert::AreEqual<uint8_t>(0x31, image[raw + 17], L"equal names retain object order");
			Assert::AreEqual<uint8_t>(0x32, image[raw + 18]);
			Assert::AreEqual<uint8_t>(0x7a, image[raw + 19]);
			const auto data = get32(image, sectionHeader(image, ".data") + 20);
			Assert::AreEqual<uint64_t>(base + 16, get64(image, data));
			Assert::AreEqual<uint64_t>(base + 19, get64(image, data + 8));
			Assert::AreEqual<uint64_t>(base + 18, get64(image, data + 16));
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(image == workspace.read(L"ordered.exe"), L"repeat links preserve deterministic ordering");
		}

		TEST_METHOD(SortedAssociativeComdatsKeepWinnerAndRelocationMappings)
		{
			Workspace workspace;
			auto associative = [](uint8_t marker)
			{
				TestSection parent{ ".text$M", { 0xc3 }, 0x60001020 };
				TestSection child{ ".order$M", { marker }, 0x40001040 };
				TestSection reference{ ".refs", Bytes(8, 0), 0x40000040 };
				reference.relocations = { { 0, 3, 1 } };
				return makeObject({ parent, child, reference }, {
					TestSymbol{ ".text$M", 0, 1, 3,{ sectionAux(1, 0, 2) } }, TestSymbol{ "parentKey", 0, 1, 2,{} },
					TestSymbol{ ".order$M", 0, 2, 3,{ sectionAux(1, 1, 5) } }, TestSymbol{ "childLocal", 0, 2, 3,{} }
					});
			};
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".order$Z",{ 0x7a }, 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			workspace.write(L"winner.obj", associative(0x51));
			workspace.write(L"loser.obj", associative(0x52));
			workspace.write(L"early.obj", makeObject({ TestSection{ ".order$A",{ 0x41 }, 0x40000040 } }, {}));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:comdat.exe", L"entry.obj", L"winner.obj", L"loser.obj", L"early.obj" }));
			const auto image = workspace.read(L"comdat.exe");
			const auto order = sectionHeader(image, ".order");
			const auto raw = get32(image, order + 20);
			Assert::AreEqual<uint8_t>(0x41, image[raw]);
			Assert::AreEqual<uint8_t>(0x51, image[raw + 1]);
			Assert::AreEqual<uint8_t>(0x7a, image[raw + 2]);
			const auto expected = get64(image, optionalHeader(image) + 24) + get32(image, order + 12) + 1;
			const auto references = get32(image, sectionHeader(image, ".refs") + 20);
			Assert::AreEqual<uint64_t>(expected, get64(image, references));
			Assert::AreEqual<uint64_t>(expected, get64(image, references + 8), L"losing associative section resolves to the selected child");
		}

		TEST_METHOD(JwlinkSubsectionOrderingIsMeasuredSeparately)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, TestSection{ ".seq$Z", { 0x7a }, 0x40000040 }, TestSection{ ".seq$A", { 0x41 }, 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:new.exe", L"input.obj" }));
			assertSuccess(runJwlink(workspace, { L"input.obj" }, L"old.exe"));
			const auto modern = workspace.read(L"new.exe");
			const auto original = workspace.read(L"old.exe");
			const auto newRaw = get32(modern, sectionHeader(modern, ".seq") + 20);
			Assert::AreEqual<uint8_t>(0x41, modern[newRaw], L"JWlink2 uses lexical subsection order");
			Assert::AreEqual<uint8_t>(0x7a, modern[newRaw + 1]);
			const auto oldSection = sectionHeader(original, ".rdata");
			const auto oldRaw = get32(original, oldSection + 20);
			const auto oldSize = get32(original, oldSection + 8);
			Assert::IsTrue(oldSize >= 2 && oldSize <= get32(original, oldSection + 16));
			const auto first = find(original.begin() + oldRaw, original.begin() + oldRaw + oldSize, uint8_t{ 0x41 });
			const auto second = find(original.begin() + oldRaw, original.begin() + oldRaw + oldSize, uint8_t{ 0x7a });
			Assert::IsTrue(first != original.begin() + oldRaw + oldSize && second != original.begin() + oldRaw + oldSize && first < second, L"JWlink also orders A before Z, but places them in .rdata with different padding");
		}

		TEST_METHOD(OrderedSubsectionsExecuteTlsCallbacksAndConstructors)
		{
			Workspace workspace;
			TestSection entry{ ".text", { 0x48, 0x83, 0xec, 0x28, 0xff, 0x15, 0, 0, 0, 0, 0xff, 0x15, 8, 0, 0, 0, 0x8b, 0x0d, 0, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			entry.relocations = { { 6, 1, 4 }, { 12, 1, 4 }, { 18, 2, 4 }, { 23, 3, 4 } };
			TestSection tlsDirectory{ ".rdata$T", Bytes(40, 0), 0x40000040 };
			tlsDirectory.relocations = { { 0, 5, 1 }, { 8, 6, 1 }, { 16, 7, 1 }, { 24, 8, 1 } };
			workspace.write(L"entry.obj", makeObject({ entry, tlsDirectory, TestSection{ ".data", Bytes(12, 0), 0xc0000040 }, TestSection{ ".tls$ZZZ", Bytes(8, 0), 0xc0400040 } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ctorStart", 0, 0, 2,{} }, TestSymbol{ "state", 0, 3, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} }, TestSymbol{ "__tls_used", 0, 2, 2,{} }, TestSymbol{ "tlsStart", 4, 3, 2,{} }, TestSymbol{ "tlsEnd", 5, 3, 2,{} }, TestSymbol{ "tlsIndex", 8, 3, 2,{} }, TestSymbol{ "callbackStart", 0, 0, 2,{} } }));
			auto contribution = [](const string& sectionName, const string& function, const string& pointer, uint8_t digit)
			{
				TestSection code{ ".text", { 0x6b, 0x05, 0, 0, 0, 0, 10, 0x83, 0xc0, digit, 0x89, 0x05, 0, 0, 0, 0, 0xc3 } };
				code.relocations = { { 2, 1, 5 }, { 12, 1, 4 } };
				TestSection slot{ sectionName, Bytes(8, 0), 0xc0400040 };
				slot.relocations = { { 0, 0, 1 } };
				return makeObject({ code, slot }, { TestSymbol{ function, 0, 1, 2,{} }, TestSymbol{ "state", 0, 0, 2,{} }, TestSymbol{ pointer, 0, 2, 2,{} } });
			};
			workspace.write(L"late.obj", contribution(".CRT$XCV", "ctorB", "ctorSlotB", 4));
			workspace.write(L"lateTls.obj", contribution(".tls$BBB", "callbackB", "callbackSlotB", 2));
			workspace.write(L"early.lib", makeArchive({ { "ctor.obj", contribution(".CRT$XCU", "ctorA", "ctorStart", 3) }, { "tls.obj", contribution(".tls$AAA", "callbackA", "callbackStart", 1) } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:ordered.exe", L"entry.obj", L"late.obj", L"lateTls.obj", L"early.lib", L"kernel.lib" }));
			const auto image = workspace.read(L"ordered.exe");
			const auto crt = sectionHeader(image, ".CRT");
			const auto tls = sectionHeader(image, ".tls");
			const auto imageBase = get64(image, optionalHeader(image) + 24);
			for (const auto header : { crt, tls })
			{
				const auto raw = get32(image, header + 20);
				const auto first = get64(image, raw) - imageBase;
				const auto second = get64(image, raw + 8) - imageBase;
				Assert::IsTrue(first != second && first <= UINT32_MAX && second <= UINT32_MAX, L"ordered pointers resolve to distinct functions");
				Assert::AreEqual<uint8_t>(header == crt ? 3 : 1, image[rvaOffset(image, static_cast<uint32_t>(first)) + 9]);
				Assert::AreEqual<uint8_t>(header == crt ? 4 : 2, image[rvaOffset(image, static_cast<uint32_t>(second)) + 9]);
			}
			Assert::AreEqual<uint64_t>(0, get64(image, get32(image, tls + 20) + 16), L"callback array is null-terminated");
			Assert::AreEqual<DWORD>(1234, runExecutable(workspace, workspace.file(L"ordered.exe"), {}).exitCode, L"loader callbacks run before ordered constructors");
		}

		TEST_METHOD(RuntimeMetadataDirectoriesArePreserved)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3, 0xc3 } };
			TestSection pdata{ ".pdata", Bytes(12, 0), 0x40000040 };
			pdata.relocations = { { 0, 0, 3 },{ 4, 1, 3 },{ 8, 2, 3 } };
			TestSection xdata{ ".xdata",{ 1, 0, 0, 0 }, 0x40000040 };
			TestSection tlsDirectory{ ".rdata$T", Bytes(40, 0), 0x40000040 };
			tlsDirectory.relocations = { { 0, 4, 1 },{ 8, 5, 1 },{ 16, 6, 1 } };
			TestSection tlsTemplate{ ".tls$",{ 0x31, 0x32, 0, 0 }, 0xc0000040 };
			TestSection tlsIndex{ ".data$t", Bytes(4, 0), 0xc0000040 };
			TestSection loadConfig{ ".rdata$L", Bytes(0x70, 0), 0x40000040 };
			put32(loadConfig.data, 0, 0x70);
			TestSection resources{ ".rsrc$01", Bytes(16, 0), 0x40000040 };
			const auto object = makeObject({ text, pdata, xdata, tlsDirectory, tlsTemplate, tlsIndex, loadConfig, resources }, {
				TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "functionEnd", 1, 1, 2,{} }, TestSymbol{ "unwindInfo", 0, 3, 2,{} },
				TestSymbol{ "__tls_used", 0, 4, 2,{} }, TestSymbol{ "tlsStart", 0, 5, 2,{} }, TestSymbol{ "tlsEnd", 4, 5, 2,{} },
				TestSymbol{ "tlsIndex", 0, 6, 2,{} }, TestSymbol{ "__load_config_used", 0, 7, 2,{} }
				});
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			auto verifyDirectory = [&](size_t directoryOffset, const string& sectionName, uint32_t expectedSize)
				{
					const auto rva = get32(image, optional + directoryOffset);
					Assert::IsTrue(rva != 0, L"runtime data-directory RVA is present");
					Assert::AreEqual<uint32_t>(expectedSize, get32(image, optional + directoryOffset + 4), L"runtime data-directory size");
					Assert::IsTrue(rva >= get32(image, sectionHeader(image, sectionName) + 12), L"runtime directory points into the expected output section");
				};
			verifyDirectory(128, ".rsrc", 16);
			verifyDirectory(136, ".pdata", 12);
			verifyDirectory(184, ".rdata", 40);
			verifyDirectory(192, ".rdata", 0x70);
			const auto pdataHeader = sectionHeader(image, ".pdata");
			const auto pdataRaw = get32(image, pdataHeader + 20);
			Assert::AreEqual<uint32_t>(get32(image, sectionHeader(image, ".text") + 12), get32(image, pdataRaw), L"RUNTIME_FUNCTION begins at the linked code RVA");
			Assert::AreEqual<uint32_t>(get32(image, pdataRaw + 8), get32(image, sectionHeader(image, ".xdata") + 12), L"RUNTIME_FUNCTION retains its unwind-info RVA");
			const auto tlsRva = get32(image, optional + 184);
			const auto tlsRaw = rvaOffset(image, tlsRva);
			const auto tlsBase = get64(image, optional + 24);
			Assert::AreEqual<uint64_t>(tlsBase + get32(image, sectionHeader(image, ".tls") + 12), get64(image, tlsRaw), L"TLS template start VA is relocated");
			Assert::AreEqual<uint64_t>(tlsBase + get32(image, sectionHeader(image, ".tls") + 12) + 4, get64(image, tlsRaw + 8), L"TLS template end VA is relocated");
			assertHasBaseReloc(image, tlsRva, 10);
		}

		TEST_METHOD(MalformedExceptionDirectorySizeIsRejected)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".pdata", Bytes(13, 0), 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "exception directory size is not a multiple of RUNTIME_FUNCTION");
			assertNoOutput(workspace);
		}

		TEST_METHOD(UnsortedExceptionDirectoryIsRejected)
		{
			Workspace workspace;
			TestSection pdata{ ".pdata", Bytes(24, 0), 0x40000040 };
			pdata.relocations = { { 0, 1, 3 },{ 4, 3, 3 },{ 8, 4, 3 },{ 12, 0, 3 },{ 16, 2, 3 },{ 20, 4, 3 } };
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3, 0xc3, 0xc3 } }, pdata, TestSection{ ".xdata",{ 1, 0, 0, 0 }, 0x40000040 } }, {
				TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "second", 2, 1, 2,{} }, TestSymbol{ "firstEnd", 1, 1, 2,{} },
				TestSymbol{ "secondEnd", 3, 1, 2,{} }, TestSymbol{ "unwindInfo", 0, 3, 2,{} }
				});
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "unsorted RUNTIME_FUNCTION entry");
			assertNoOutput(workspace);
		}

		TEST_METHOD(ShortResourceDirectoryIsRejected)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".rsrc", Bytes(15, 0), 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "resource directory is shorter than IMAGE_RESOURCE_DIRECTORY");
			assertNoOutput(workspace);
		}

	};
}
