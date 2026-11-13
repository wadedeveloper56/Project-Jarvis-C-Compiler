#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(DiagnosticTests)
	{
	public:
		TEST_METHOD(UnresolvedExternalIsReported)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			const auto result = linkObject(workspace, makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "notFound", 0, 0, 2,{} } }));
			assertFailureContains(result, "notFound");
			assertFailureContains(result, "referenced by");
			assertFailureContains(result, "input.obj");
			Assert::AreEqual<DWORD>(1, result.exitCode);
			assertNoOutput(workspace);
		}

		TEST_METHOD(UnresolvedDiagnosticsIdentifyAllReferencingObjects)
		{
			Workspace workspace;
			const auto object = [](const string& symbol) {
				TestSection text{ ".text", Bytes(4, 0) };
				text.relocations = { { 0, 0, 4 } };
				return makeObject({ text }, { TestSymbol{ symbol, 0, 0, 2,{} } });
			};
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", object("missingA"));
			workspace.write(L"second.obj", object("missingA"));
			workspace.write(L"third.obj", object("missingB"));
			const auto result = runLinker(workspace, { L"/ENTRY:main", L"/OUT:missing.exe", L"entry.obj", L"first.obj", L"second.obj", L"third.obj" });
			assertFailureContains(result, "'missingA' (referenced by first.obj, second.obj)");
			assertFailureContains(result, "'missingB' (referenced by third.obj)");
			Assert::AreEqual<DWORD>(1, result.exitCode);
			Assert::IsFalse(fs::exists(workspace.file(L"missing.exe")));
		}

		TEST_METHOD(UnsupportedAmd64RelocationIsReported)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 0, 12 } };
			const auto result = linkObject(workspace, makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(result, "unsupported AMD64 relocation type");
		}

		TEST_METHOD(RelocationOutsideInitializedDataIsRejected)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3 } };
			text.relocations = { { 1, 0, 4 } };
			const auto result = linkObject(workspace, makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(result, "relocation offset is outside its section");
		}

		TEST_METHOD(Addr32OverflowIsRejected)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(4, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 2 } };
			const auto object = makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "does not fit its 32-bit relocation type");
		}

		TEST_METHOD(DuplicateStrongDefinitionsAreRejected)
		{
			Workspace workspace;
			workspace.write(L"first.obj", makeEntryObject());
			workspace.write(L"second.obj", makeEntryObject());
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"first.obj").wstring());
			args.push_back(workspace.file(L"second.obj").wstring());
			const auto result = runLinker(workspace, args);
			assertFailureContains(result, "duplicate definition");
			assertFailureContains(result, "first.obj");
			assertFailureContains(result, "second.obj");
			Assert::AreEqual<DWORD>(1, result.exitCode);
			assertNoOutput(workspace);
		}

		TEST_METHOD(ComdatNoDuplicatesAndSizeMismatchAreRejected)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeComdatObject("duplicate", { 0x11 }, 1));
			workspace.write(L"second.obj", makeComdatObject("duplicate", { 0x11 }, 1));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertFailureContains(runLinker(workspace, args), "duplicate NODUPLICATES COMDAT");
			Workspace sizeWorkspace;
			sizeWorkspace.write(L"entry.obj", makeEntryObject());
			sizeWorkspace.write(L"first.obj", makeComdatObject("sameSize", { 0x11 }, 3));
			sizeWorkspace.write(L"second.obj", makeComdatObject("sameSize", { 0x11, 0x22 }, 3));
			auto sizeArgs = outputArgs(sizeWorkspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) sizeArgs.push_back(sizeWorkspace.file(file).wstring());
			assertFailureContains(runLinker(sizeWorkspace, sizeArgs), "SAME_SIZE COMDAT mismatch");
		}

		TEST_METHOD(ComdatExactMatchMismatchIsRejected)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeComdatObject("exact", { 0x11 }, 4));
			workspace.write(L"second.obj", makeComdatObject("exact", { 0x22 }, 4));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertFailureContains(runLinker(workspace, args), "EXACT_MATCH COMDAT mismatch");
		}

		TEST_METHOD(InvalidWeakSearchModeIsRejected)
		{
			Workspace workspace;
			TestSymbol weak{ "weakRef", 0, 0, 105,{ weakAux(3, 4) } };
			TestSymbol fallback{ "fallback", 0, 0, 2,{} };
			const auto result = linkObject(workspace, makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} }, weak, fallback }));
			assertFailureContains(result, "unsupported weak-external search mode");
		}

		TEST_METHOD(MalformedArchiveAndMalformedImportObjectAreRejected)
		{
			Workspace workspace;
			workspace.write(L"bad.lib", Bytes{ 'n', 'o', 'p', 'e' });
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"bad.lib").wstring());
			assertFailureContains(runLinker(workspace, args), "truncated");
			Workspace importWorkspace;
			TestSection text{ ".text",{ 0xff, 0x25, 0, 0, 0, 0 } };
			text.relocations = { { 2, 1, 4 } };
			importWorkspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "__imp_bad", 0, 0, 2,{} } }));
			auto badImport = makeImportObject("bad", "BAD.dll", "bad", 0, 4);
			put16(badImport, 6, 0x14c);
			importWorkspace.write(L"bad.lib", makeArchive({ { "bad.obj", badImport } }));
			auto importArgs = outputArgs(importWorkspace);
			importArgs.push_back(importWorkspace.file(L"input.obj").wstring());
			importArgs.push_back(importWorkspace.file(L"bad.lib").wstring());
			assertFailureContains(runLinker(importWorkspace, importArgs), "import library member machine does not match the target");
		}

		TEST_METHOD(MalformedAndUnsupportedCoffDirectivesAreReported)
		{
			Workspace workspace;
			const auto unknown = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-not-a-linker-option:value") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			const auto unknownResult = linkObject(workspace, unknown);
			assertFailureContains(unknownResult, "unsupported .drectve option");
			assertNoOutput(workspace);

			Workspace alignmentWorkspace;
			const auto invalidAlignment = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-align:300") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			assertFailureContains(linkObject(alignmentWorkspace, invalidAlignment), ".drectve /ALIGN must be a power of two");

			Workspace quoteWorkspace;
			const auto malformedQuote = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-defaultlib:\"unterminated") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			assertFailureContains(linkObject(quoteWorkspace, malformedQuote), "unterminated quote in .drectve");

			Workspace exportWorkspace;
			const auto malformedExport = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-export:api,@1,@2") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 1, 2,{} } });
			assertFailureContains(linkObject(exportWorkspace, malformedExport), "specifies more than one ordinal");
		}

		TEST_METHOD(ResponseFilesHandleNestingEncodingAndCycles)
		{
			Workspace workspace;
			workspace.write(L"input file.obj", makeEntryObject());
			workspace.writeText(L"outer.rsp", L"@inner.rsp /OUT:first.exe");
			workspace.writeText(L"inner.rsp", L"/ENTRY:main \"input file.obj\" /OUT:earlier.exe");
			assertSuccess(runLinker(workspace, { L"@outer.rsp", L"/OUT:last.exe" }));
			Assert::IsTrue(fs::exists(workspace.file(L"last.exe")), L"the last output option wins across nested response files");
			Assert::IsFalse(fs::exists(workspace.file(L"earlier.exe")));
			assertSuccess(runLinker(workspace, { L"@inner.rsp", L"/OUT:map.exe", L"/MAP:explicit.map", L"/MAP" }));
			Assert::IsTrue(fs::exists(workspace.file(L"map.map")));
			Assert::IsFalse(fs::exists(workspace.file(L"explicit.map")));
			workspace.writeText(L"commands.lnk", L"/ENTRY:main /OUT:lnk.exe \"input file.obj\"");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"@commands" }, { { L"commands", L"/UNKNOWN" } }));
			Assert::IsTrue(fs::exists(workspace.file(L"lnk.exe")), L"the .lnk file wins over an environment variable");
			const string command = "/ENTRY:main /OUT:utf8.exe \"input file.obj\"";
			Bytes utf8{ 0xef, 0xbb, 0xbf };
			utf8.insert(utf8.end(), command.begin(), command.end());
			workspace.write(L"utf8.rsp", utf8);
			assertSuccess(runLinker(workspace, { L"@utf8.rsp" }));
			Assert::IsTrue(fs::exists(workspace.file(L"utf8.exe")));
			const string command16 = "/ENTRY:main /OUT:utf16.exe \"input file.obj\"";
			Bytes utf16{ 0xff, 0xfe };
			for (const auto ch : command16) { utf16.push_back(static_cast<uint8_t>(ch)); utf16.push_back(0); }
			workspace.write(L"utf16.rsp", utf16);
			assertSuccess(runLinker(workspace, { L"@utf16.rsp" }));
			Assert::IsTrue(fs::exists(workspace.file(L"utf16.exe")));
			workspace.write(L"invalid.rsp", Bytes{ 0xef, 0xbb, 0xbf, 0xff });
			assertFailureContains(runLinker(workspace, { L"@invalid.rsp" }), "invalid UTF-8");
			workspace.write(L"odd.rsp", Bytes{ 0xff, 0xfe, 0x41 });
			assertFailureContains(runLinker(workspace, { L"@odd.rsp" }), "invalid UTF-16LE");
			workspace.writeText(L"cycle.rsp", L"@CYCLE.RSP");
			assertFailureContains(runLinker(workspace, { L"@cycle.rsp" }), "cycle involving");
		}

		TEST_METHOD(JwlinkCommandEnvironmentExpansionAndLibrarySearch)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "needed", 0, 0, 2,{} } }));
			for (const auto* dir : { L"first", L"second", L"env" }) fs::create_directories(workspace.file(dir));
			auto archive = [](uint8_t marker) { return makeArchive({ { "lib.obj", makeObject({ TestSection{ ".text",{ marker, 0xc3 } } }, { TestSymbol{ "needed", 0, 1, 2,{} } }) } }); };
			workspace.write(L"first\\x.lib", archive(0x71));
			workspace.write(L"second\\x.lib", archive(0x72));
			workspace.write(L"env\\x.lib", archive(0x73));
			const vector<pair<wstring, wstring>> env{ { L"LIBPE", L"" }, { L"LIB386", L"" }, { L"LIB", workspace.file(L"env").wstring() }, { L"JWL2_CMDS", L"format win pe option quiet,start=main file input.obj library x.lib name env.exe" }, { L"JWL2_DIR", L"format win pe" } };
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"@JWL2_CMDS" }, env));
			const auto fromLib = workspace.read(L"env.exe");
			Assert::AreEqual<uint8_t>(0x73, fromLib[get32(fromLib, sectionHeader(fromLib, ".text") + 20) + 1], L"LIB provides libraries when no explicit path matches");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"%JWL2_DIR%", L"option", L"start=main", L"file", L"input.obj", L"libpath", L"first", L"libpath", L"second", L"library", L"x.lib", L"name", L"order.exe" }, env));
			const auto image = workspace.read(L"order.exe");
			const auto raw = get32(image, sectionHeader(image, ".text") + 20);
			Assert::AreEqual<uint8_t>(0x72, image[raw + 1], L"last LIBPATH wins over earlier paths and LIB");
			const vector<pair<wstring, wstring>> orderedEnv{ { L"LIBPE", workspace.file(L"first").wstring() }, { L"LIB386", workspace.file(L"second").wstring() }, { L"LIB", workspace.file(L"env").wstring() } };
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"format", L"win", L"pe", L"option", L"start=main", L"file", L"input.obj", L"library", L"x.lib", L"name", L"envorder.exe" }, orderedEnv));
			const auto envImage = workspace.read(L"envorder.exe");
			Assert::AreEqual<uint8_t>(0x71, envImage[get32(envImage, sectionHeader(envImage, ".text") + 20) + 1], L"LIBPE precedes LIB386 and LIB");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"/ENTRY:main", L"/OUT:switchorder.exe", L"/LIBPATH:first", L"/LIBPATH:second", L"input.obj", L"x.lib" }, orderedEnv));
			const auto switchImage = workspace.read(L"switchorder.exe");
			Assert::AreEqual<uint8_t>(0x71, switchImage[get32(switchImage, sectionHeader(switchImage, ".text") + 20) + 1], L"switch-style paths retain command-line order");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"/ENTRY:main", L"/OUT:explicit.exe", L"/LIBPATH:first", L"input.obj", L"second\\x.lib" }, orderedEnv));
			const auto explicitImage = workspace.read(L"explicit.exe");
			Assert::AreEqual<uint8_t>(0x72, explicitImage[get32(explicitImage, sectionHeader(explicitImage, ".text") + 20) + 1], L"path-qualified libraries bypass search paths");
			workspace.write(L"x.lib", archive(0x74));
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"format", L"win", L"pe", L"option", L"start=main", L"file", L"input.obj", L"libpath", L"first", L"library", L"x.lib", L"name", L"current.exe" }, orderedEnv));
			const auto currentImage = workspace.read(L"current.exe");
			Assert::AreEqual<uint8_t>(0x74, currentImage[get32(currentImage, sectionHeader(currentImage, ".text") + 20) + 1], L"current directory precedes library paths");
			workspace.write(L"plain.obj", makeEntryObject());
			assertSuccess(runExecutable(workspace, jwlinkPath(), { L"%JWL2_DIR%", L"option", L"quiet,start=main", L"file", L"plain.obj", L"name", L"original.exe" }, env));
			Assert::IsTrue(fs::exists(workspace.file(L"original.exe")), L"JWlink accepts the same percent environment expansion");
			const vector<pair<wstring, wstring>> sourceEnv{ { L"JWL2_SOURCE", L"format win pe option quiet,start=main file plain.obj name originalenv.exe" } };
			assertSuccess(runExecutable(workspace, jwlinkPath(), { L"@JWL2_SOURCE" }, sourceEnv));
			Assert::IsTrue(fs::exists(workspace.file(L"originalenv.exe")), L"JWlink accepts environment command sources");
			const vector<pair<wstring, wstring>> cycle{ { L"JWL2_CYCLE", L"@JWL2_CYCLE" } };
			assertFailureContains(runExecutable(workspace, jwlink2Path(), { L"@JWL2_CYCLE" }, cycle), "cycle involving");
			assertFailureContains(runExecutable(workspace, jwlink2Path(), { L"%JWL2_MISSING%" }, { { L"JWL2_MISSING", L"" } }), "not defined");
		}

		TEST_METHOD(CommandLineAndResponseFileErrorsAreReported)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			assertFailureContains(runLinker(workspace, { L"/UNKNOWN", workspace.file(L"input.obj").wstring() }), "unsupported option");
			assertFailureContains(runLinker(workspace, { L"/MANIFESTINPUT:app.manifest", workspace.file(L"input.obj").wstring() }), "app.manifest");
			assertFailureContains(runLinker(workspace, { L"/BASE:0x12345", workspace.file(L"input.obj").wstring() }), "64-KiB-aligned");
			assertFailureContains(runLinker(workspace, { L"/ALIGN:300", workspace.file(L"input.obj").wstring() }), "/ALIGN must be a power of two");
			assertFailureContains(runLinker(workspace, { L"/SUBSYSTEM:EFI", workspace.file(L"input.obj").wstring() }), "unsupported subsystem");
			assertFailureContains(runLinker(workspace, { L"/MACHINE:ARM64", workspace.file(L"input.obj").wstring() }), "supports /MACHINE:X64 or /MACHINE:X86");
			assertFailureContains(runLinker(workspace, { L"/INCREMENTAL", workspace.file(L"input.obj").wstring() }), "INCREMENTAL:NO");
			assertFailureContains(runLinker(workspace, { L"@missing.rsp" }), "cannot open response file");
			workspace.writeText(L"bad.rsp", L"/OUT:\"unterminated");
			assertFailureContains(runLinker(workspace, { L"@" + workspace.file(L"bad.rsp").wstring() }), "unterminated quote");
		}

		TEST_METHOD(MissingInputsAndMissingFilesAreReported)
		{
			Workspace workspace;
			assertFailureContains(runLinker(workspace, {}), "no input object files or libraries");
			assertFailureContains(runLinker(workspace, { L"missing.obj" }), "cannot open");
			const vector<wstring> args{ L"format", L"win", L"pe", L"option", L"quiet,start=main", L"file", L"missing.obj", L"name", L"missing.exe" };
			const auto jwlink = runExecutable(workspace, jwlinkPath(), args);
			const auto jwlink2 = runLinker(workspace, args);
			Assert::AreEqual<DWORD>(1, jwlink.exitCode, L"JWlink reports a link failure with exit code 1");
			Assert::AreEqual(jwlink.exitCode, jwlink2.exitCode, L"JWlink2 reports the same failure status");
			assertFailureContains(jwlink2, "missing.obj");
			Assert::IsFalse(fs::exists(workspace.file(L"missing.exe")));
		}
	};
}
