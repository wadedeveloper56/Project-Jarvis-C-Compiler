#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(ImageTests)
	{
	public:
		TEST_METHOD(ValidAmd64ObjectProducesPe32PlusWithDefaultBaseAndEntry)
		{
			Workspace workspace;
			assertSuccess(linkObject(workspace, makeEntryObject()));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint16_t>(0x8664, get16(image, 0x84), L"AMD64 machine");
			Assert::AreEqual<uint64_t>(0x140000000, get64(image, optional + 24), L"default image base");
			Assert::AreEqual<uint32_t>(0x1000, get32(image, optional + 16), L"default entry point RVA");
			Assert::AreEqual<uint16_t>(3, get16(image, optional + 68), L"default console subsystem");
			Assert::IsTrue((get16(image, 0x96) & 0x2000) == 0, L"supported output is a PE executable, not a DLL");
		}

		TEST_METHOD(WindowsSubsystemSelectsGuiStartupAndKeepsExplicitEntry)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb8, 11, 0, 0, 0, 0xc3, 0xb8, 22, 0, 0, 0, 0xc3, 0xb8, 33, 0, 0, 0, 0xc3 } };
			workspace.write(L"entry.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "mainCRTStartup", 6, 1, 2,{} }, TestSymbol{ "WinMainCRTStartup", 12, 1, 2,{} } }));
			struct Case { vector<wstring> args; const wchar_t* name; DWORD result; uint16_t subsystem; };
			const vector<Case> cases{
				{ { L"/subsystem:windows", L"/out:gui.exe", L"entry.obj" }, L"gui.exe", 33, 2 },
				{ { L"/subsystem:console", L"/out:console.exe", L"entry.obj" }, L"console.exe", 22, 3 },
				{ { L"/subsystem:windows", L"/entry:main", L"/out:explicit.exe", L"entry.obj" }, L"explicit.exe", 11, 2 },
				{ { L"/subsystem:windows", L"/subsystem:console", L"/out:last.exe", L"entry.obj" }, L"last.exe", 22, 3 }
			};
			for (const auto& test : cases)
			{
				assertSuccess(runLinker(workspace, test.args));
				const auto image = workspace.read(test.name);
				Assert::AreEqual(test.subsystem, get16(image, optionalHeader(image) + 68));
				Assert::AreEqual(test.result, runExecutable(workspace, workspace.file(test.name), {}).exitCode);
			}
			workspace.write(L"startup.lib", makeArchive({ { "startup.obj", makeObject({ TestSection{ ".text",{ 0xb8, 44, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "WinMainCRTStartup", 0, 1, 2,{} } }) } }));
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/out:archive.exe", L"startup.lib" }));
			Assert::AreEqual<DWORD>(44, runExecutable(workspace, workspace.file(L"archive.exe"), {}).exitCode, L"default GUI startup pulls its archive member");
			workspace.write(L"console-start.lib", makeArchive({ { "startup.obj", makeObject({ TestSection{ ".text",{ 0xb8, 66, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "mainCRTStartup", 0, 1, 2,{} } }) } }));
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/out:archive-fallback.exe", L"console-start.lib" }));
			Assert::AreEqual<DWORD>(66, runExecutable(workspace, workspace.file(L"archive-fallback.exe"), {}).exitCode, L"existing archive-supplied mainCRTStartup fallback remains usable");
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/out:both-archives.exe", L"console-start.lib", L"startup.lib" }));
			Assert::AreEqual<DWORD>(44, runExecutable(workspace, workspace.file(L"both-archives.exe"), {}).exitCode, L"GUI startup takes priority when both archive candidates exist");
			workspace.write(L"fallback.obj", makeObject({ TestSection{ ".text",{ 0xb8, 55, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/out:fallback.exe", L"fallback.obj", L"startup.lib" }));
			Assert::AreEqual<DWORD>(55, runExecutable(workspace, workspace.file(L"fallback.exe"), {}).exitCode, L"an already defined main keeps the existing fallback");
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/entry:WinMainCRTStartup", L"/out:forced.exe", L"fallback.obj", L"startup.lib" }));
			Assert::AreEqual<DWORD>(44, runExecutable(workspace, workspace.file(L"forced.exe"), {}).exitCode, L"explicit GUI startup pulls the archive even when main exists");
			workspace.write(L"bare.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "WinMain", 0, 1, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/subsystem:windows", L"bare.obj" }), "WinMainCRTStartup");
		}

		TEST_METHOD(UnclassifiedSectionIsNotCountedAsInitializedData)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".extra",{ 1, 2, 3 }, 0x40000000 }, TestSection{ ".rdata",{ 4, 5 }, 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			Assert::AreEqual(get32(image, sectionHeader(image, ".rdata") + 16), get32(image, optionalHeader(image) + 8), L"only initialized-data sections contribute to SizeOfInitializedData");
			validatePeLayout(image);
		}

		TEST_METHOD(RealGuiExecutableCreatesHiddenWindow)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"cl.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe") && fs::exists(entry.path().parent_path() / L"dumpbin.exe")) toolDirectory = entry.path().parent_path();
			}
			wchar_t programFilesX86[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles(x86)", programFilesX86, MAX_PATH);
			const auto sdkRoot = fs::path(programFilesX86) / L"Windows Kits" / L"10" / L"Lib";
			fs::path sdk;
			for (const auto& entry : fs::directory_iterator(sdkRoot, ignored))
			{
				const auto candidate = entry.path() / L"um" / L"x64";
				if (fs::exists(candidate / L"user32.lib") && fs::exists(candidate / L"kernel32.lib") && (sdk.empty() || candidate.wstring() > sdk.wstring())) sdk = candidate;
			}
			if (toolDirectory.empty() || sdk.empty()) { Logger::WriteMessage("MSVC x64 tools or Windows SDK import libraries not found; GUI integration test skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"gui.c", L"typedef void *HANDLE; typedef unsigned short WCHAR;\n"
				L"__declspec(dllimport) HANDLE __stdcall CreateWindowExW(unsigned long,const WCHAR*,const WCHAR*,unsigned long,int,int,int,int,HANDLE,HANDLE,HANDLE,void*);\n"
				L"__declspec(dllimport) int __stdcall IsWindow(HANDLE);\n"
				L"__declspec(dllimport) int __stdcall DestroyWindow(HANDLE);\n"
				L"__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned int);\n"
				L"void WinMainCRTStartup(void) { HANDLE w = CreateWindowExW(0,L\"STATIC\",L\"JWlink2 test\",0,0,0,16,16,0,0,0,0); if (!w) ExitProcess(41); if (!IsWindow(w)) ExitProcess(42); if (!DestroyWindow(w)) ExitProcess(43); ExitProcess(0); }\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"/Fogui.obj", L"gui.c" }));
			const auto user32 = (sdk / L"user32.lib").wstring();
			const auto kernel32 = (sdk / L"kernel32.lib").wstring();
			assertSuccess(runLinker(workspace, { L"/nodefaultlib", L"/subsystem:windows,6.0", L"/out:gui.exe", L"gui.obj", user32, kernel32 }));
			assertSuccess(runLinker(workspace, { L"system", L"nt_win", L"option", L"start=WinMainCRTStartup", L"file", L"gui.obj", L"library", user32, kernel32, L"name", L"directive.exe" }));
			assertSuccess(runLinker(workspace, { L"runtime", L"windows=6.0", L"option", L"start=WinMainCRTStartup", L"file", L"gui.obj", L"library", user32, kernel32, L"name", L"runtime.exe" }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:windows,6.0", L"/entry:WinMainCRTStartup", L"/out:msvc.exe", L"gui.obj", user32, kernel32 }));
			for (const auto* name : { L"gui.exe", L"directive.exe", L"runtime.exe", L"msvc.exe" })
			{
				const auto image = workspace.read(name);
				if (wstring_view(name) != L"msvc.exe") validatePeLayout(image);
				Assert::AreEqual<uint16_t>(0x8664, get16(image, peHeader(image) + 4), L"AMD64 machine");
				Assert::AreEqual<uint16_t>(0x20b, get16(image, optionalHeader(image)), L"PE32+ image");
				Assert::AreEqual<uint16_t>(2, get16(image, optionalHeader(image) + 68), L"Windows GUI subsystem");
				Assert::AreEqual<uint16_t>(6, get16(image, optionalHeader(image) + 48), L"subsystem major version");
				Assert::AreEqual<uint16_t>(0, get16(image, optionalHeader(image) + 50), L"subsystem minor version");
				Assert::IsTrue((get16(image, peHeader(image) + 22) & 0x2000) == 0, L"image is an EXE");
				Assert::IsTrue(get32(image, optionalHeader(image) + 16) != 0, L"entry RVA is nonzero");
				const auto inspection = runExecutable(workspace, toolDirectory / L"dumpbin.exe", { L"/nologo", L"/headers", L"/imports", workspace.file(name).wstring() });
				assertSuccess(inspection);
				for (const auto* expected : { "8664 machine (x64)", "PE32+", "Windows GUI", "USER32.dll", "CreateWindowExW", "DestroyWindow" }) Assert::IsTrue(inspection.output.find(expected) != string::npos, wstring(expected, expected + strlen(expected)).c_str());
				Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(name), {}).exitCode, L"GUI process creates, checks, and destroys a hidden window");
			}
		}

		TEST_METHOD(DllExportsGenerateRoundTrippableImportLibrary)
		{
			Workspace workspace;
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/EXPORT:api", L"/IMPLIB:api-imports.lib", workspace.file(L"dll.obj").wstring() }));
			const auto dll = workspace.read(L"a.dll");
			const auto pe = peHeader(dll);
			const auto optional = optionalHeader(dll);
			Assert::IsTrue((get16(dll, pe + 22) & 0x2000) != 0, L"DLL characteristic is set");
			Assert::AreEqual<uint32_t>(0, get32(dll, optional + 16), L"DLL without a startup routine has no entry point");
			const auto exportRva = get32(dll, optional + 112);
			Assert::IsTrue(exportRva != 0, L"export directory is present");
			const auto exportDirectory = rvaOffset(dll, exportRva);
			Assert::IsTrue(cStringAtRva(dll, get32(dll, exportDirectory + 12)) == "a.dll", L"export directory module name");
			Assert::IsTrue(fs::exists(workspace.file(L"api-imports.lib")), L"/IMPLIB selects the generated import-library path");

			TestSection clientText{ ".text", Bytes(16, 0) };
			clientText.relocations = { { 0, 1, 4 },{ 8, 2, 4 } };
			workspace.write(L"client.obj", makeObject({ clientText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} }, TestSymbol{ "__imp_api", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"client.exe").wstring(), L"/ENTRY:main", workspace.file(L"client.obj").wstring(), workspace.file(L"api-imports.lib").wstring() }));
			const auto client = workspace.read(L"client.exe");
			const auto clientTextHeader = sectionHeader(client, ".text");
			const auto textRaw = get32(client, clientTextHeader + 20);
			const auto textRva = get32(client, clientTextHeader + 12);
			const auto apiThunkRva = static_cast<uint32_t>(static_cast<int64_t>(textRva) + 4 + static_cast<int32_t>(get32(client, textRaw)));
			const auto apiOffset = rvaOffset(client, apiThunkRva);
			Assert::AreEqual<uint8_t>(0xff, client[apiOffset], L"import library provides a callable function thunk");
			const auto importRva = get32(client, optionalHeader(client) + 120);
			Assert::IsTrue(importRva != 0, L"round-trip link includes import descriptors");
			const auto descriptor = rvaOffset(client, importRva);
			Assert::IsTrue(cStringAtRva(client, get32(client, descriptor + 12)) == "a.dll", L"generated import object names its DLL");
		}

		TEST_METHOD(DefinitionFileAndDllEntryPointOptionsAreHonored)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3, 0xc3 } };
			TestSection data{ ".data",{ 0x5a }, 0xc0000040 };
			workspace.write(L"dll.obj", makeObject({ text, data }, {
				TestSymbol{ "DllMainCRTStartup", 0, 1, 2,{} }, TestSymbol{ "internalFn", 1, 1, 2,{} }, TestSymbol{ "globalData", 0, 2, 2,{} }
				}));
			workspace.writeText(L"module.def", L"LIBRARY fixture\nEXPORTS\n api=internalFn @5\n table=globalData @8 DATA\n ordinal=internalFn @11 NONAME\n hidden=internalFn PRIVATE\n");
			assertSuccess(runLinker(workspace, { L"/DLL", L"/DEF:" + workspace.file(L"module.def").wstring(), L"/OUT:" + workspace.file(L"custom.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			const auto dll = workspace.read(L"custom.dll");
			const auto optional = optionalHeader(dll);
			const auto entryRva = get32(dll, optional + 16);
			Assert::AreEqual<uint32_t>(get32(dll, sectionHeader(dll, ".text") + 12), entryRva, L"DLL defaults to DllMainCRTStartup when present");
			const auto exportDirectory = rvaOffset(dll, get32(dll, optional + 112));
			Assert::IsTrue(cStringAtRva(dll, get32(dll, exportDirectory + 12)) == "fixture.dll", L".def LIBRARY name controls the export module name");
			Assert::AreEqual<uint32_t>(11, get32(dll, exportDirectory + 20), L"export ordinal table spans assigned ordinals");
			Assert::AreEqual<uint32_t>(3, get32(dll, exportDirectory + 24), L"named exports include DATA and PRIVATE but omit NONAME");
			Assert::IsTrue(fs::exists(workspace.file(L"custom.lib")), L".def DLL emits the default import library");

			TestSection clientText{ ".text", Bytes(20, 0) };
			clientText.relocations = { { 0, 1, 4 },{ 8, 2, 4 },{ 16, 3, 4 } };
			workspace.write(L"client.obj", makeObject({ clientText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} }, TestSymbol{ "__imp_table", 0, 0, 2,{} }, TestSymbol{ "ordinal", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"client.exe").wstring(), L"/ENTRY:main", workspace.file(L"client.obj").wstring(), workspace.file(L"custom.lib").wstring() }));
			const auto client = workspace.read(L"client.exe");
			const auto importRva = get32(client, optionalHeader(client) + 120);
			const auto descriptor = rvaOffset(client, importRva);
			Assert::IsTrue(cStringAtRva(client, get32(client, descriptor + 12)) == "fixture.dll", L".def module name is carried into generated imports");
			const auto clientTextHeader = sectionHeader(client, ".text");
			const auto textRaw = get32(client, clientTextHeader + 20);
			const auto textRva = get32(client, clientTextHeader + 12);
			const auto dataIatRva = static_cast<uint32_t>(static_cast<int64_t>(textRva) + 12 + static_cast<int32_t>(get32(client, textRaw + 8)));
			const auto lookupOffset = rvaOffset(client, get32(client, descriptor));
			const auto iatRva = get32(client, descriptor + 16);
			bool foundDataImport = false;
			for (size_t index = 0;; ++index)
			{
				const auto nameRva = get64(client, lookupOffset + index * 8);
				if (nameRva == 0) break;
				if ((nameRva & (uint64_t{ 1 } << 63)) != 0) continue;
				const auto nameOffset = rvaOffset(client, static_cast<uint32_t>(nameRva)) + 2;
				const string importedName(reinterpret_cast<const char*>(client.data() + nameOffset));
				if (importedName == "table")
				{
					Assert::AreEqual<uint32_t>(iatRva + static_cast<uint32_t>(index * 8), dataIatRva, L"DATA import resolves exactly to its generated IAT slot");
					foundDataImport = true;
				}
			}
			Assert::IsTrue(foundDataImport, L"DATA import is present in the generated lookup table");
			const auto ordinalLookup = rvaOffset(client, get32(client, descriptor));
			bool foundOrdinalImport = false;
			for (size_t index = 0;; ++index)
			{
				const auto thunk = get64(client, ordinalLookup + index * 8);
				if (thunk == 0) break;
				if ((thunk & (uint64_t{ 1 } << 63)) != 0 && (thunk & 0xffff) == 11) foundOrdinalImport = true;
			}
			Assert::IsTrue(foundOrdinalImport, L"NONAME export is imported by its assigned ordinal");
			TestSection privateText{ ".text", Bytes(8, 0) };
			privateText.relocations = { { 0, 1, 4 } };
			workspace.write(L"private-client.obj", makeObject({ privateText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "hidden", 0, 0, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/OUT:" + workspace.file(L"private-client.exe").wstring(), L"/ENTRY:main", workspace.file(L"private-client.obj").wstring(), workspace.file(L"custom.lib").wstring() }), "unresolved external symbol 'hidden'");
		}

		TEST_METHOD(DllNoEntryAndInvalidImportLibraryRequestsAreHandled)
		{
			Workspace workspace;
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:api", L"/OUT:" + workspace.file(L"noentry.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			const auto noEntryImage = workspace.read(L"noentry.dll");
			Assert::AreEqual<uint32_t>(0, get32(noEntryImage, optionalHeader(noEntryImage) + 16), L"/NOENTRY leaves the DLL entry RVA zero");
			assertSuccess(runLinker(workspace, { L"/DLL", L"/ENTRY:api", L"/EXPORT:api", L"/OUT:" + workspace.file(L"explicit.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			const auto explicitEntryImage = workspace.read(L"explicit.dll");
			Assert::AreEqual<uint32_t>(get32(explicitEntryImage, sectionHeader(explicitEntryImage, ".text") + 12), get32(explicitEntryImage, optionalHeader(explicitEntryImage) + 16), L"/ENTRY selects an explicit DLL entry point");
			Workspace invalidWorkspace;
			invalidWorkspace.write(L"plain.obj", makeEntryObject());
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/IMPLIB:requested.lib", L"/OUT:" + invalidWorkspace.file(L"empty.dll").wstring(), invalidWorkspace.file(L"plain.obj").wstring() }), "DLL has no importable exports");
			assertNoOutput(invalidWorkspace, L"empty.dll");
			Assert::IsFalse(fs::exists(invalidWorkspace.file(L"requested.lib")), L"failed DLL link does not create an import library");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/NOENTRY", L"/ENTRY:main", invalidWorkspace.file(L"plain.obj").wstring() }), "/NOENTRY is valid only for a DLL without /ENTRY");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/EXPORT:missing", invalidWorkspace.file(L"plain.obj").wstring() }), "unresolved external symbol 'missing'");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/EXPORT:main", L"/EXPORT:main", invalidWorkspace.file(L"plain.obj").wstring() }), "duplicate export name 'main'");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/EXPORT:main", L"/OUT:" + invalidWorkspace.file(L"same.dll").wstring(), L"/IMPLIB:" + invalidWorkspace.file(L"same.dll").wstring(), invalidWorkspace.file(L"plain.obj").wstring() }), "import-library output path must differ");
		}

		TEST_METHOD(LinkedImagesSatisfyPeLayoutInvariants)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(16, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 } };
			TestSection text{ ".text",{ 0xc3 } };
			const auto object = makeObject({ data, text }, { TestSymbol{ "main", 0, 2, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			validatePeLayout(workspace.read(L"linked.exe"));

			auto args = outputArgs(workspace, L"aligned.exe");
			args.insert(args.begin() + 1, L"/ALIGN:8192");
			args.push_back(workspace.file(L"input.obj").wstring());
			workspace.write(L"input.obj", object);
			assertSuccess(runLinker(workspace, args));
			validatePeLayout(workspace.read(L"aligned.exe"));

			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:api", L"/OUT:" + workspace.file(L"layout.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			validatePeLayout(workspace.read(L"layout.dll"));
		}

		TEST_METHOD(StaticComdatSectionsWithoutExternalSymbolsAreKept)
		{
			Workspace workspace;
			auto staticComdat = [](uint8_t marker)
				{
					TestSection section{ ".text$mn", { marker }, 0x60001020 };
					TestSymbol sectionSymbol{ ".text$mn", 0, 1, 3,{ sectionAux(1, 0, 2) } };
					return makeObject({ section }, { sectionSymbol });
				};
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", staticComdat(0x11));
			workspace.write(L"second.obj", staticComdat(0x22));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			const auto end = raw + get32(image, text + 16);
			Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x11) != image.begin() + end, L"first static COMDAT is kept");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x22) != image.begin() + end, L"static COMDATs from different objects never merge");
		}

		TEST_METHOD(DllIsRebasedByTheWindowsLoaderAndRelocationsAreApplied)
		{
			Workspace workspace;
			// get: mov rax,[rip+slot]; ret    where: lea rax,[rip+slot]; ret
			TestSection text{ ".text",{ 0x48, 0x8b, 0x05, 0, 0, 0, 0, 0xc3, 0x48, 0x8d, 0x05, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 3, 2, 4 },{ 11, 2, 4 } };
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 2, 1 } };
			workspace.write(L"dll.obj", makeObject({ text, data }, { TestSymbol{ "get", 0, 1, 2,{} }, TestSymbol{ "where", 8, 1, 2,{} }, TestSymbol{ "slot", 0, 2, 2,{} } }));
			for (const auto* name : { L"one.dll", L"two.dll" })
			{
				assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/BASE:0x180000000", L"/DYNAMICBASE", L"/EXPORT:get", L"/EXPORT:where", L"/OUT:" + workspace.file(name).wstring(), workspace.file(L"dll.obj").wstring() }));
			}
			const auto image = workspace.read(L"one.dll");
			validatePeLayout(image);
			Assert::IsTrue((get16(image, optionalHeader(image) + 70) & 0x40) != 0, L"DLL advertises ASLR");

			using Getter = uintptr_t(*)();
			vector<HMODULE> modules;
			for (const auto* name : { L"one.dll", L"two.dll" })
			{
				const auto module = LoadLibraryExW(workspace.file(name).c_str(), nullptr, 0);
				Assert::IsNotNull(module, (wstring(L"Windows loader accepts ") + name).c_str());
				modules.push_back(module);
				const auto get = reinterpret_cast<Getter>(GetProcAddress(module, "get"));
				const auto where = reinterpret_cast<Getter>(GetProcAddress(module, "where"));
				Assert::IsTrue(get && where, L"exports resolve through the loader");
				Assert::AreEqual<uintptr_t>(where(), get(), L"DIR64 relocation tracks the load address");
				const auto base = reinterpret_cast<uintptr_t>(module);
				Assert::IsTrue(where() >= base && where() < base + get32(image, optionalHeader(image) + 56), L"relocated pointer lies inside the loaded image");
			}
			Assert::IsTrue(modules[0] != modules[1], L"two images with one preferred base load at distinct addresses");
			for (const auto module : modules) FreeLibrary(module);
		}

		TEST_METHOD(DumpbinAcceptsLinkedImagesAndReportsExpectedStructure)
		{
			fs::path dumpbin;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			if (programFiles)
			{
				for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
				{
					if (entry.path().filename() == L"dumpbin.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64") dumpbin = entry.path();
				}
			}
			if (dumpbin.empty()) Logger::WriteMessage("dumpbin.exe not found; standard-tool inspection skipped");
			if (dumpbin.empty()) return;

			Workspace workspace;
			TestSection data{ ".data", Bytes(16, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 } };
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 2, 4 } };
			workspace.write(L"input.obj", makeObject({ data, text }, { TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "slot", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			args.insert(args.begin() + 1, L"/BASE:0x140000000");
			assertSuccess(runLinker(workspace, args));
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:api", L"/OUT:" + workspace.file(L"tool.dll").wstring(), workspace.file(L"dll.obj").wstring() }));

			const auto exe = runExecutable(workspace, dumpbin, { L"/NOLOGO", L"/HEADERS", L"/IMPORTS", L"/RELOCATIONS", workspace.file(L"linked.exe").wstring() });
			Assert::AreEqual<DWORD>(0, exe.exitCode, wstring(exe.output.begin(), exe.output.end()).c_str());
			for (const auto* expected : { "PE signature found", "8664 machine (x64)", "PE32+", "Dynamic base", "KERNEL32.dll", "ExitProcess", "DIR64", "section alignment", ".text name" })
				Assert::IsTrue(exe.output.find(expected) != string::npos, (L"dumpbin output for the executable contains " + wstring(expected, expected + strlen(expected))).c_str());
			Assert::IsTrue(exe.output.find("warning") == string::npos && exe.output.find("invalid") == string::npos, L"dumpbin reports no PE warnings");

			const auto dll = runExecutable(workspace, dumpbin, { L"/NOLOGO", L"/HEADERS", L"/EXPORTS", workspace.file(L"tool.dll").wstring() });
			Assert::AreEqual<DWORD>(0, dll.exitCode, L"dumpbin accepts the DLL");
			for (const auto* expected : { "DLL", "api", "tool.dll" })
				Assert::IsTrue(dll.output.find(expected) != string::npos, (L"dumpbin output for the DLL contains " + wstring(expected, expected + strlen(expected))).c_str());
		}

	};
}
