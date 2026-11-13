#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(I386Tests)
	{
	public:
		TEST_METHOD(ExplicitAmd64RejectsI386Object)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } }, 0x14c));
			assertFailureContains(runLinker(workspace, { L"/machine:x64", L"/entry:main", L"/out:linked.exe", L"input.obj" }), "expected AMD64 COFF object");
			assertNoOutput(workspace);
		}

		TEST_METHOD(I386TargetAndStartupSelection)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb8, 11, 0, 0, 0, 0xc3, 0xb8, 22, 0, 0, 0, 0xc3, 0xb8, 33, 0, 0, 0, 0xc3 } };
			workspace.write(L"entry.obj", makeObject({ text }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "_mainCRTStartup", 6, 1, 2,{} }, TestSymbol{ "_WinMainCRTStartup", 12, 1, 2,{} } }, 0x14c));
			struct Case { vector<wstring> args; const wchar_t* name; DWORD result; uint16_t subsystem; };
			const vector<Case> cases{
				{ { L"/out:console.exe", L"entry.obj" }, L"console.exe", 22, 3 },
				{ { L"/subsystem:windows", L"/out:gui.exe", L"entry.obj" }, L"gui.exe", 33, 2 },
				{ { L"/entry:main", L"/out:plain.exe", L"entry.obj" }, L"plain.exe", 11, 3 },
				{ { L"/entry:_main", L"/out:decorated.exe", L"entry.obj" }, L"decorated.exe", 11, 3 },
				{ { L"system", L"nt_win", L"option", L"start=_main", L"file", L"entry.obj", L"name", L"jw.exe" }, L"jw.exe", 11, 2 },
				{ { L"format", L"windows", L"pe", L"runtime", L"console", L"option", L"start=_mainCRTStartup", L"file", L"entry.obj", L"name", L"format.exe" }, L"format.exe", 22, 3 }
			};
			for (const auto& test : cases)
			{
				assertSuccess(runLinker(workspace, test.args));
				const auto image = workspace.read(test.name);
				const auto pe = peHeader(image);
				Assert::AreEqual<uint16_t>(0x14c, get16(image, pe + 4));
				Assert::AreEqual<uint16_t>(0x10b, get16(image, pe + 24));
				Assert::AreEqual<uint32_t>(0x400000, get32(image, pe + 24 + 28));
				Assert::AreEqual(test.subsystem, get16(image, pe + 24 + 68));
				Assert::AreEqual(test.result, runExecutable(workspace, workspace.file(test.name), {}).exitCode);
			}
			workspace.write(L"start.lib", makeArchive({ { "start.obj", makeObject({ TestSection{ ".text",{ 0xb8, 44, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "_WinMainCRTStartup", 0, 1, 2,{} } }, 0x14c) } }));
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/out:archive.exe", L"start.lib" }));
			Assert::AreEqual<DWORD>(44, runExecutable(workspace, workspace.file(L"archive.exe"), {}).exitCode);
			assertSuccess(runLinker(workspace, { L"/entry:WinMainCRTStartup", L"/out:force-archive.exe", L"start.lib" }));
			Assert::AreEqual<DWORD>(44, runExecutable(workspace, workspace.file(L"force-archive.exe"), {}).exitCode);
			workspace.write(L"stdcall.obj", makeObject({ TestSection{ ".text",{ 0xb8, 55, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "_start@0", 0, 1, 2,{} } }, 0x14c));
			assertSuccess(runLinker(workspace, { L"/entry:_start@0", L"/out:stdcall.exe", L"stdcall.obj" }));
			Assert::AreEqual<DWORD>(55, runExecutable(workspace, workspace.file(L"stdcall.exe"), {}).exitCode);
			const auto fallback = runLinker(workspace, { L"format", L"windows", L"pe", L"file", L"entry.obj", L"name", L"fallback32.exe" });
			assertSuccess(fallback);
			Assert::IsTrue(fallback.output.find("no start address specified") != string::npos);
			Assert::AreEqual<DWORD>(11, runExecutable(workspace, workspace.file(L"fallback32.exe"), {}).exitCode);
			workspace.write(L"direct.obj", makeObject({ text, makeDirectiveSection("/ENTRY:_WinMainCRTStartup") }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "_WinMainCRTStartup", 12, 1, 2,{} } }, 0x14c));
			assertSuccess(runLinker(workspace, { L"/out:direct.exe", L"direct.obj" }));
			Assert::AreEqual<DWORD>(33, runExecutable(workspace, workspace.file(L"direct.exe"), {}).exitCode);
			assertSuccess(runLinker(workspace, { L"/entry:_main", L"/out:override.exe", L"direct.obj" }));
			Assert::AreEqual<DWORD>(11, runExecutable(workspace, workspace.file(L"override.exe"), {}).exitCode);
			assertFailureContains(runLinker(workspace, { L"/entry:_missing", L"entry.obj" }), "_missing");
			assertFailureContains(runLinker(workspace, { L"/entry:", L"entry.obj" }), "/ENTRY requires a symbol name");
			workspace.write(L"x64.obj", makeEntryObject());
			assertFailureContains(runLinker(workspace, { L"/entry:_main", L"entry.obj", L"x64.obj" }), "expected I386");
			workspace.write(L"mixed.lib", makeArchive({ { "x86.obj", makeObject({ text }, { TestSymbol{ "_main", 0, 1, 2,{} } }, 0x14c) }, { "x64.obj", makeEntryObject() } }));
			assertFailureContains(runLinker(workspace, { L"/entry:_main", L"mixed.lib" }), "expected I386");
			assertFailureContains(runLinker(workspace, { L"system", L"nt", L"option", L"start=main", L"file", L"entry.obj" }), "'main'");
		}

		TEST_METHOD(I386StaticArchiveProducesRunnablePe32)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 1, 1, 0x14 } };
			workspace.write(L"entry.obj", makeObject({ text }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "_helper", 0, 0, 2,{} } }, 0x14c));
			TestSection helper{ ".text",{ 0xa1, 0, 0, 0, 0, 0xc3 } };
			helper.relocations = { { 1, 1, 6 } };
			workspace.write(L"helper.lib", makeArchive({ { "helper.obj", makeObject({ helper, TestSection{ ".data",{ 42, 0, 0, 0 }, 0xc0000040 } }, { TestSymbol{ "_helper", 0, 1, 2,{} }, TestSymbol{ "_value", 0, 2, 2,{} } }, 0x14c) } }));
			assertSuccess(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/out:x86.exe", L"entry.obj", L"helper.lib" }));
			const auto image = workspace.read(L"x86.exe");
			const auto pe = peHeader(image);
			const auto optional = pe + 24;
			Assert::AreEqual<uint16_t>(0x14c, get16(image, pe + 4));
			Assert::AreEqual<uint16_t>(224, get16(image, pe + 20));
			Assert::AreEqual<uint16_t>(0x10b, get16(image, optional));
			Assert::AreEqual<uint32_t>(0x400000, get32(image, optional + 28));
			Assert::AreEqual<uint16_t>(3, get16(image, optional + 68));
			Assert::IsTrue((get16(image, pe + 22) & 0x0100) != 0, L"32-bit machine characteristic");
			const auto relocRva = get32(image, optional + 96 + 5 * 8);
			Assert::IsTrue(relocRva != 0 && get32(image, optional + 96 + 5 * 8 + 4) >= 12, L"DIR32 generates a HIGHLOW relocation directory");
			const auto table = optional + 224;
			bool highLow = false;
			for (size_t i = 0; i < get16(image, pe + 6); ++i)
			{
				const auto h = table + 40 * i;
				if (get32(image, h + 12) == relocRva)
				{
					const auto raw = get32(image, h + 20);
					highLow = (get16(image, raw + 8) >> 12) == 3;
				}
			}
			Assert::IsTrue(highLow, L"base relocation has HIGHLOW type");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"x86.exe"), {}).exitCode, L"x86 relative and absolute relocations execute");
			assertFailureContains(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/base:0x100000000", L"entry.obj", L"helper.lib" }), "target address range");
			assertFailureContains(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/debug", L"entry.obj" }), "x86 debug");
			assertFailureContains(runLinker(workspace, { L"/machine:x64", L"/entry:_main", L"helper.lib" }), "expected AMD64");
		}

		TEST_METHOD(I386RelocationsAndUnsupportedInputsAreChecked)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(16, 0) };
			text.data[0] = 0xc3;
			text.relocations = { { 1, 1, 7 }, { 5, 1, 0x0a }, { 7, 1, 0x0b } };
			workspace.write(L"input.obj", makeObject({ text, TestSection{ ".data",{ 42, 0, 0, 0 }, 0xc0000040 } }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "_data", 0, 2, 2,{} } }, 0x14c));
			assertSuccess(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/out:x86.exe", L"input.obj" }));
			const auto image = workspace.read(L"x86.exe");
			const auto pe = peHeader(image);
			const auto table = pe + 24 + 224;
			const auto textRaw = get32(image, table + 20);
			const auto dataRva = get32(image, table + 40 + 12);
			Assert::AreEqual(dataRva, get32(image, textRaw + 1), L"DIR32NB uses the target RVA");
			Assert::AreEqual<uint16_t>(2, get16(image, textRaw + 5), L"SECTION uses the output section index");
			Assert::AreEqual<uint32_t>(0, get32(image, textRaw + 7), L"SECREL uses the output section offset");
			text.relocations = { { 1, 1, 1 } };
			workspace.write(L"unsupported.obj", makeObject({ text, TestSection{ ".data",{ 42 }, 0xc0000040 } }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "_data", 0, 2, 2,{} } }, 0x14c));
			assertFailureContains(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"unsupported.obj" }), "unsupported I386 relocation");
			workspace.write(L"x64.obj", makeEntryObject());
			assertFailureContains(runLinker(workspace, { L"/machine:x86", L"/entry:main", L"x64.obj" }), "expected I386 COFF object");
		}

		TEST_METHOD(I386EmbeddedManifestIsAnActivatableResource)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xb8, 0, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "_main", 0, 1, 2,{} } }, 0x14c));
			const string manifest = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\"><assemblyIdentity type=\"win32\" name=\"JWlink2.Test.Activation\" version=\"1.0.0.0\" processorArchitecture=\"x86\"/></assembly>";
			workspace.write(L"app.manifest", Bytes(manifest.begin(), manifest.end()));
			for (const auto& [name, id, option] : vector<tuple<wstring, WORD, wstring>>{ { L"default.exe", 1, L"/manifest:embed" }, { L"custom.exe", 17, L"/manifest:embed,id=17" } })
			{
				assertSuccess(runLinker(workspace, { option, L"/manifestinput:app.manifest", L"/out:" + name, L"entry.obj" }));
				const auto image = workspace.read(name);
				const auto pe = peHeader(image);
				Assert::AreEqual<uint16_t>(0x10b, get16(image, pe + 24));
				Assert::IsTrue(get32(image, pe + 24 + 96 + 16) != 0, L"PE32 resource directory exists");
				const auto module = LoadLibraryExW(workspace.file(name).c_str(), nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
				Assert::IsNotNull(module);
				const auto info = FindResourceExW(module, MAKEINTRESOURCEW(24), MAKEINTRESOURCEW(id), 0x409);
				Assert::IsNotNull(info, L"manifest ID and language");
				const auto data = static_cast<const char*>(LockResource(LoadResource(module, info)));
				Assert::IsNotNull(data);
				Assert::AreEqual(manifest, string(data, SizeofResource(module, info)));
				Assert::IsTrue(FreeLibrary(module) != 0);
				ACTCTXW context{};
				context.cbSize = sizeof(context);
				const auto path = workspace.file(name).wstring();
				context.lpSource = path.c_str();
				context.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
				context.lpResourceName = MAKEINTRESOURCEW(id);
				const auto handle = CreateActCtxW(&context);
				Assert::IsTrue(handle != INVALID_HANDLE_VALUE, L"Windows creates an activation context from the PE32 manifest");
				ULONG_PTR cookie = 0;
				Assert::IsTrue(ActivateActCtx(handle, &cookie) != 0);
				Assert::IsTrue(DeactivateActCtx(0, cookie) != 0);
				ReleaseActCtx(handle);
				Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(name), {}).exitCode);
			}
		}

		TEST_METHOD(I386ResourceObjectFromWindowsTools)
		{
			wchar_t programFiles[MAX_PATH]{};
			wchar_t programFilesX86[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			GetEnvironmentVariableW(L"ProgramFiles(x86)", programFilesX86, MAX_PATH);
			error_code ignored;
			fs::path resourceCompiler, cvtres;
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFilesX86) / L"Windows Kits" / L"10" / L"bin", fs::directory_options::skip_permission_denied, ignored))
				if (entry.path().filename() == L"rc.exe" && entry.path().parent_path().filename() == L"x64" && (resourceCompiler.empty() || entry.path().wstring() > resourceCompiler.wstring())) resourceCompiler = entry.path();
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
				if (entry.path().filename() == L"cvtres.exe" && entry.path().parent_path().filename() == L"x86" && entry.path().parent_path().parent_path().filename() == L"Hostx64") cvtres = entry.path();
			if (resourceCompiler.empty() || cvtres.empty()) { Logger::WriteMessage("Windows SDK resource compiler or MSVC cvtres unavailable; x86 resource object test skipped"); return; }
			Workspace workspace;
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xb8, 0, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "_main", 0, 1, 2,{} } }, 0x14c));
			workspace.writeText(L"resources.rc", L"LANGUAGE 9, 1\n101 RCDATA { 0x1234, 0x5678 }\n102 RCDATA { 0x789a }\nLANGUAGE 12, 1\n101 RCDATA { 0x4321 }\n");
			assertSuccess(runExecutable(workspace, resourceCompiler, { L"/nologo", L"/foresources.res", L"resources.rc" }));
			assertSuccess(runExecutable(workspace, cvtres, { L"/nologo", L"/machine:x86", L"/out:resources.obj", L"resources.res" }));
			assertSuccess(runLinker(workspace, { L"/out:resource.exe", L"entry.obj", L"resources.obj" }));
			const auto module = LoadLibraryExW(workspace.file(L"resource.exe").c_str(), nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
			Assert::IsNotNull(module);
			for (const auto& [language, expected] : vector<pair<WORD, Bytes>>{ { 0x409, { 0x34, 0x12, 0x78, 0x56 } }, { 0x40c, { 0x21, 0x43 } } })
			{
				const auto info = FindResourceExW(module, RT_RCDATA, MAKEINTRESOURCEW(101), language);
				Assert::IsNotNull(info, L"Windows resolves the language-specific resource");
				const auto data = static_cast<const uint8_t*>(LockResource(LoadResource(module, info)));
				Assert::IsNotNull(data);
				Assert::IsTrue(Bytes(data, data + SizeofResource(module, info)) == expected);
			}
			const auto other = FindResourceExW(module, RT_RCDATA, MAKEINTRESOURCEW(102), 0x409);
			Assert::IsNotNull(other, L"Windows resolves a second resource ID");
			const auto otherData = static_cast<const uint8_t*>(LockResource(LoadResource(module, other)));
			Assert::IsNotNull(otherData);
			Assert::IsTrue(Bytes(otherData, otherData + SizeofResource(module, other)) == Bytes({ 0x9a, 0x78 }));
			Assert::IsTrue(FreeLibrary(module) != 0);
			Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(L"resource.exe"), {}).exitCode);
			assertFailureContains(runLinker(workspace, { L"entry.obj", L"resources.res" }), ".res resource input is not yet supported");
			assertFailureContains(runLinker(workspace, { L"/manifest:embed", L"entry.obj", L"resources.obj" }), "already contains a .rsrc");
			assertFailureContains(runLinker(workspace, { L"entry.obj", L"resources.obj", L"resources.obj" }), "merging x86 resource sections");
		}

		TEST_METHOD(RealMsvcI386ObjectAndArchiveExecute)
		{
			fs::path tools;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"cl.exe" && entry.path().parent_path().filename() == L"x86" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"lib.exe") && fs::exists(entry.path().parent_path() / L"link.exe") && fs::exists(entry.path().parent_path() / L"dumpbin.exe")) tools = entry.path().parent_path();
			}
			if (tools.empty()) { Logger::WriteMessage("MSVC x86 tools unavailable; real x86 comparison skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"main.c", L"int helper(void); int main(void) { return helper(); }\n");
			workspace.writeText(L"helper.c", L"int value = 42; int helper(void) { return value; }\n");
			for (const auto* file : { L"main.c", L"helper.c" }) assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", file }));
			assertSuccess(runExecutable(workspace, tools / L"lib.exe", { L"/nologo", L"/out:helper.lib", L"helper.obj" }));
			assertSuccess(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/out:jw2.exe", L"main.obj", L"helper.lib" }));
			assertSuccess(runExecutable(workspace, tools / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/machine:x86", L"/safeseh:no", L"/entry:main", L"/subsystem:console", L"/out:msvc.exe", L"main.obj", L"helper.lib" }));
			for (const auto* name : { L"jw2.exe", L"msvc.exe" })
			{
				const auto inspection = runExecutable(workspace, tools / L"dumpbin.exe", { L"/nologo", L"/headers", L"/relocations", name });
				assertSuccess(inspection);
				for (const auto* expected : { "14C machine (x86)", "PE32", "HIGHLOW" }) Assert::IsTrue(inspection.output.find(expected) != string::npos, wstring(expected, expected + strlen(expected)).c_str());
				Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(name), {}).exitCode, L"Windows executes linked x86 archive member");
			}
			wchar_t programFilesX86[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles(x86)", programFilesX86, MAX_PATH);
			fs::path kernel;
			for (const auto& entry : fs::directory_iterator(fs::path(programFilesX86) / L"Windows Kits" / L"10" / L"Lib", ignored))
			{
				const auto candidate = entry.path() / L"um" / L"x86" / L"kernel32.lib";
				if (fs::exists(candidate) && (kernel.empty() || candidate.wstring() > kernel.wstring())) kernel = candidate;
			}
			if (kernel.empty()) { Logger::WriteMessage("Windows SDK x86 kernel32.lib unavailable; import test skipped"); return; }
			workspace.writeText(L"activation.c", L"__declspec(dllimport) int __stdcall FindActCtxSectionStringW(unsigned, const void*, unsigned, const unsigned short*, void*);\n"
				L"__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned);\n"
				L"void main(void) { unsigned long data[64]; data[0] = sizeof(data); ExitProcess(FindActCtxSectionStringW(0,0,3,L\"JWlink2.Test.WindowClass\",data) ? 0 : 46); }\n");
			workspace.writeText(L"activation.manifest", L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\"><assemblyIdentity type=\"win32\" name=\"JWlink2.Test.Activation\" version=\"1.0.0.0\" processorArchitecture=\"x86\"/><file name=\"jwlink2-test.dll\"><windowClass versioned=\"no\">JWlink2.Test.WindowClass</windowClass></file></assembly>");
			assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"activation.c" }));
			assertSuccess(runLinker(workspace, { L"/manifest:embed", L"/manifestinput:activation.manifest", L"/out:activation.exe", L"activation.obj", kernel.wstring() }));
			Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(L"activation.exe"), {}).exitCode, L"Windows automatically activates the embedded process manifest");
			workspace.writeText(L"import.c", L"__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned int); void main(void) { ExitProcess(43); }\n");
			assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"import.c" }));
			assertSuccess(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/out:import.exe", L"import.obj", kernel.wstring() }));
			const auto inspection = runExecutable(workspace, tools / L"dumpbin.exe", { L"/nologo", L"/headers", L"/imports", L"import.exe" });
			assertSuccess(inspection);
			Assert::IsTrue(inspection.output.find("KERNEL32.dll") != string::npos && inspection.output.find("ExitProcess") != string::npos, L"dumpbin reports x86 import");
			Assert::AreEqual<DWORD>(43, runExecutable(workspace, workspace.file(L"import.exe"), {}).exitCode, L"x86 SDK import executes");
			workspace.writeText(L"thunk.c", L"__declspec(noreturn) void __stdcall ExitProcess(unsigned int); void main(void) { ExitProcess(44); }\n");
			assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"thunk.c" }));
			assertSuccess(runLinker(workspace, { L"/machine:x86", L"/entry:_main", L"/out:thunk.exe", L"thunk.obj", kernel.wstring() }));
			const auto thunkInspection = runExecutable(workspace, tools / L"dumpbin.exe", { L"/nologo", L"/imports", L"/relocations", L"thunk.exe" });
			assertSuccess(thunkInspection);
			Assert::IsTrue(thunkInspection.output.find("ExitProcess") != string::npos && thunkInspection.output.find("HIGHLOW") != string::npos, L"x86 import thunk has a base relocation");
			Assert::AreEqual<DWORD>(44, runExecutable(workspace, workspace.file(L"thunk.exe"), {}).exitCode, L"x86 function thunk executes");
			workspace.writeText(L"start.c", L"__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned); void mainCRTStartup(void) { ExitProcess(45); }\n");
			assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"start.c" }));
			const auto original = runExecutable(workspace, jwlinkPath(), { L"format", L"windows", L"pe", L"runtime", L"console", L"option", L"quiet,start=_main", L"file", L"main.obj", L"library", L"helper.lib", L"name", L"original32.exe" });
			if (original.exitCode == 0) Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"original32.exe"), {}).exitCode);
			else
			{
				Assert::AreEqual<DWORD>(0xc0000005, original.exitCode, L"unexpected JWlink x86 comparison failure");
				Logger::WriteMessage("JWlink x86 comparison unavailable: bundled JWlink crashed with access violation");
			}
			assertSuccess(runExecutable(workspace, tools / L"lib.exe", { L"/nologo", L"/out:start.lib", L"start.obj" }));
			assertSuccess(runLinker(workspace, { L"/out:inferred.exe", L"start.lib", kernel.wstring() }));
			assertSuccess(runExecutable(workspace, tools / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/machine:x86", L"/safeseh:no", L"/subsystem:console", L"/out:msvc-start.exe", L"start.lib", kernel.wstring() }));
			for (const auto* name : { L"inferred.exe", L"msvc-start.exe" }) Assert::AreEqual<DWORD>(45, runExecutable(workspace, workspace.file(name), {}).exitCode);
			const auto user32 = kernel.parent_path() / L"user32.lib";
			if (!fs::exists(user32)) { Logger::WriteMessage("Windows SDK x86 user32.lib unavailable; GUI test skipped"); return; }
			workspace.writeText(L"gui32.c", L"typedef void *HANDLE; typedef unsigned short WCHAR;\n"
				L"__declspec(dllimport) HANDLE __stdcall CreateWindowExW(unsigned long,const WCHAR*,const WCHAR*,unsigned long,int,int,int,int,HANDLE,HANDLE,HANDLE,void*);\n"
				L"__declspec(dllimport) int __stdcall DestroyWindow(HANDLE);\n"
				L"__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned);\n"
				L"void WinMainCRTStartup(void) { HANDLE w=CreateWindowExW(0,L\"STATIC\",L\"x86\",0,0,0,16,16,0,0,0,0); if (!w) ExitProcess(41); if (!DestroyWindow(w)) ExitProcess(42); ExitProcess(0); }\n");
			assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"gui32.c" }));
			assertSuccess(runLinker(workspace, { L"/subsystem:windows", L"/out:gui32.exe", L"gui32.obj", user32.wstring(), kernel.wstring() }));
			assertSuccess(runLinker(workspace, { L"system", L"nt_win", L"option", L"start=_WinMainCRTStartup", L"file", L"gui32.obj", L"library", user32.wstring(), kernel.wstring(), L"name", L"jwgui32.exe" }));
			assertSuccess(runExecutable(workspace, tools / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/safeseh:no", L"/subsystem:windows", L"/out:msvc-gui32.exe", L"gui32.obj", user32.wstring(), kernel.wstring() }));
			for (const auto* name : { L"gui32.exe", L"jwgui32.exe", L"msvc-gui32.exe" })
			{
				const auto image = workspace.read(name);
				Assert::AreEqual<uint16_t>(2, get16(image, peHeader(image) + 24 + 68));
				Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(name), {}).exitCode);
			}
		}

	};
}
