#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(OptionAndJwlinkTests)
	{
	public:
		TEST_METHOD(JwlinkDirectiveSyntaxLinksLikeEquivalentSwitches)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			// Mirrors test\run.bat: format win pe, option map=..., file ..., lib ..., name ...
			assertSuccess(runLinker(workspace, { L"format", L"win", L"pe", L"option", L"map=directive.map,quiet,start=main", L"file", L"input", L"lib", L"kernel.lib", L"name", L"directive.exe" }));
			assertSuccess(runLinker(workspace, { L"/MAP:switch.map", L"/NOLOGO", L"/ENTRY:main", L"/OUT:switch.exe", L"input.obj", L"kernel.lib" }));
			Assert::IsTrue(workspace.read(L"directive.exe") == workspace.read(L"switch.exe"), L"directive and switch spellings produce identical images");
			Assert::IsTrue(fs::exists(workspace.file(L"directive.map")), L"option map=<file> writes the requested map");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"directive.exe"), {}).exitCode, L"directive-linked image executes");

			workspace.writeText(L"build.rsp", L"format windows pe\r\noption start=main\r\nfile input.obj\r\nlibrary kernel.lib\r\nname response.exe\r\n");
			assertSuccess(runLinker(workspace, { L"@build.rsp" }));
			Assert::IsTrue(workspace.read(L"response.exe") == workspace.read(L"switch.exe"), L"directives in a response file link identically");
		}

		TEST_METHOD(StackHeapMergeAndSectionControlsAreWrittenToTheImage)
		{
			Workspace workspace;
			TestSection rdata{ ".rdata",{ 1, 2, 3, 4 }, 0x40000040 };
			TestSection text{ ".text",{ 0xc3 } };
			TestSection custom{ ".custom",{ 9 }, 0x40000040 };
			workspace.write(L"input.obj", makeObject({ text, rdata, custom }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			auto args = outputArgs(workspace);
			for (const auto* option : { L"/STACK:0x400000,0x8000", L"/HEAP:0x200000,0x4000", L"/MERGE:.rdata=.custom", L"/SECTION:.custom,RW" }) args.insert(args.begin() + 1, option);
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint64_t>(0x400000, get64(image, optional + 72));
			Assert::AreEqual<uint64_t>(0x8000, get64(image, optional + 80));
			Assert::AreEqual<uint64_t>(0x200000, get64(image, optional + 88));
			Assert::AreEqual<uint64_t>(0x4000, get64(image, optional + 96));
			const auto custom0 = sectionHeader(image, ".custom");
			Assert::AreEqual<uint32_t>(0xc0000000u, get32(image, custom0 + 36) & 0xfe000000u);
			const auto raw = get32(image, custom0 + 20);
			Assert::AreEqual<uint8_t>(1, image[raw], L".rdata bytes lead the merged section");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, custom0 + 16), uint8_t{ 9 }) != image.begin() + raw + get32(image, custom0 + 16), L".custom bytes follow");
			const auto sectionCount = get16(image, peHeader(image) + 6);
			for (size_t i = 0; i < sectionCount; ++i)
			{
				const auto name = reinterpret_cast<const char*>(image.data() + sectionTable(image) + i * 40);
				Assert::IsTrue(strncmp(name, ".rdata", 8) != 0, L".rdata was merged away");
			}
			validatePeLayout(image);
		}

		TEST_METHOD(WholeArchiveAndIncludePullInUnreferencedMembers)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			const auto extra = makeObject({ TestSection{ ".text",{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee } } }, { TestSymbol{ "unused_fn", 0, 1, 2,{} } });
			workspace.write(L"x.lib", makeArchive({ { "extra.obj", extra } }));
			const auto contains = [](const Bytes& image) { const Bytes needle{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee }; return search(image.begin(), image.end(), needle.begin(), needle.end()) != image.end(); };
			auto link = [&](const wstring& out, vector<wstring> extraArgs)
			{
				vector<wstring> args{ L"/ENTRY:main", L"/OUT:" + workspace.file(out).wstring() };
				args.insert(args.end(), extraArgs.begin(), extraArgs.end());
				args.push_back(workspace.file(L"input.obj").wstring());
				args.push_back(workspace.file(L"x.lib").wstring());
				assertSuccess(runLinker(workspace, args));
				return workspace.read(out);
			};
			Assert::IsFalse(contains(link(L"plain.exe", {})), L"unreferenced member stays out by default");
			Assert::IsTrue(contains(link(L"whole.exe", { L"/WHOLEARCHIVE:x.lib" })), L"/WHOLEARCHIVE pulls in every member");
			Assert::IsTrue(contains(link(L"all.exe", { L"/WHOLEARCHIVE" })), L"bare /WHOLEARCHIVE applies to all archives");
			Assert::IsTrue(contains(link(L"inc.exe", { L"/INCLUDE:unused_fn" })), L"/INCLUDE forces the defining member");
			const auto missing = runLinker(workspace, { L"/ENTRY:main", L"/INCLUDE:nope", L"/OUT:" + workspace.file(L"m.exe").wstring(), workspace.file(L"input.obj").wstring() });
			assertFailureContains(missing, "nope");
		}

		TEST_METHOD(NamedWholeArchiveLoadsAndDistinguishesLibraries)
		{
			Workspace workspace;
			fs::create_directories(workspace.file(L"first"));
			fs::create_directories(workspace.file(L"second"));
			const auto entry = makeObject({ TestSection{ ".text",{ 0xb8, 42, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			const auto extra = makeObject({ TestSection{ ".text",{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee } } }, { TestSymbol{ "unused_fn", 0, 1, 2,{} } });
			const auto other = makeObject({ TestSection{ ".text",{ 0x11, 0x22, 0x33, 0x44, 0x55 } } }, { TestSymbol{ "other_fn", 0, 1, 2,{} } });
			workspace.write(L"first\\same.lib", makeArchive({ { "entry.obj", entry }, { "extra.obj", extra } }));
			workspace.write(L"second\\same.lib", makeArchive({ { "other.obj", other } }));
			const Bytes marker{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee };
			const Bytes otherMarker{ 0x11, 0x22, 0x33, 0x44, 0x55 };
			const auto contains = [](const Bytes& image, const Bytes& needle) { return search(image.begin(), image.end(), needle.begin(), needle.end()) != image.end(); };
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:alone.exe", L"/wholearchive:first\\same.lib" }));
			Assert::IsTrue(contains(workspace.read(L"alone.exe"), marker), L"named option loads unreferenced archive members");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"alone.exe"), {}).exitCode);
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:duplicate.exe", L"/wholearchive:first\\same.lib", L"first\\same.lib", L"/wholearchive:first\\same.lib", L"second\\same.lib" }));
			const auto image = workspace.read(L"duplicate.exe");
			Assert::IsTrue(contains(image, marker));
			Assert::IsFalse(contains(image, otherMarker), L"same basename in a different directory is not selected wholesale");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"duplicate.exe"), {}).exitCode);
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:search.exe", L"/libpath:first", L"/wholearchive:same", L"/defaultlib:same.lib" }));
			Assert::IsTrue(contains(workspace.read(L"search.exe"), marker), L"named library uses library search and is not loaded twice as a default library");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"search.exe"), {}).exitCode);
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:nodefault.exe", L"/nodefaultlib", L"/wholearchive:first\\same.lib" }));
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"nodefault.exe"), {}).exitCode, L"explicit whole archive is not suppressed by /NODEFAULTLIB");
			assertFailureContains(runLinker(workspace, { L"/entry:main", L"/wholearchive:missing.lib" }), "missing.lib");
			workspace.write(L"notarchive.obj", entry);
			assertFailureContains(runLinker(workspace, { L"/entry:main", L"/wholearchive:notarchive.obj" }), "/WHOLEARCHIVE requires an archive");
		}

		TEST_METHOD(NamedWholeArchiveMatchesMsvcForStandaloneLibrary)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"lib.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; whole-archive comparison skipped"); return; }
			Workspace workspace;
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xb8, 42, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			workspace.write(L"extra.obj", makeObject({ TestSection{ ".text",{ 0xb8, 73, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "unused_fn", 0, 1, 2,{} } }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/out:members.lib", L"entry.obj", L"extra.obj" }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/opt:noref", L"/out:msvc.exe", L"/wholearchive:members.lib" }));
			assertSuccess(runLinker(workspace, { L"/nodefaultlib", L"/entry:main", L"/out:jw2.exe", L"/wholearchive:members.lib" }));
			const Bytes marker{ 0xb8, 73, 0, 0, 0, 0xc3 };
			for (const auto* name : { L"msvc.exe", L"jw2.exe" })
			{
				const auto image = workspace.read(name);
				Assert::IsTrue(search(image.begin(), image.end(), marker.begin(), marker.end()) != image.end(), L"unreferenced member is present");
				Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(name), {}).exitCode);
			}
		}

		TEST_METHOD(VerboseReportsInputsAndLayout)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/VERBOSE");
			args.push_back(workspace.file(L"input.obj").wstring());
			const auto result = runLinker(workspace, args);
			assertSuccess(result);
			Assert::IsTrue(result.output.find("jwlink2: object") != string::npos && result.output.find("jwlink2: section .text") != string::npos && result.output.find("jwlink2: wrote") != string::npos);
		}

		TEST_METHOD(ManifestIsEmbeddedAsResourceOrWrittenExternally)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			const auto input = workspace.file(L"input.obj").wstring();
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST:EMBED", L"/OUT:" + workspace.file(L"embed.exe").wstring(), input }));
			const auto image = workspace.read(L"embed.exe");
			validatePeLayout(image);
			const auto resource = sectionHeader(image, ".rsrc");
			const auto base = get32(image, resource + 20);
			const auto directoryRva = get32(image, optionalHeader(image) + 112 + 16);
			Assert::AreEqual(get32(image, resource + 12), directoryRva, L"resource data directory points at .rsrc");
			Assert::AreEqual<uint32_t>(24, get32(image, base + 16));
			Assert::AreEqual<uint32_t>(1, get32(image, base + 24 + 16), L"EXE manifest resource ID is 1");
			const auto dataRva = get32(image, base + 72);
			const auto size = get32(image, base + 76);
			const string text(reinterpret_cast<const char*>(image.data() + base + (dataRva - directoryRva)), size);
			Assert::IsTrue(text.find("requestedExecutionLevel") != string::npos);

			workspace.writeText(L"app.manifest", L"<assembly>custom</assembly>");
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:main", L"/MANIFESTINPUT:" + workspace.file(L"app.manifest").wstring(), L"/OUT:" + workspace.file(L"m.dll").wstring(), input }));
			const auto dll = workspace.read(L"m.dll");
			const auto dllBase = get32(dll, sectionHeader(dll, ".rsrc") + 20);
			Assert::AreEqual<uint32_t>(2, get32(dll, dllBase + 24 + 16), L"DLL manifest resource ID is 2");
			const string custom(reinterpret_cast<const char*>(dll.data() + dllBase + 88), get32(dll, dllBase + 76));
			Assert::AreEqual<string>("<assembly>custom</assembly>", custom);
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST:EMBED,ID=17", L"/MANIFESTINPUT:" + workspace.file(L"app.manifest").wstring(), L"/OUT:custom-id.exe", input }));

			for (const auto& [name, id, expected] : vector<tuple<wstring, WORD, string>>{ { L"embed.exe", 1, text }, { L"m.dll", 2, custom }, { L"custom-id.exe", 17, custom } })
			{
				const auto module = LoadLibraryExW(workspace.file(name).c_str(), nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
				Assert::IsNotNull(module, L"Windows loads image resources");
				const auto resourceInfo = FindResourceExW(module, MAKEINTRESOURCEW(24), MAKEINTRESOURCEW(id), 0x409);
				Assert::IsNotNull(resourceInfo, L"Windows locates the manifest by type, ID and language");
				const auto resourceData = LoadResource(module, resourceInfo);
				Assert::IsNotNull(resourceData);
				const auto data = static_cast<const char*>(LockResource(resourceData));
				Assert::IsNotNull(data);
				Assert::AreEqual(expected, string(data, SizeofResource(module, resourceInfo)));
				Assert::IsTrue(FreeLibrary(module) != 0);
			}

			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST", L"/OUT:" + workspace.file(L"ext.exe").wstring(), input }));
			Assert::IsTrue(fs::exists(workspace.file(L"ext.exe.manifest")));
			Assert::IsTrue(workspace.read(L"ext.exe").size() > 0);
			Assert::IsTrue(fs::exists(workspace.file(L"ext.exe.manifest")) && !fs::exists(workspace.file(L"plain.manifest")));

			TestSection rsrc{ ".rsrc",{ 0, 0, 0, 0 }, 0x40000040 };
			workspace.write(L"res.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, rsrc }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST:EMBED", L"/OUT:" + workspace.file(L"conflict.exe").wstring(), workspace.file(L"res.obj").wstring() }), "already contains a .rsrc");
		}

		TEST_METHOD(OptionMatrixAcceptsAndRejectsDocumentedForms)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			struct Row { vector<wstring> args; const char* rejected; };
			const vector<Row> matrix = {
				// JWlink directives: accepted
				{ { L"format", L"windows", L"pe", L"file", L"input.obj", L"name", L"m1.exe", L"option", L"start=main" }, nullptr },
				{ { L"form", L"win", L"nt", L"fi", L"input", L"na", L"m2", L"op", L"start=main,quiet,caseexact" }, nullptr },
				{ { L"format", L"windows", L"pe", L"dll", L"file", L"dll.obj", L"export", L"api", L"option", L"implib=m3.lib,nodefaultlibs", L"name", L"m3.dll" }, nullptr },
				{ { L"system", L"nt_dll", L"file", L"dll.obj", L"export", L"api", L"name", L"m4.dll" }, nullptr },
				{ { L"system", L"nt_win", L"file", L"input.obj", L"option", L"start=main" }, nullptr },
				{ { L"runtime", L"console=6.0", L"debug", L"all", L"file", L"input.obj", L"option", L"start=main,map" }, nullptr },
				{ { L"format", L"win", L"pe", L"file", L"input.obj", L"libpath", L".", L"library", L"kernel", L"disable", L"1014", L"option", L"start=main,offset=0x10000,objalign=8192,norelocs" }, nullptr },
				// JWlink directives: rejected explicitly
				{ { L"format", L"os2", L"file", L"input.obj" }, "unsupported FORMAT" },
				{ { L"format", L"windows", L"file", L"input.obj" }, "requires PE" },
				{ { L"format", L"windows", L"pe", L"elf", L"file", L"input.obj" }, "unsupported FORMAT modifier" },
				{ { L"system", L"dos", L"file", L"input.obj" }, "unsupported SYSTEM" },
				{ { L"runtime", L"native", L"file", L"input.obj" }, "unsupported RUNTIME" },
				{ { L"debug", L"dwarf", L"file", L"input.obj" }, "unsupported DEBUG format" },
				{ { L"option", L"stack=bad", L"file", L"input.obj" }, "JWlink size option" },
				{ { L"option", L"linkversion=256.0", L"file", L"input.obj" }, "invalid JWlink version" },
				{ { L"option", L"nxcompat=no", L"file", L"input.obj" }, "does not accept" },
				{ { L"option", L"eliminate", L"file", L"input.obj" }, "unsupported JWlink OPTION" },
				{ { L"option", L"start", L"file", L"input.obj" }, "requires a value" },
				{ { L"alias", L"a=b", L"file", L"input.obj" }, "unsupported JWlink directive 'alias'" },
				{ { L"segment", L"x", L"file", L"input.obj" }, "unsupported JWlink directive 'segment'" },
				{ { L"order", L"x", L"file", L"input.obj" }, "unsupported JWlink directive 'order'" },
				{ { L"format", L"win", L"pe", L"file" }, "requires an argument" },
				{ { L"format", L"win", L"pe", L"name", L"a.exe", L"b.exe", L"file", L"input.obj" }, "exactly one output" },
				{ { L"format", L"win", L"pe", L"export", L"a.bad", L"file", L"input.obj" }, "ordinal" },
				{ { L"format", L"win", L"pe", L"anonymousexport", L"a", L"file", L"input.obj" }, "NONAME requires" },
				{ { L"format", L"win", L"pe", L"disable", L"abc", L"file", L"input.obj" }, "DISABLE requires" },
				{ { L"format", L"win", L"pe", L"/OUT:x.exe", L"file", L"input.obj" }, "cannot be mixed" },
				// MSVC-style switches: accepted
				{ { L"/ENTRY:main", L"/OUT:s1.exe", L"/NOLOGO", L"/MACHINE:X64", L"/INCREMENTAL:NO", L"input.obj" }, nullptr },
				{ { L"/ENTRY:main", L"/OUT:s2.exe", L"/NXCOMPAT", L"/DYNAMICBASE:NO", L"/SUBSYSTEM:WINDOWS,6.0", L"input.obj" }, nullptr },
				// Switches whose behavior JWlink2 does not implement are rejected, never silently ignored.
				// Controls that are implemented are accepted; invalid arguments are diagnosed.
				{ { L"/ENTRY:main", L"/OUT:s3.exe", L"/MANIFEST:EMBED", L"/STACK:0x200000,0x2000", L"/HEAP:0x200000", L"/VERBOSE", L"input.obj" }, nullptr },
				{ { L"/ENTRY:main", L"/OUT:s4.exe", L"/MANIFEST", L"/MERGE:.rdata=.text", L"/SECTION:.text,ER", L"/INCLUDE:main", L"/WHOLEARCHIVE:kernel.lib", L"/WHOLEARCHIVE", L"/MANIFEST:NO", L"input.obj", L"kernel.lib" }, nullptr },
				{ { L"/MANIFEST:BOGUS", L"input.obj" }, "/MANIFEST accepts" },
				{ { L"/STACK:0x1000,0x2000", L"input.obj" }, "reserve must be nonzero" },
				{ { L"/HEAP:abc", L"input.obj" }, "/HEAP" },
				{ { L"/MERGE:.rdata", L"input.obj" }, "/MERGE requires" },
				{ { L"/MERGE:.pdata=.text", L"input.obj" }, "linker-managed" },
				{ { L"/MERGE:.a=.a", L"input.obj" }, "same section" },
				{ { L"/MERGE:.a=.b", L"/MERGE:.b=.c", L"input.obj" }, "chain" },
				{ { L"/SECTION:.x,Q", L"input.obj" }, "is not one of" },
				{ { L"/SECTION:.x", L"input.obj" }, "/SECTION requires" },
				{ { L"/VERBOSE:1", L"input.obj" }, "does not accept a value" },
				{ { L"/INCLUDE:", L"input.obj" }, "requires a symbol" },
				{ { L"/MANIFESTINPUT:missing.manifest", L"input.obj" }, "missing.manifest" },
				{ { L"/OPT:REF", L"input.obj" }, "unsupported option" },
				{ { L"/LTCG", L"input.obj" }, "unsupported option" },
				{ { L"/MACHINE:ARM64", L"input.obj" }, "supports /MACHINE:X64 or /MACHINE:X86" },
				{ { L"/INCREMENTAL", L"input.obj" }, "INCREMENTAL:NO" }
			};
			for (const auto& row : matrix)
			{
				const auto result = runLinker(workspace, row.args);
				wstring label;
				for (const auto& argument : row.args) label += argument + L" ";
				if (row.rejected) Assert::IsTrue(result.exitCode != 0 && result.output.find(row.rejected) != string::npos, (L"expected rejection '" + wstring(row.rejected, row.rejected + strlen(row.rejected)) + L"' for: " + label + wstring(result.output.begin(), result.output.end())).c_str());
				else Assert::AreEqual<DWORD>(0, result.exitCode, (L"expected success for: " + label + wstring(result.output.begin(), result.output.end())).c_str());
			}
			Assert::IsTrue(fs::exists(workspace.file(L"m2.exe")), L"directive name without an extension gets .exe");
			Assert::IsTrue(fs::exists(workspace.file(L"m3.dll")) && fs::exists(workspace.file(L"m3.lib")), L"directive DLL link writes the DLL and import library");
		}

		TEST_METHOD(JwlinkPeOptionsMatchHeaderFields)
		{
			Workspace original;
			Workspace replacement;
			const auto object = makeObject({ TestSection{ ".text", { 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			original.write(L"program.obj", object);
			replacement.write(L"program.obj", object);
			const vector<wstring> args{ L"format", L"win", L"pe", L"option", L"quiet,start=main,stack=2048,heapsize=3072,version=7.8,osversion=10.4,linkversion=12.34,nxcompat,nolargeaddressaware", L"file", L"program.obj", L"name", L"out.exe" };
			assertSuccess(runExecutable(original, jwlinkPath(), args));
			assertSuccess(runLinker(replacement, args));
			const auto expected = original.read(L"out.exe");
			const auto actual = replacement.read(L"out.exe");
			const auto expectedOptional = optionalHeader(expected);
			const auto actualOptional = optionalHeader(actual);
			for (const size_t offset : { 2u, 3u }) Assert::AreEqual<uint8_t>(expected[expectedOptional + offset], actual[actualOptional + offset]);
			for (const size_t offset : { 40u, 42u, 44u, 46u }) Assert::AreEqual<uint16_t>(get16(expected, expectedOptional + offset), get16(actual, actualOptional + offset));
			for (const size_t offset : { 72u, 80u, 88u, 96u }) Assert::AreEqual<uint64_t>(get64(expected, expectedOptional + offset), get64(actual, actualOptional + offset));
			Assert::AreEqual<uint16_t>(peFileCharacteristics(expected) & 0x20, peFileCharacteristics(actual) & 0x20);
			Assert::AreEqual<uint16_t>(get16(expected, expectedOptional + 70) & 0x100, get16(actual, actualOptional + 70) & 0x100);
			const vector<wstring> repeated{ L"format", L"win", L"pe", L"option", L"quiet,start=main,stack=8192,stack=512,heapsize=1024,heapsize=8192,version=2.3,version=4,osversion=6.7,osversion=8,linkversion=4.5,linkversion=9,nolargeaddressaware,largeaddressaware", L"file", L"program.obj", L"name", L"repeat.exe" };
			assertSuccess(runExecutable(original, jwlinkPath(), repeated));
			assertSuccess(runLinker(replacement, repeated));
			const auto originalRepeat = original.read(L"repeat.exe");
			const auto replacementRepeat = replacement.read(L"repeat.exe");
			const auto eo = optionalHeader(originalRepeat), ao = optionalHeader(replacementRepeat);
			for (const size_t offset : { 2u, 3u }) Assert::AreEqual<uint8_t>(originalRepeat[eo + offset], replacementRepeat[ao + offset]);
			for (const size_t offset : { 40u, 42u, 44u, 46u }) Assert::AreEqual<uint16_t>(get16(originalRepeat, eo + offset), get16(replacementRepeat, ao + offset));
			for (const size_t offset : { 72u, 80u, 88u, 96u }) Assert::AreEqual<uint64_t>(get64(originalRepeat, eo + offset), get64(replacementRepeat, ao + offset));
			Assert::AreEqual<uint16_t>(peFileCharacteristics(originalRepeat) & 0x20, peFileCharacteristics(replacementRepeat) & 0x20);
		}

		TEST_METHOD(JwlinkExportOrdinalAliasAndAnonymousWorkAtRuntime)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text", { 0xb8, 42, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "func", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"format", L"win", L"pe", L"dll", L"file", L"input.obj", L"export", L"publicName.5=func", L"anonymousexport", L"hidden.7=func", L"name", L"out.dll" }));
			const auto module = LoadLibraryW(workspace.file(L"out.dll").c_str());
			Assert::IsNotNull(module, L"Windows loads the linked DLL");
			const auto named = GetProcAddress(module, "publicName");
			const auto ordinal = GetProcAddress(module, MAKEINTRESOURCEA(5));
			const auto anonymous = GetProcAddress(module, MAKEINTRESOURCEA(7));
			Assert::IsNotNull(named);
			Assert::IsTrue(named == ordinal && ordinal == anonymous, L"ordinals and alias target the same function");
			Assert::IsNull(GetProcAddress(module, "hidden"), L"anonymous export has no name");
			Assert::AreEqual<int>(42, reinterpret_cast<int(*)()>(named)());
			FreeLibrary(module);
		}

		TEST_METHOD(JwlinkReferencePullsArchiveMember)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text", { 0x42, 0x53, 0x64 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			const auto result = runLinker(workspace, { L"format", L"win", L"pe", L"option", L"start=main", L"file", L"input.obj", L"library", L"helpers.lib", L"reference", L"helper", L"name", L"out.exe" });
			assertSuccess(result);
			const auto image = workspace.read(L"out.exe");
			const auto header = sectionHeader(image, ".text");
			const auto raw = get32(image, header + 20);
			const Bytes needle{ 0x42, 0x53, 0x64 };
			Assert::IsTrue(search(image.begin() + raw, image.begin() + raw + get32(image, header + 16), needle.begin(), needle.end()) != image.begin() + raw + get32(image, header + 16));
			assertFailureContains(runLinker(workspace, { L"format", L"win", L"pe", L"file", L"input.obj", L"reference", L"missing", L"name", L"fail.exe" }), "missing");
		}

		TEST_METHOD(JwlinkDirectiveDefaultsAndExplicitEntry)
		{
			Workspace original;
			Workspace replacement;
			const auto object = makeObject({ TestSection{ ".text", { 0x90, 0xc3 } } }, { TestSymbol{ "main", 1, 1, 2,{} } });
			original.write(L"program.obj", object);
			replacement.write(L"program.obj", object);
			const vector<wstring> args{ L"format", L"win", L"pe", L"option", L"start=main", L"file", L"program.obj" };
			assertSuccess(runExecutable(original, jwlinkPath(), args));
			assertSuccess(runLinker(replacement, args));
			const auto expected = original.read(L"program.exe");
			const auto actual = replacement.read(L"program.exe");
			for (const auto* image : { &expected, &actual })
			{
				Assert::AreEqual<uint16_t>(3, get16(*image, optionalHeader(*image) + 68));
				const auto entry = get32(*image, optionalHeader(*image) + 16);
				Assert::AreEqual<uint8_t>(0xc3, (*image)[rvaOffset(*image, entry)], L"explicit start selects main");
			}
			const auto defaultResult = runLinker(replacement, { L"format", L"win", L"pe", L"file", L"program.obj", L"name", L"fallback.exe" });
			assertSuccess(defaultResult);
			Assert::IsTrue(defaultResult.output.find("warning: no start address") != string::npos);
			const auto fallback = replacement.read(L"fallback.exe");
			Assert::AreEqual<uint8_t>(0x90, fallback[rvaOffset(fallback, get32(fallback, optionalHeader(fallback) + 16))], L"directive-mode fallback starts at the first section");
			const vector<wstring> guiArgs{ L"format", L"win", L"pe", L"runtime", L"windows", L"option", L"start=main", L"file", L"program.obj", L"name", L"gui.exe" };
			assertSuccess(runExecutable(original, jwlinkPath(), guiArgs));
			assertSuccess(runLinker(replacement, guiArgs));
			const auto originalGui = original.read(L"gui.exe");
			const auto replacementGui = replacement.read(L"gui.exe");
			Assert::AreEqual<uint16_t>(2, get16(originalGui, optionalHeader(originalGui) + 68));
			Assert::AreEqual<uint16_t>(2, get16(replacementGui, optionalHeader(replacementGui) + 68));
			assertSuccess(runLinker(replacement, { L"/OUT:switch.exe", L"program.obj" }));
			const auto switched = replacement.read(L"switch.exe");
			Assert::AreEqual<uint8_t>(0xc3, switched[rvaOffset(switched, get32(switched, optionalHeader(switched) + 16))], L"switch-mode default remains main");
		}

		TEST_METHOD(RepresentativeObjectMatchesJWlinkPeBehavior)
		{
			Workspace workspace;
			const auto object = makeObject(
				{ TestSection{ ".text",{ 0xc3, 0x2a } }, makeDirectiveSection("-entry:main ") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			workspace.write(L"input.obj", object);
			const auto jwlink2Args = vector<wstring>{ L"/OUT:" + workspace.file(L"jwlink2.exe").wstring(), workspace.file(L"input.obj").wstring() };
			assertSuccess(runExecutable(workspace, jwlink2Path(), jwlink2Args));
			const auto jwlinkArgs = vector<wstring>{
				L"format", L"win", L"pe", L"option", L"quiet,start=main",
				L"file", workspace.file(L"input.obj").wstring(),
				L"name", workspace.file(L"jwlink.exe").wstring()
			};
			assertSuccess(runExecutable(workspace, jwlinkPath(), jwlinkArgs));

			const auto jwlink2Image = workspace.read(L"jwlink2.exe");
			const auto jwlinkImage = workspace.read(L"jwlink.exe");
			for (const auto* image : { &jwlink2Image, &jwlinkImage })
			{
				const auto optional = optionalHeader(*image);
				const auto pe = peHeader(*image);
				Assert::AreEqual<uint16_t>(0x8664, get16(*image, pe + 4), L"both linkers target AMD64");
				Assert::IsTrue((peFileCharacteristics(*image) & 0x2000) == 0, L"both outputs are executables, not DLLs");
				Assert::AreEqual<uint16_t>(3, get16(*image, optional + 68), L"both outputs use the console subsystem");
				const auto entryRva = get32(*image, optional + 16);
				const auto entryOffset = rvaOffset(*image, entryRva);
				Assert::AreEqual<uint8_t>(0xc3, (*image)[entryOffset], L"both entry points execute the return instruction");
				const auto text = sectionHeader(*image, ".text");
				Assert::IsTrue(get32(*image, text + 16) >= 2, L"both images preserve initialized code bytes");
				const auto textOffset = get32(*image, text + 20);
				Assert::AreEqual<uint8_t>(0xc3, (*image)[textOffset], L"both images preserve the entry-point instruction");
				Assert::AreEqual<uint8_t>(0x2a, (*image)[textOffset + 1], L"both images preserve the second code-section byte");
			}
		}

	};
}
