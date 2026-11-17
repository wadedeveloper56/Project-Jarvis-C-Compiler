#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(I386TlsTests)
	{
	public:
		TEST_METHOD(I386TlsDirectoryRequiresCompleteMetadata)
		{
			Workspace workspace;
			workspace.write(L"missing.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, TestSection{ ".tls", { 1, 2, 3, 4 }, 0xc0000040 } }, { TestSymbol{ "_main", 0, 1, 2,{} } }, 0x14c));
			assertSuccess(runLinker(workspace, { L"/entry:_main", L"/out:missing.exe", L"missing.obj" }));
			const auto missing = workspace.read(L"missing.exe");
			Assert::AreEqual<uint32_t>(0, get32(missing, peHeader(missing) + 24 + 96 + 72));
			workspace.write(L"short.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, TestSection{ ".rdata$A", Bytes(23, 0), 0x40000040 }, TestSection{ ".rdata$B", Bytes(24, 0), 0x40000040 } }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "__tls_used", 0, 2, 2,{} } }, 0x14c));
			assertFailureContains(runLinker(workspace, { L"/entry:_main", L"/out:short.exe", L"short.obj" }), "TLS directory structure is truncated");
			assertNoOutput(workspace, L"short.exe");
		}

		TEST_METHOD(I386TlsDirectoryAddressesAndBaseRelocations)
		{
			Workspace workspace;
			TestSection directory{ ".rdata", Bytes(24, 0), 0x40000040 };
			directory.relocations = { { 0, 2, 6 }, { 4, 3, 6 }, { 8, 4, 6 }, { 12, 5, 6 } };
			for (const auto* symbol : { "__tls_used", "_tls_used" })
			{
				workspace.write(L"tls.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, directory, TestSection{ ".tls$A", { 1, 2, 3, 4 }, 0xc0000040 }, TestSection{ ".data", Bytes(8, 0), 0xc0000040 } },
					{ TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ symbol, 0, 2, 2,{} }, TestSymbol{ "start", 0, 3, 2,{} }, TestSymbol{ "end", 4, 3, 2,{} }, TestSymbol{ "index", 0, 4, 2,{} }, TestSymbol{ "callbacks", 4, 4, 2,{} } }, 0x14c));
				for (const bool fixed : { false, true })
				{
					vector<wstring> args{ L"/entry:_main", L"/out:tls.exe", L"tls.obj" };
					if (fixed) args.insert(args.begin(), L"/fixed");
					assertSuccess(runLinker(workspace, args));
					const auto image = workspace.read(L"tls.exe");
					const auto pe = peHeader(image);
					const auto optional = pe + 24;
					const auto table = optional + get16(image, pe + 20);
					auto section = [&](const string& name)
						{
							for (size_t i = 0; i < get16(image, pe + 6); ++i)
							{
								const auto header = table + 40 * i;
								if (string(reinterpret_cast<const char*>(image.data() + header), strnlen_s(reinterpret_cast<const char*>(image.data() + header), 8)) == name) return header;
							}
							Assert::Fail(L"section not found");
							return size_t{ 0 };
						};
					const auto rdata = section(".rdata"), tls = section(".tls"), data = section(".data");
					const auto rva = get32(image, optional + 96 + 72);
					Assert::AreEqual(get32(image, rdata + 12), rva);
					Assert::AreEqual<uint32_t>(24, get32(image, optional + 96 + 76));
					const auto raw = get32(image, rdata + 20), base = get32(image, optional + 28);
					Assert::AreEqual(base + get32(image, tls + 12), get32(image, raw));
					Assert::AreEqual(base + get32(image, tls + 12) + 4, get32(image, raw + 4));
					Assert::AreEqual(base + get32(image, data + 12), get32(image, raw + 8));
					Assert::AreEqual(base + get32(image, data + 12) + 4, get32(image, raw + 12));
					const auto relocRva = get32(image, optional + 96 + 40);
					if (fixed) Assert::AreEqual<uint32_t>(0, relocRva);
					else
					{
						const auto reloc = section(".reloc");
						const auto relocRaw = get32(image, reloc + 20);
						Assert::AreEqual(get32(image, reloc + 12), relocRva);
						const auto size = get32(image, optional + 96 + 44);
						array<bool, 4> found{};
						for (size_t block = 0; block < size; )
						{
							const auto page = get32(image, relocRaw + block), length = get32(image, relocRaw + block + 4);
							Assert::IsTrue(length >= 8 && block + length <= size);
							for (size_t entry = 8; entry < length; entry += 2)
							{
								const auto value = get16(image, relocRaw + block + entry);
								for (size_t i = 0; i < found.size(); ++i) if ((value >> 12) == 3 && page + (value & 0xfff) == rva + i * 4) found[i] = true;
							}
							block += length;
						}
						for (const auto present : found) Assert::IsTrue(present, L"TLS address has a HIGHLOW relocation");
					}
				}
			}
		}

		TEST_METHOD(I386UninitializedCoffSectionIsZeroFilled)
		{
			Workspace workspace;
			TestSection text{ ".text", { 0xa1, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 1, 1, 6 } };
			auto object = makeObject({ text, TestSection{ ".bss", Bytes(4, 0), 0xc0000080 } }, { TestSymbol{ "_main", 0, 1, 2,{} }, TestSymbol{ "_zero", 0, 2, 2,{} } }, 0x14c);
			put32(object, 20 + 40 + 20, 0);
			workspace.write(L"bss.obj", object);
			assertSuccess(runLinker(workspace, { L"/entry:_main", L"/out:bss.exe", L"bss.obj" }));
			Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(L"bss.exe"), {}).exitCode);
			put32(object, 20 + 40 + 36, 0xc0000040);
			workspace.write(L"bad.obj", object);
			assertFailureContains(runLinker(workspace, { L"/entry:_main", L"/out:bad.exe", L"bad.obj" }), "initialized section has no raw data offset");
			assertNoOutput(workspace, L"bad.exe");
		}

		TEST_METHOD(RealI386TlsRunsCallbacksAndIsolatesThreads)
		{
			fs::path tools;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"cl.exe" && entry.path().parent_path().filename() == L"x86" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe")) tools = entry.path().parent_path();
			}
			wchar_t programFilesX86[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles(x86)", programFilesX86, MAX_PATH);
			fs::path kernel;
			for (const auto& entry : fs::directory_iterator(fs::path(programFilesX86) / L"Windows Kits" / L"10" / L"Lib", ignored))
			{
				const auto candidate = entry.path() / L"um" / L"x86" / L"kernel32.lib";
				if (fs::exists(candidate) && (kernel.empty() || candidate.wstring() > kernel.wstring())) kernel = candidate;
			}
			if (tools.empty() || kernel.empty()) { Logger::WriteMessage("MSVC x86 tools or SDK unavailable; TLS execution test skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"tls.c", L"typedef unsigned long DWORD; typedef void *HANDLE; typedef void (__stdcall *Callback)(void*,DWORD,void*);\n"
				L"__declspec(dllimport) HANDLE __stdcall CreateThread(void*,unsigned,DWORD (__stdcall *)(void*),void*,DWORD,DWORD*);\n"
				L"__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE,DWORD);\n"
				L"__declspec(dllimport) int __stdcall GetExitCodeThread(HANDLE,DWORD*);\n"
				L"__declspec(dllimport) int __stdcall CloseHandle(HANDLE);\n"
				L"__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned);\n"
				L"__declspec(thread) int value=7; __declspec(thread) int zero; volatile int processOrder=0, threadOrder=0, detachOrder=0;\n"
				L"unsigned long _tls_index=0;\n"
				L"#pragma section(\".tls\",read,write)\n#pragma section(\".tls$ZZZ\",read,write)\n"
				L"__declspec(allocate(\".tls\")) char _tls_start=0; __declspec(allocate(\".tls$ZZZ\")) char _tls_end=0;\n"
				L"void __stdcall first(void* m,DWORD reason,void* r) { if(reason==1) processOrder=processOrder*10+1; if(reason==2) threadOrder=threadOrder*10+1; if(reason==3) detachOrder=detachOrder*10+1; }\n"
				L"void __stdcall second(void* m,DWORD reason,void* r) { if(reason==1) processOrder=processOrder*10+2; if(reason==2) threadOrder=threadOrder*10+2; if(reason==3) detachOrder=detachOrder*10+2; }\n"
				L"#pragma section(\".CRT$XLA\",read)\n#pragma section(\".CRT$XLB\",read)\n#pragma section(\".CRT$XLC\",read)\n#pragma section(\".CRT$XLZ\",read)\n"
				L"__declspec(allocate(\".CRT$XLC\")) Callback late=second; __declspec(allocate(\".CRT$XLB\")) Callback early=first; __declspec(allocate(\".CRT$XLA\")) Callback cbStart=0; __declspec(allocate(\".CRT$XLZ\")) Callback cbEnd=0;\n"
				L"struct TlsDirectory { const void* start; const void* end; unsigned long* index; Callback* callbacks; DWORD zeroFill; DWORD characteristics; };\n"
				L"const struct TlsDirectory _tls_used = { &_tls_start, &_tls_end, &_tls_index, &cbStart+1, 0, 0 };\n"
				L"DWORD __stdcall worker(void* p) { if(value!=7 || zero!=0 || threadOrder!=12) return 41; value=99; zero=17; return value; }\n"
				L"void main(void) { DWORD code=0; HANDLE h; if(processOrder!=12) ExitProcess(100+processOrder); if(value!=7 || zero!=0) ExitProcess(47); value=21; zero=9; h=CreateThread(0,0,worker,0,0,0); if(!h) ExitProcess(43); if(WaitForSingleObject(h,0xffffffff)!=0) ExitProcess(44); if(!GetExitCodeThread(h,&code)) ExitProcess(45); CloseHandle(h); if(code!=99 || value!=21 || zero!=9 || detachOrder!=12) ExitProcess(46); ExitProcess(0); }\n");
			assertSuccess(runExecutable(workspace, tools / L"cl.exe", { L"/nologo", L"/c", L"/Od", L"/GS-", L"/Zl", L"tls.c" }));
			const auto runtime = tools.parent_path().parent_path().parent_path() / L"lib" / L"x86" / L"libcmt.lib";
			assertSuccess(runExecutable(workspace, tools / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/machine:x86", L"/safeseh:no", L"/entry:main", L"/subsystem:console", L"/out:msvc-tls.exe", L"tls.obj", runtime.wstring(), kernel.wstring() }));
			assertSuccess(runLinker(workspace, { L"/entry:_main", L"/out:jw-tls.exe", L"tls.obj", runtime.wstring(), kernel.wstring() }));
			for (const auto* name : { L"jw-tls.exe", L"msvc-tls.exe" })
			{
				const auto image = workspace.read(name);
				const auto pe = peHeader(image);
				Assert::AreEqual<uint16_t>(0x10b, get16(image, pe + 24));
				Assert::IsTrue(get32(image, pe + 24 + 96 + 72) != 0, L"TLS directory is present");
				Assert::AreEqual<uint32_t>(24, get32(image, pe + 24 + 96 + 76));
				Assert::AreEqual<DWORD>(0, runExecutable(workspace, workspace.file(name), {}).exitCode, L"TLS initialization, callback order and per-thread isolation");
			}
		}
	};
}
