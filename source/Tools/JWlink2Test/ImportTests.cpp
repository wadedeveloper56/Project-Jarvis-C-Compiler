#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(ImportTests)
	{
	public:
		TEST_METHOD(OrdinaryImportsRemainCallableAtRuntime)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto runtime = runExecutable(workspace, workspace.file(L"linked.exe"), {});
			Assert::AreEqual<DWORD>(42, runtime.exitCode, L"ordinary imports continue to call through their IAT thunks");
		}

		TEST_METHOD(DelayLoadEmitsMetadataAndResolvesFunctionsAtRuntime)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text, makeDirectiveSection("-delayload:KERNEL32.dll") }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto args = outputArgs(workspace);
			args.push_back(L"/DELAYLOAD:KERNEL32.dll");
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			const auto linkResult = runLinker(workspace, args);
			if (linkResult.exitCode != 0)
			{
				wstring message = L"linker exit code: " + to_wstring(linkResult.exitCode) + L"; output: ";
				message.append(linkResult.output.begin(), linkResult.output.end());
				Assert::Fail(message.c_str());
			}
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto directoryRva = get32(image, optional + 216);
			Assert::IsTrue(directoryRva != 0, L"delay-import data directory is present");
			Assert::AreEqual<uint32_t>(64, get32(image, optional + 220), L"one delay descriptor plus terminator");
			const auto descriptor = rvaOffset(image, directoryRva);
			Assert::AreEqual<uint32_t>(1, get32(image, descriptor), L"delay descriptor uses RVA-based fields");
			Assert::IsTrue(cStringAtRva(image, get32(image, descriptor + 4)) == "KERNEL32.dll", L"delay descriptor DLL name");
			Assert::AreEqual<uint32_t>(0, get32(image, descriptor + 8), L"the custom resolver does not use a module-handle slot");
			Assert::AreEqual<uint32_t>(0, get32(image, descriptor + 20), L"bound delay IAT is omitted");
			Assert::AreEqual<uint32_t>(0, get32(image, descriptor + 24), L"unload delay IAT is omitted");
			const auto delayIatRva = get32(image, descriptor + 12);
			const auto delayIntRva = get32(image, descriptor + 16);
			const auto delayIat = rvaOffset(image, delayIatRva);
			const auto delayInt = rvaOffset(image, delayIntRva);
			const auto imageBase = get64(image, optional + 24);
			const auto resolverRva = get64(image, delayIat) - imageBase;
			const auto textHeader = sectionHeader(image, ".text");
			Assert::IsTrue(resolverRva >= get32(image, textHeader + 12) && resolverRva < get32(image, textHeader + 12) + get32(image, textHeader + 8), L"initial delay IAT points at generated resolver code");
			Assert::IsTrue(get64(image, delayInt) != 0, L"delay lookup table contains the imported function");
			Assert::AreEqual<uint64_t>(0, get64(image, delayIat + 8), L"delay IAT is null terminated");
			Assert::AreEqual<uint64_t>(0, get64(image, delayInt + 8), L"delay lookup table is null terminated");
			const auto normalImportRva = get32(image, optional + 120);
			Assert::IsTrue(normalImportRva != 0, L"resolver APIs remain ordinary eager imports");
			const auto normalDescriptor = rvaOffset(image, normalImportRva);
			const auto normalLookup = rvaOffset(image, get32(image, normalDescriptor));
			const auto helperNameRva = static_cast<uint32_t>(get64(image, normalLookup));
			Assert::IsTrue(cStringAtRva(image, helperNameRva + 2) == "LoadLibraryA", L"resolver imports LoadLibraryA normally");
			const auto exceptionDirectoryRva = get32(image, optional + 136);
			Assert::IsTrue(exceptionDirectoryRva != 0 && get32(image, optional + 140) == 12, L"resolver has a RUNTIME_FUNCTION unwind record");
			const auto runtimeFunction = rvaOffset(image, exceptionDirectoryRva);
			Assert::AreEqual<uint32_t>(static_cast<uint32_t>(resolverRva), get32(image, runtimeFunction), L"unwind range begins at the resolver");
			Assert::IsTrue(get32(image, runtimeFunction + 4) > get32(image, runtimeFunction), L"unwind range covers resolver code");
			const auto unwindInfo = rvaOffset(image, get32(image, runtimeFunction + 8));
			Assert::AreEqual<uint8_t>(1, image[unwindInfo], L"resolver unwind data uses version 1");
			Assert::AreEqual<uint8_t>(7, image[unwindInfo + 1], L"resolver unwind data describes its stack-allocation prolog");
			const auto runtime = runExecutable(workspace, workspace.file(L"linked.exe"), {});
			Assert::AreEqual<DWORD>(42, runtime.exitCode, L"delay-loaded function resolves on first use");
		}

		TEST_METHOD(DelayLoadCachesResolvedFunctionAcrossCalls)
		{
			if (sizeof(void*) != 8) { Logger::WriteMessage("An x64 test host is required to load the generated DLL; delay cache test skipped"); return; }
			Workspace workspace;
			workspace.write(L"provider.obj", makeObject({ TestSection{ ".text",{ 0x8d, 0x04, 0x11, 0x44, 0x01, 0xc0, 0x44, 0x01, 0xc8, 0x03, 0x44, 0x24, 0x28, 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/dll", L"/noentry", L"/export:api", L"/out:delay_provider_unique.dll", L"provider.obj" }));
			const auto providerPath = workspace.file(L"delay_provider_unique.dll");
			workspace.write(L"delay.lib", makeArchive({ { "api.obj", makeImportObject("api", providerPath.string(), "api", 0, 4) } }));
			TestSection text{ ".text",{ 0xe9, 0, 0, 0, 0 } };
			text.relocations = { { 1, 1, 4 } };
			workspace.write(L"client.obj", makeObject({ text }, { TestSymbol{ "callApi", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/dll", L"/noentry", L"/export:callApi", L"/out:delay_client.dll", L"/delayload:" + providerPath.wstring(), L"client.obj", L"delay.lib" }));
			const auto image = workspace.read(L"delay_client.dll");
			const auto descriptor = rvaOffset(image, get32(image, optionalHeader(image) + 216));
			const auto iatRva = get32(image, descriptor + 12);
			Assert::IsNull(GetModuleHandleW(L"delay_provider_unique.dll"), L"provider is not already loaded");
			const auto client = LoadLibraryW(workspace.file(L"delay_client.dll").c_str());
			Assert::IsNotNull(client, L"Windows loads the client without loading the delayed DLL");
			Assert::IsNull(GetModuleHandleW(L"delay_provider_unique.dll"), L"the import remains lazy");
			using Api = int(*)(int, int, int, int, int);
			const auto call = reinterpret_cast<Api>(GetProcAddress(client, "callApi"));
			Assert::IsNotNull(call);
			const auto iat = reinterpret_cast<const volatile uintptr_t*>(reinterpret_cast<const uint8_t*>(client) + iatRva);
			const auto initial = *iat;
			Assert::AreEqual(15, call(1, 2, 3, 4, 5), L"first call preserves register and stack arguments");
			const auto provider = GetModuleHandleW(L"delay_provider_unique.dll");
			Assert::IsNotNull(provider, L"first call loads the provider");
			const auto resolved = reinterpret_cast<uintptr_t>(GetProcAddress(provider, "api"));
			Assert::IsTrue(resolved != 0 && resolved != initial && *iat == resolved, L"first call caches the actual function address");
			Assert::AreEqual(40, call(6, 7, 8, 9, 10), L"cached call preserves arguments");
			Assert::IsTrue(*iat == resolved, L"subsequent calls retain the cached address");
			Assert::IsTrue(FreeLibrary(client) != 0);
			Assert::IsTrue(FreeLibrary(provider) != 0);
		}

		TEST_METHOD(DelayLoadRejectsMalformedRequestsAndDataImports)
		{
			Workspace missingWorkspace;
			missingWorkspace.write(L"input.obj", makeEntryObject());
			auto missingArgs = outputArgs(missingWorkspace);
			missingArgs.push_back(L"/DELAYLOAD:missing.dll");
			missingArgs.push_back(missingWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(missingWorkspace, missingArgs), "no imports from that DLL were selected");

			Workspace optionWorkspace;
			optionWorkspace.write(L"input.obj", makeEntryObject());
			auto optionArgs = outputArgs(optionWorkspace);
			optionArgs.push_back(L"/DELAYLOAD");
			optionArgs.push_back(optionWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(optionWorkspace, optionArgs), "/DELAYLOAD requires an attached DLL name");

			Workspace directiveWorkspace;
			const auto directiveObject = makeObject({ makeDirectiveSection("-delayload:"), TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			assertFailureContains(linkObject(directiveWorkspace, directiveObject), ".drectve /DELAYLOAD requires a DLL name");

			Workspace dataWorkspace;
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 1, 1 } };
			dataWorkspace.write(L"input.obj", makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "__imp_importedData", 0, 0, 2,{} } }));
			dataWorkspace.write(L"data.lib", makeArchive({ { "data.obj", makeImportObject("importedData", "DATA.dll", "importedData", 17, 1) } }));
			auto dataArgs = outputArgs(dataWorkspace);
			dataArgs.push_back(L"/DELAYLOAD:DATA.dll");
			dataArgs.push_back(dataWorkspace.file(L"input.obj").wstring());
			dataArgs.push_back(dataWorkspace.file(L"data.lib").wstring());
			assertFailureContains(runLinker(dataWorkspace, dataArgs), "cannot be used with data imports");

			Workspace multipleWorkspace;
			TestSection multipleText{ ".text",{ 0xe8, 0, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xc3 } };
			multipleText.relocations = { { 1, 1, 4 },{ 6, 2, 4 } };
			multipleWorkspace.write(L"input.obj", makeObject({ multipleText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "GetCurrentProcessId", 0, 0, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			multipleWorkspace.write(L"kernel.lib", makeArchive({
				{ "pid.obj", makeImportObject("GetCurrentProcessId", "KERNEL32.dll", "GetCurrentProcessId", 0, 4) },
				{ "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto multipleArgs = outputArgs(multipleWorkspace);
			multipleArgs.push_back(L"/DELAYLOAD:KERNEL32.dll");
			multipleArgs.push_back(multipleWorkspace.file(L"input.obj").wstring());
			multipleArgs.push_back(multipleWorkspace.file(L"kernel.lib").wstring());
			assertFailureContains(runLinker(multipleWorkspace, multipleArgs), "currently supports one delay-loaded function import per link");
		}

		TEST_METHOD(ImportLibraryGeneratesDescriptorIatAndFunctionThunk)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xff, 0x25, 0, 0, 0, 0 } };
			text.relocations = { { 2, 1, 4 } };
			const auto object = makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "__imp_ExitProcess", 0, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			const auto import = makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 321, 4);
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", import } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto importRva = get32(image, optional + 120);
			Assert::IsTrue(importRva != 0, L"import directory RVA");
			const auto descriptor = rvaOffset(image, importRva);
			const auto nameRva = get32(image, descriptor + 12);
			const auto lookupRva = get32(image, descriptor);
			const auto iatRva = get32(image, descriptor + 16);
			Assert::IsTrue(cStringAtRva(image, nameRva) == "KERNEL32.dll", L"import DLL name");
			const auto lookup = rvaOffset(image, lookupRva);
			const auto iat = rvaOffset(image, iatRva);
			Assert::AreEqual<uint64_t>(get64(image, lookup), get64(image, iat), L"lookup and address tables begin identically");
			const auto hintNameRva = static_cast<uint32_t>(get64(image, lookup));
			const auto hintName = rvaOffset(image, hintNameRva);
			Assert::AreEqual<uint16_t>(321, get16(image, hintName), L"import hint");
			Assert::IsTrue(string(reinterpret_cast<const char*>(image.data() + hintName + 2), 11) == "ExitProcess", L"import symbol name");
			Assert::IsTrue(get64(image, lookup + 8) == 0 && get64(image, iat + 8) == 0, L"import thunk tables are null-terminated");
			Assert::IsTrue(get32(image, optional + 208) == iatRva && get32(image, optional + 212) == 16, L"IAT data directory");
		}

		TEST_METHOD(OrdinalImportUsesOrdinalThunkEncoding)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xff, 0x25, 0, 0, 0, 0 } };
			text.relocations = { { 2, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "__imp_OrdinalProc", 0, 0, 2,{} } }));
			workspace.write(L"ordinal.lib", makeArchive({ { "ordinal.obj", makeImportObject("OrdinalProc", "ORDINAL.dll", "", 77, 0) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"ordinal.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto importRva = get32(image, optionalHeader(image) + 120);
			const auto lookup = rvaOffset(image, get32(image, rvaOffset(image, importRva)));
			Assert::AreEqual<uint64_t>(0x800000000000004dULL, get64(image, lookup), L"ordinal import encoding");
		}

		TEST_METHOD(DataImportCreatesIatWithoutFunctionThunk)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 1, 1 } };
			workspace.write(L"input.obj", makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "__imp_importedData", 0, 0, 2,{} } }));
			workspace.write(L"data.lib", makeArchive({ { "data.obj", makeImportObject("importedData", "DATA.dll", "importedData", 17, 1) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"data.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto importRva = get32(image, optionalHeader(image) + 120);
			const auto descriptor = rvaOffset(image, importRva);
			const auto iatRva = get32(image, descriptor + 16);
			Assert::IsTrue(iatRva != 0, L"data import IAT is present");
			Assert::IsTrue(get32(image, optionalHeader(image) + 16) != 0, L"entry RVA remains a valid section-based address");
		}

	};
}
