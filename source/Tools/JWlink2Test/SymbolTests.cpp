#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(SymbolTests)
	{
	public:
		TEST_METHOD(CommonSymbolAllocatesBssAndAppearsInMap)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "commonValue", 32, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto bss = sectionHeader(image, ".bss");
			Assert::IsTrue(get32(image, bss + 8) >= 32, L"common symbol storage is allocated in BSS");
			ifstream map(workspace.file(L"linked.map"));
			const string contents((istreambuf_iterator<char>(map)), istreambuf_iterator<char>());
			Assert::IsTrue(contents.find("commonValue") != string::npos, L"map includes common symbol");
		}

		TEST_METHOD(CommonSymbolsUseLargestSizeAndStrongDefinitionsWin)
		{
			Workspace commonWorkspace;
			TestSection references{ ".data", Bytes(16, 0), 0xc0000040 };
			references.relocations = { { 0, 1, 1 },{ 8, 2, 1 } };
			commonWorkspace.write(L"entry.obj", makeObject({ references, TestSection{ ".text",{ 0xc3 } } }, {
				TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "firstCommon", 4, 0, 2,{} }, TestSymbol{ "secondCommon", 8, 0, 2,{} },
				TestSymbol{ "firstCommon", 16, 0, 2,{} }
				}));
			auto commonArgs = outputArgs(commonWorkspace);
			commonArgs.push_back(commonWorkspace.file(L"entry.obj").wstring());
			assertSuccess(runLinker(commonWorkspace, commonArgs));
			auto verifyCommonRanges = [&](const Bytes& image)
				{
					const auto data = get32(image, sectionHeader(image, ".data") + 20);
					const auto first = get64(image, data);
					const auto second = get64(image, data + 8);
					Assert::IsTrue(first + 16 <= second || second + 8 <= first, L"merged common symbols occupy disjoint storage using the largest common size");
				};
			verifyCommonRanges(commonWorkspace.read(L"linked.exe"));

			Workspace strongWorkspace;
			TestSection strongReference{ ".data", Bytes(8, 0), 0xc0000040 };
			strongReference.relocations = { { 0, 1, 1 } };
			strongWorkspace.write(L"entry.obj", makeObject({ strongReference, TestSection{ ".text",{ 0xc3 } } }, {
				TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "sharedValue", 32, 0, 2,{} }
				}));
			strongWorkspace.write(L"strong.obj", makeObject({ TestSection{ ".data",{ 0x5a }, 0xc0000040 } }, { TestSymbol{ "sharedValue", 0, 1, 2,{} } }));
			auto strongArgs = outputArgs(strongWorkspace);
			strongArgs.push_back(strongWorkspace.file(L"entry.obj").wstring());
			strongArgs.push_back(strongWorkspace.file(L"strong.obj").wstring());
			assertSuccess(runLinker(strongWorkspace, strongArgs));
			auto verifyStrongOverride = [&](const Bytes& image)
				{
					const auto optional = optionalHeader(image);
					const auto base = get64(image, optional + 24);
					const auto strongData = sectionHeader(image, ".data");
					const auto raw = get32(image, strongData + 20);
					const auto strongAddress = get64(image, raw);
					Assert::AreEqual<uint64_t>(base + get32(image, strongData + 12) + 8, strongAddress, L"a strong definition overrides a common symbol regardless of object order");
					Assert::AreEqual<uint8_t>(0x5a, image[raw + 8], L"strong definition bytes are linked");
				};
			verifyStrongOverride(strongWorkspace.read(L"linked.exe"));
		}

		TEST_METHOD(ComdatAnyKeepsOneDuplicateDefinition)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeComdatObject("shared", { 0x11 }, 2));
			workspace.write(L"second.obj", makeComdatObject("shared", { 0x22 }, 2));
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			assertSuccess(runJwlink(workspace, { workspace.file(L"entry.obj").wstring(), workspace.file(L"first.obj").wstring(), workspace.file(L"second.obj").wstring() }, L"jwlink.exe"));
			auto verifyWinner = [&](const Bytes& image)
				{
					const auto text = sectionHeader(image, ".text");
					const auto raw = get32(image, text + 20);
					const auto end = raw + get32(image, text + 16);
					Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x11) != image.begin() + end, L"first ANY COMDAT candidate is selected");
					Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x22) == image.begin() + end, L"losing ANY COMDAT candidate is discarded");
				};
			verifyWinner(workspace.read(L"linked.exe"));
			verifyWinner(workspace.read(L"jwlink.exe"));
			ifstream map(workspace.file(L"linked.map"));
			const string contents((istreambuf_iterator<char>(map)), istreambuf_iterator<char>());
			const auto first = contents.find(" shared\n");
			Assert::IsTrue(first != string::npos && contents.find(" shared\n", first + 1) == string::npos, L"map contains exactly one selected COMDAT symbol");
		}

		TEST_METHOD(ComdatLargestChoosesLargestCandidate)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"small.obj", makeComdatObject("largest", { 0x10 }, 6));
			workspace.write(L"large.obj", makeComdatObject("largest", { 0x20, 0x21, 0x22 }, 6));
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			for (const auto& file : { L"entry.obj", L"small.obj", L"large.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			const auto rawSize = get32(image, text + 16);
			const Bytes expected{ 0x20, 0x21, 0x22 };
			Assert::IsTrue(search(image.begin() + raw, image.begin() + raw + rawSize, expected.begin(), expected.end()) != image.begin() + raw + rawSize,
				L"largest COMDAT bytes are retained");
		}

		TEST_METHOD(ComdatNewestChoosesLastCandidate)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"old.obj", makeComdatObject("newest", { 0x31 }, 7));
			workspace.write(L"new.obj", makeComdatObject("newest", { 0x32 }, 7));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"old.obj", L"new.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x32) != image.begin() + raw + get32(image, text + 16), L"NEWEST COMDAT data is selected");
		}

		TEST_METHOD(AssociativeComdatFollowsSelectedParent)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"parent1.obj", makeAssociativeComdatObject({ 0xc3 }, { 0x41, 0x42 }));
			workspace.write(L"parent2.obj", makeAssociativeComdatObject({ 0xc3 }, { 0x51, 0x52 }));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"parent1.obj", L"parent2.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto xdata = sectionHeader(image, ".xdata");
			const auto raw = get32(image, xdata + 20);
			Assert::AreEqual<uint8_t>(0x41, image[raw], L"associative section follows the selected first parent");
			Assert::AreEqual<uint8_t>(0x42, image[raw + 1], L"associative section data is retained");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, xdata + 16), 0x51) == image.begin() + raw + get32(image, xdata + 16), L"losing parent's associative section is discarded");
		}

		TEST_METHOD(WeakExternalResolvesFallbackFromLibrary)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 1, 1 } };
			TestSymbol weak{ "weakRef", 0, 0, 105,{ weakAux(3, 3) } };
			TestSymbol fallback{ "fallback", 0, 0, 2,{} };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, weak, fallback }));
			workspace.write(L"fallback.lib", makeArchive({ { "fallback.obj", makeObject({ TestSection{ ".data",{ 0x77 }, 0xc0000040 } },{ TestSymbol{ "fallback", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"fallback.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto target = get64(image, get32(image, sectionHeader(image, ".text") + 20));
			const auto data = sectionHeader(image, ".data");
			const auto expected = get64(image, optionalHeader(image) + 24) + get32(image, data + 12);
			Assert::AreEqual<uint64_t>(expected, target, L"weak alias relocates to the selected library fallback");
			Assert::AreEqual<uint8_t>(0x77, image[get32(image, data + 20)], L"fallback member data is included");
		}

		TEST_METHOD(WeakSearchNoneResolvesToZero)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 1, 1 } };
			TestSymbol weak{ "optional", 0, 0, 105,{ weakAux(3, 1) } };
			TestSymbol fallback{ "missingFallback", 0, 0, 2,{} };
			const auto object = makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, weak, fallback });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			Assert::AreEqual<uint64_t>(0, get64(image, get32(image, sectionHeader(image, ".text") + 20)), L"unresolved weak external defaults to zero");
		}

		TEST_METHOD(ResponseFileAndLinkerOptionsAreHonored)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			workspace.writeText(L"link.rsp", L"/OUT:\"linked.exe\" /ENTRY:main /MACHINE:X64 /SUBSYSTEM:WINDOWS,6.2 /BASE:0x180000000 /NXCOMPAT:NO /INCREMENTAL:NO /Nologo input.obj");
			assertSuccess(runLinker(workspace, { L"@" + workspace.file(L"link.rsp").wstring() }));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint64_t>(0x180000000, get64(image, optional + 24), L"/BASE");
			Assert::AreEqual<uint16_t>(2, get16(image, optional + 68), L"/SUBSYSTEM:WINDOWS");
			Assert::AreEqual<uint16_t>(6, get16(image, optional + 48), L"subsystem major version");
			Assert::AreEqual<uint16_t>(2, get16(image, optional + 50), L"subsystem minor version");
			Assert::IsTrue((get16(image, optional + 70) & 0x100) == 0, L"/NXCOMPAT:NO");
		}

		TEST_METHOD(GnuShortOptionsAndMapOptionAreAccepted)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			const auto output = workspace.file(L"short.exe").wstring();
			const auto result = runLinker(workspace, { L"-o" + output, L"-emain", L"/MAP", workspace.file(L"input.obj").wstring() });
			assertSuccess(result);
			Assert::IsTrue(fs::exists(workspace.file(L"short.map")), L"/MAP creates a map beside the executable");
		}

		TEST_METHOD(LtcgAndLlvmBitcodeInputsAreExplicitlyUnsupported)
		{
			Workspace rawWorkspace;
			const auto rawBitcode = Bytes{ 'B', 'C', 0xc0, 0xde, 0x00, 0x00, 0x00, 0x00 };
			rawWorkspace.write(L"input.obj", rawBitcode);
			auto args = outputArgs(rawWorkspace);
			args.push_back(rawWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(rawWorkspace, args), "LLVM bitcode/LTCG input is not supported");
			assertNoOutput(rawWorkspace);

			Workspace wrappedWorkspace;
			const auto wrappedBitcode = Bytes{ 0xde, 0xc0, 0x17, 0x0b, 0x00, 0x00, 0x00, 0x00 };
			wrappedWorkspace.write(L"input.obj", wrappedBitcode);
			auto wrappedArgs = outputArgs(wrappedWorkspace);
			wrappedArgs.push_back(wrappedWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(wrappedWorkspace, wrappedArgs), "LLVM bitcode/LTCG input is not supported");

			Workspace sectionWorkspace;
			const auto sectionObject = makeObject({ TestSection{ ".llvmbc", rawBitcode, 0x40000040 }, TestSection{ ".text",{ 0xc3 } } },
				{ TestSymbol{ "main", 0, 2, 2,{} } });
			assertFailureContains(linkObject(sectionWorkspace, sectionObject), "LLVM bitcode/LTCG section '.llvmbc' is not supported");

			Workspace archiveWorkspace;
			archiveWorkspace.write(L"input.lib", makeArchive({ { "module.obj", rawBitcode } }));
			assertFailureContains(runLinker(archiveWorkspace, { L"/OUT:" + archiveWorkspace.file(L"linked.exe").wstring(), archiveWorkspace.file(L"input.lib").wstring() }),
				"input.lib(module.obj): LLVM bitcode/LTCG input is not supported");
		}

		TEST_METHOD(MalformedTruncatedObjectReportsErrorAndWritesNoImage)
		{
			Workspace workspace;
			workspace.write(L"input.obj", Bytes{ 0x64, 0x86, 0x01 });
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			const auto result = runLinker(workspace, args);
			assertFailureContains(result, "truncated");
			assertNoOutput(workspace);
		}

	};
}
