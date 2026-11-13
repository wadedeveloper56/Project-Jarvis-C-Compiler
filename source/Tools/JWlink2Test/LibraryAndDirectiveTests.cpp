#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(LibraryAndDirectiveTests)
	{
	public:
		TEST_METHOD(MsvcToolchainArchiveIndexesSelectRegularObjectMember)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 1, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "archiveHelper", 0, 0, 2,{} } }));
			workspace.write(L"toolchain.lib", makeMsvcIndexedArchive());
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"toolchain.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto textHeader = sectionHeader(image, ".text");
			const auto raw = get32(image, textHeader + 20);
			const auto target = static_cast<size_t>(raw) + 5 + get32(image, raw + 1);
			Assert::IsTrue(target < image.size(), L"MSVC archive symbol index resolves the call target within the linked image");
			Assert::AreEqual<uint8_t>(0xc3, image[target], L"toolchain-generated archive member code is included");
		}

		TEST_METHOD(ActualLibrariansSelectLongNamedMembersAndHonorLibraryOrder)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"lib.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64") toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 librarian not found; actual MSVC archive test skipped"); return; }
			Workspace workspace;
			TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 1, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			workspace.write(L"long_archive_member_name_first.obj", makeObject({ TestSection{ ".text",{ 0xb8, 37, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }));
			workspace.write(L"long_archive_member_name_second.obj", makeObject({ TestSection{ ".text",{ 0xb8, 49, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }));
			workspace.write(L"long_archive_member_name_unused.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "other", 0, 1, 2,{} } }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/out:first.lib", L"long_archive_member_name_first.obj", L"long_archive_member_name_unused.obj" }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/out:second.lib", L"long_archive_member_name_second.obj" }));
			assertSuccess(runExecutable(workspace, toolPath(L"JWlib.exe"), { L"-q", L"-n", L"-fac", L"jw.lib", L"+long_archive_member_name_first.obj", L"+long_archive_member_name_unused.obj" }, {}, false));
			for (const auto* library : { L"first.lib", L"jw.lib" })
			{
				assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:linked.exe", L"/map", L"input.obj", library }));
				Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"linked.exe"), {}).exitCode, L"actual librarian archive member executes");
				ifstream mapFile(workspace.file(L"linked.map"));
				const string map((istreambuf_iterator<char>(mapFile)), istreambuf_iterator<char>());
				Assert::IsTrue(map.find("long_archive_member_name_first.obj)\n") != string::npos, L"long archive member name is preserved exactly");
				Assert::IsTrue(map.find("long_archive_member_name_unused.obj") == string::npos, L"unused member is not extracted");
				mapFile.close();
				assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:linked.exe", L"/map", L"/include:other", L"input.obj", library }));
				ifstream bothFile(workspace.file(L"linked.map"));
				const string both((istreambuf_iterator<char>(bothFile)), istreambuf_iterator<char>());
				Assert::IsTrue(both.find("long_archive_member_name_unused.obj)\n") != string::npos, L"second long archive name resolves exactly");
			}
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:order.exe", L"input.obj", L"second.lib", L"first.lib", L"first.lib" }));
			Assert::AreEqual<DWORD>(49, runExecutable(workspace, workspace.file(L"order.exe"), {}).exitCode, L"first explicit archive wins duplicate symbols");
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:default.exe", L"/defaultlib:first.lib", L"/defaultlib:first.lib", L"input.obj", L"second.lib" }));
			Assert::AreEqual<DWORD>(49, runExecutable(workspace, workspace.file(L"default.exe"), {}).exitCode, L"explicit libraries precede repeated default libraries");
		}

		TEST_METHOD(Jwlink2ConsumesJwlibJwasmArchive)
		{
			Workspace workspace;
			workspace.writeText(L"entry.asm", L"option casemap:none\nextern helper:proc\n.code\npublic main\nmain proc\n call helper\n ret\nmain endp\nend\n");
			workspace.writeText(L"helper.asm", L"option casemap:none\n.code\npublic helper\nhelper proc\n mov eax, 37\n ret\nhelper endp\nend\n");
			assertSuccess(runExecutable(workspace, toolPath(L"JWasm.exe"), { L"-win64", L"-Foentry.obj", L"entry.asm" }));
			assertSuccess(runExecutable(workspace, toolPath(L"JWasm.exe"), { L"-win64", L"-Fohelper.obj", L"helper.asm" }));
			assertSuccess(runExecutable(workspace, toolPath(L"JWlib.exe"), { L"-q", L"-n", L"-fac", L"jw.lib", L"+helper.obj" }, {}, false));
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:new.exe", L"entry.obj", L"jw.lib" }));
			const auto image = workspace.read(L"new.exe");
			const Bytes marker{ 0xb8, 37, 0, 0, 0, 0xc3 };
			Assert::IsTrue(search(image.begin(), image.end(), marker.begin(), marker.end()) != image.end(), L"selected archive code appears in image");
			Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"new.exe"), {}).exitCode, L"JWasm's empty data section remains loadable");
		}

		TEST_METHOD(WindowsSdkImportLibraryResolvesAndExecutes)
		{
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles(x86)", programFiles, MAX_PATH);
			const auto root = fs::path(programFiles) / L"Windows Kits" / L"10" / L"Lib";
			fs::path sdkLibrary;
			error_code ignored;
			for (const auto& entry : fs::directory_iterator(root, ignored))
			{
				const auto candidate = entry.path() / L"um" / L"x64" / L"kernel32.lib";
				if (fs::exists(candidate) && (sdkLibrary.empty() || candidate.wstring() > sdkLibrary.wstring())) sdkLibrary = candidate;
			}
			if (sdkLibrary.empty()) { Logger::WriteMessage("Windows SDK x64 kernel32.lib not found; SDK import test skipped"); return; }
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:sdk.exe", L"input.obj", sdkLibrary.wstring() }));
			const auto image = workspace.read(L"sdk.exe");
			const auto descriptor = rvaOffset(image, get32(image, optionalHeader(image) + 120));
			Assert::IsTrue(cStringAtRva(image, get32(image, descriptor + 12)) == "KERNEL32.dll", L"SDK import descriptor names KERNEL32.dll");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"sdk.exe"), {}).exitCode, L"SDK import executes through the Windows loader");
		}

		TEST_METHOD(MsvcAndJwlink2ImportLibrariesInteroperate)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"lib.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; import-library interoperability test skipped"); return; }
			for (const auto* dll : { L"fixture.dll", L"fixture_with_a_long_module_name.dll" })
			{
				Workspace workspace;
				workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xb8, 37, 0, 0, 0, 0xc3, 0xb8, 5, 0, 0, 0, 0xc3 } }, TestSection{ ".data",{ 7, 0, 0, 0 }, 0xc0000040 } }, { TestSymbol{ "api", 0, 1, 2,{} }, TestSymbol{ "ordinalFn", 6, 1, 2,{} }, TestSymbol{ "value", 0, 2, 2,{} } }));
				assertSuccess(runLinker(workspace, { L"/dll", L"/noentry", L"/base:0x180000000", L"/export:api", L"/export:ordinalFn,@5,NONAME", L"/export:value,DATA", wstring(L"/out:") + dll, L"/implib:jw2.lib", L"dll.obj" }));
				workspace.writeText(L"fixture.def", wstring(L"LIBRARY ") + dll + L"\nEXPORTS\n api\n ordinalFn @5 NONAME\n value DATA\n");
				assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/machine:x64", L"/def:fixture.def", L"/out:msvc.lib" }));
				TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
				text.relocations = { { 1, 1, 4 } };
				workspace.write(L"client.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} } }));
				assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:jw2-client.exe", L"client.obj", L"msvc.lib" }));
				Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"jw2-client.exe"), {}).exitCode, L"JWlink2 consumes MSVC import library");
				if (wstring(dll) == L"fixture.dll")
				{
					assertSuccess(runExecutable(workspace, toolPath(L"JWlib.exe"), { L"-q", L"-n", L"-fac", L"-i6", L"-ic", L"jw.lib", L"++api.fixture.api" }, {}, false));
					assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:jwlib-client.exe", L"client.obj", L"jw.lib" }));
					Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"jwlib-client.exe"), {}).exitCode, L"JWlink2 consumes JWlib AMD64 import archive");
					workspace.write(L"other.obj", makeObject({ TestSection{ ".text",{ 0xb8, 49, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "otherApi", 0, 1, 2,{} } }));
					assertSuccess(runLinker(workspace, { L"/dll", L"/noentry", L"/base:0x190000000", L"/export:otherApi", L"/out:other.dll", L"/implib:other.lib", L"other.obj" }));
					TestSection pair{ ".text",{ 0xe8, 0, 0, 0, 0, 0xe9, 0, 0, 0, 0 } };
					pair.relocations = { { 1, 1, 4 }, { 6, 2, 4 } };
					workspace.write(L"pair.obj", makeObject({ pair }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} }, TestSymbol{ "otherApi", 0, 0, 2,{} } }));
					assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:pair.exe", L"pair.obj", L"jw2.lib", L"other.lib" }));
					Assert::AreEqual<DWORD>(49, runExecutable(workspace, workspace.file(L"pair.exe"), {}).exitCode, L"MSVC LINK resolves generated libraries for two DLLs");
				}
				assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:msvc-client.exe", L"client.obj", L"jw2.lib" }));
				Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"msvc-client.exe"), {}).exitCode, L"MSVC LINK consumes JWlink2 import library");
				TestSection mixed{ ".text",{ 0xe8, 0, 0, 0, 0, 0x48, 0x8b, 0x05, 0, 0, 0, 0, 0x8b, 0x00, 0xc3 } };
				mixed.relocations = { { 1, 1, 4 }, { 8, 2, 4 } };
				workspace.write(L"mixed.obj", makeObject({ mixed }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ordinalFn", 0, 0, 2,{} }, TestSymbol{ "__imp_value", 0, 0, 2,{} } }));
				for (const auto* library : { L"jw2.lib", L"msvc.lib" })
				{
					assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:mixed.exe", L"mixed.obj", library }));
					Assert::AreEqual<DWORD>(7, runExecutable(workspace, workspace.file(L"mixed.exe"), {}).exitCode, L"MSVC LINK resolves ordinal and data imports");
					assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:mixed-jw2.exe", L"mixed.obj", library }));
					Assert::AreEqual<DWORD>(7, runExecutable(workspace, workspace.file(L"mixed-jw2.exe"), {}).exitCode, L"JWlink2 resolves ordinal and data imports");
				}
			}
		}

		TEST_METHOD(ArchiveLongNameRequiresTerminator)
		{
			Workspace workspace;
			Bytes archive{ '!', '<', 'a', 'r', 'c', 'h', '>', '\n' };
			appendArchiveMember(archive, "//", Bytes{ 'b', 'a', 'd' });
			appendArchiveMember(archive, "/0", makeEntryObject());
			workspace.write(L"bad.lib", archive);
			assertFailureContains(runLinker(workspace, { L"/entry:main", L"/out:bad.exe", L"bad.lib" }), "unterminated or empty archive long name");
		}

		TEST_METHOD(MalformedMsvcArchiveIndexIsRejected)
		{
			Workspace workspace;
			auto archive = makeMsvcIndexedArchive();
			archive.at(68) = 0xff;
			archive.at(69) = 0xff;
			archive.at(70) = 0xff;
			archive.at(71) = 0xff;
			workspace.write(L"malformed.lib", archive);
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"malformed.lib").wstring());
			assertFailureContains(runLinker(workspace, args), "invalid first archive linker-member symbol count");
		}

		TEST_METHOD(StaticArchiveExtractsOnlyNeededObjectMember)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"helpers.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(sectionHeader(workspace.read(L"linked.exe"), ".text") != 0, L"archive member section should be linked");
		}

		TEST_METHOD(DefaultLibraryAndLibraryPathResolveArchive)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			fs::create_directories(workspace.path / L"lib");
			ofstream library(workspace.path / L"lib" / L"default.lib", ios::binary);
			const auto archive = makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "helper", 0, 1, 2,{} } }) } });
			library.write(reinterpret_cast<const char*>(archive.data()), static_cast<streamsize>(archive.size()));
			library.close();
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/DEFAULTLIB:default");
			args.insert(args.begin() + 1, L"/LIBPATH:" + (workspace.path / L"lib").wstring());
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
		}

		TEST_METHOD(DefaultLibraryExclusionsDoNotSuppressExplicitLibraries)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			const auto object = makeObject({ text, makeDirectiveSection("-defaultlib:helpers.lib") }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			for (const auto* exclusion : { L"/NODEFAULTLIB", L"/NODEFAULTLIB:helpers" })
			{
				const vector<wstring> args{ L"/ENTRY:main", L"/OUT:excluded.exe", exclusion, L"input.obj" };
				const auto result = runLinker(workspace, args);
				assertFailureContains(result, "helper");
				Assert::AreEqual<DWORD>(1, result.exitCode);
				Assert::IsFalse(fs::exists(workspace.file(L"excluded.exe")));
				assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:explicit.exe", exclusion, L"input.obj", L"helpers.lib" }));
			}
		}

		TEST_METHOD(CoffDirectiveEntryPointIsHonoredAndDirectiveSectionIsConsumed)
		{
			Workspace workspace;
			const auto object = makeObject(
				{ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-entry:mainCRTStartup ") },
				{ TestSymbol{ "mainCRTStartup", 0, 1, 2,{} } });
			workspace.write(L"input.obj", object);
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"linked.exe").wstring(), workspace.file(L"input.obj").wstring() }));
			const auto image = workspace.read(L"linked.exe");
			Assert::AreEqual<uint32_t>(0x1000, get32(image, optionalHeader(image) + 16), L".drectve entry selects the startup symbol");
			Assert::IsFalse(hasSection(image, ".drectve"), L"linker directives are not emitted as an image section");
		}

		TEST_METHOD(CoffDefaultLibraryDirectiveLoadsLibraryWithQuotedName)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			const auto object = makeObject({ text, makeDirectiveSection("-defaultlib:\"helpers.lib\"") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"linked.exe").wstring(), L"/ENTRY:main", workspace.file(L"input.obj").wstring() }));
			Assert::IsTrue(hasSection(workspace.read(L"linked.exe"), ".text"), L"default-library member was linked");
		}

		TEST_METHOD(CoffDirectivesFromSelectedArchiveMembersAreProcessed)
		{
			Workspace workspace;
			TestSection entryText{ ".text", Bytes(4, 0) };
			entryText.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ entryText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			TestSection helperText{ ".text", Bytes(4, 0) };
			helperText.relocations = { { 0, 1, 4 } };
			const auto helper = makeObject({ helperText, makeDirectiveSection("-defaultlib:secondary.lib") },
				{ TestSymbol{ "helper", 0, 1, 2,{} }, TestSymbol{ "secondarySymbol", 0, 0, 2,{} } });
			const auto unused = makeObject({ makeDirectiveSection("-unknown:ignored-unless-selected") }, { TestSymbol{ "unusedSymbol", 0, 0, 2,{} } });
			workspace.write(L"outer.lib", makeArchive({ { "helper.obj", helper },{ "unused.obj", unused } }));
			workspace.write(L"secondary.lib", makeArchive({ { "secondary.obj", makeObject({ TestSection{ ".data",{ 0x6b }, 0xc0000040 } },{ TestSymbol{ "secondarySymbol", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"outer.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto data = sectionHeader(image, ".data");
			Assert::AreEqual<uint8_t>(0x6b, image[get32(image, data + 20)], L"selected member default library is loaded and unused member directives are ignored");
		}

		TEST_METHOD(CoffIncludeDirectiveForcesArchiveMemberExtraction)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-include:forcedSymbol") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			workspace.write(L"input.obj", object);
			workspace.write(L"forced.lib", makeArchive({ { "forced.obj", makeObject({ TestSection{ ".data",{ 0x5a }, 0xc0000040 } },{ TestSymbol{ "forcedSymbol", 0, 1, 2,{} } }) } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"linked.exe").wstring(), L"/ENTRY:main", workspace.file(L"input.obj").wstring(), workspace.file(L"forced.lib").wstring() }));
			const auto image = workspace.read(L"linked.exe");
			const auto data = sectionHeader(image, ".data");
			Assert::AreEqual<uint8_t>(0x5a, image[get32(image, data + 20)], L"forced archive member contributes its data");
		}

		TEST_METHOD(MapReportsDeterministicImageAndArchiveContributions)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "zeta", 0, 1, 2,{} }, TestSymbol{ "alpha", 0, 1, 2,{} } }));
			workspace.write(L"forced.lib", makeArchive({
				{ "forced.obj", makeObject({ TestSection{ ".data",{ 0x5a }, 0xc0000040 } }, { TestSymbol{ "forced", 0, 1, 2,{} } }) },
				{ "unused.obj", makeObject({ TestSection{ ".data",{ 0x33 }, 0xc0000040 } }, { TestSymbol{ "unused", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, { L"/MAP", L"/INCLUDE:forced" });
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"forced.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			auto readMap = [&]() { ifstream map(workspace.file(L"linked.map")); return string((istreambuf_iterator<char>(map)), istreambuf_iterator<char>()); };
			const auto contents = readMap();
			const auto image = workspace.read(L"linked.exe");
			const auto base = get64(image, optionalHeader(image) + 24);
			const auto text = sectionHeader(image, ".text");
			const auto memory = contents.find("\nMemory Map\nAddress Symbol\n");
			const auto contributions = contents.find("\nInput Contributions\nAddress Size Section Module\n");
			Assert::IsTrue(memory != string::npos && contributions != string::npos && memory < contributions, L"map tables are present");
			const auto sectionRow = contents.find(" .text\n");
			Assert::IsTrue(sectionRow != string::npos && sectionRow < memory, L"output section row is present");
			const auto sectionStart = contents.rfind('\n', sectionRow) + 1;
			const auto sizeStart = contents.find(' ', sectionStart) + 1;
			Assert::AreEqual<uint64_t>(base + get32(image, text + 12), stoull(contents.substr(sectionStart, sizeStart - sectionStart), nullptr, 16), L"section address matches PE header");
			Assert::AreEqual<uint64_t>(get32(image, text + 8), stoull(contents.substr(sizeStart, sectionRow - sizeStart), nullptr, 16), L"section size matches PE header");
			const auto alpha = contents.find(" alpha\n", memory);
			const auto main = contents.find(" main\n", memory);
			const auto zeta = contents.find(" zeta\n", memory);
			Assert::IsTrue(alpha < main && main < zeta && zeta < contributions, L"equal-address symbols are sorted by name");
			const auto line = contents.rfind('\n', alpha);
			Assert::AreEqual<uint64_t>(base + get32(image, text + 12), stoull(contents.substr(line + 1, alpha - line - 1), nullptr, 16), L"symbol address matches PE layout");
			Assert::IsTrue(contents.find("forced.lib(forced.obj)", contributions) != string::npos, L"selected archive member is reported");
			Assert::IsTrue(contents.find("unused.obj", contributions) == string::npos, L"unselected archive member is absent");
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(contents == readMap(), L"map output is deterministic across links");

			workspace.write(L"reference.obj", makeEntryObject());
			assertSuccess(runExecutable(workspace, jwlinkPath(), { L"format", L"win", L"pe", L"option", L"quiet,start=main,map=" + workspace.file(L"reference.map").wstring(), L"file", workspace.file(L"reference.obj").wstring(), L"name", workspace.file(L"reference.exe").wstring() }));
			ifstream referenceFile(workspace.file(L"reference.map"));
			const string reference((istreambuf_iterator<char>(referenceFile)), istreambuf_iterator<char>());
			Assert::IsTrue(reference.find("Groups") != string::npos && reference.find("Segments") != string::npos && reference.find("Memory Map") != string::npos && reference.find("main") != string::npos, L"JWlink map uses its own group, segment and symbol tables");
		}

		TEST_METHOD(CoffAlignDirectiveSetsSectionAndFileAlignment)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-ALIGN:0x200") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint32_t>(0x200, get32(image, optional + 32), L"directive sets section alignment");
			Assert::AreEqual<uint32_t>(0x200, get32(image, optional + 36), L"small section alignment also sets file alignment");
			Assert::AreEqual<uint32_t>(0x200, get32(image, sectionHeader(image, ".text") + 12), L"first section RVA obeys alignment");

			Workspace commandLineWorkspace;
			const auto overridden = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-ALIGN:0x200") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			commandLineWorkspace.write(L"input.obj", overridden);
			auto args = outputArgs(commandLineWorkspace);
			args.insert(args.begin() + 1, L"/ALIGN:0x1000");
			args.push_back(commandLineWorkspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(commandLineWorkspace, args));
			const auto overriddenImage = commandLineWorkspace.read(L"linked.exe");
			Assert::AreEqual<uint32_t>(0x1000, get32(overriddenImage, optionalHeader(overriddenImage) + 32), L"command-line alignment overrides the object directive");
		}

		TEST_METHOD(CoffAlignCommDirectiveAlignsCommonSymbol)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-aligncomm:alignedCommon,32") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "alignedCommon", 4, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
			ifstream map(workspace.file(L"linked.map"));
			const string contents((istreambuf_iterator<char>(map)), istreambuf_iterator<char>());
			const auto symbolPosition = contents.find(" alignedCommon\n");
			Assert::IsTrue(symbolPosition != string::npos, L"aligned common symbol is present in the map");
			const auto lineStart = contents.rfind('\n', symbolPosition);
			const auto addressStart = lineStart == string::npos ? 0 : lineStart + 1;
			const auto addressEnd = contents.find(' ', addressStart);
			const auto address = stoull(contents.substr(addressStart, addressEnd - addressStart), nullptr, 16);
			Assert::AreEqual<uint64_t>(0, address % 32, L"ALIGNCOMM address respects the requested alignment");
		}

		TEST_METHOD(CoffExportDirectiveBuildsExportDirectoryAndAlias)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3, 0xc3 } }, makeDirectiveSection("-export:api=implementation,@7") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "implementation", 1, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto exportRva = get32(image, optional + 112);
			Assert::IsTrue(exportRva != 0 && get32(image, optional + 116) >= 40, L"export data directory is present");
			const auto directory = rvaOffset(image, exportRva);
			Assert::AreEqual<uint32_t>(7, get32(image, directory + 16), L"export ordinal base");
			Assert::AreEqual<uint32_t>(1, get32(image, directory + 20), L"number of exported functions");
			Assert::AreEqual<uint32_t>(1, get32(image, directory + 24), L"number of named exports");
			const auto functions = rvaOffset(image, get32(image, directory + 28));
			const auto names = rvaOffset(image, get32(image, directory + 32));
			const auto ordinals = rvaOffset(image, get32(image, directory + 36));
			const auto text = sectionHeader(image, ".text");
			Assert::AreEqual<uint32_t>(get32(image, text + 12) + 1, get32(image, functions), L"alias resolves to the internal symbol RVA");
			Assert::IsTrue(cStringAtRva(image, get32(image, names)) == "api", L"public export name");
			Assert::AreEqual<uint16_t>(0, get16(image, ordinals), L"export name maps to ordinal index zero");
		}

	};
}
