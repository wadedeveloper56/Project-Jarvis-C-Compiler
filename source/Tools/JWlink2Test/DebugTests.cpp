#include "pch.h"
#include "TestFixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace JWlink2Test::Fixtures;
namespace fs = filesystem;

namespace JWlink2Test
{
	TEST_CLASS(DebugTests)
	{
	public:
		TEST_METHOD(DebugOptionWritesMatchingPdbAndRemapsCodeViewLines)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3 }, 0x60000020 };
			Bytes types(4, 0);
			put32(types, 0, 4);
			Bytes modifier{ 8, 0, 1, 0x10, 0x74, 0, 0, 0, 0, 0 };
			types.insert(types.end(), modifier.begin(), modifier.end());
			TestSection debugTypes{ ".debug$T", types, 0x42000040 };
			Bytes debugSymbols(4, 0);
			put32(debugSymbols, 0, 4);
			Bytes publicSymbol(4, 0);
			put16(publicSymbol, 2, 0x110e);
			put32(publicSymbol, 4, 0);
			put32(publicSymbol, 8, 0);
			put16(publicSymbol, 12, 1);
			appendString(publicSymbol, "main");
			put16(publicSymbol, 0, static_cast<uint16_t>(publicSymbol.size() - 2));
			put32(debugSymbols, debugSymbols.size(), 0xF1);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(publicSymbol.size()));
			const auto symbolPayload = debugSymbols.size();
			debugSymbols.insert(debugSymbols.end(), publicSymbol.begin(), publicSymbol.end());
			debugSymbols.resize((debugSymbols.size() + 3) & ~size_t(3), 0);
			Bytes stringTable{ 0 };
			appendString(stringTable, "debug.cpp");
			put32(debugSymbols, debugSymbols.size(), 0xF3);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(stringTable.size()));
			debugSymbols.insert(debugSymbols.end(), stringTable.begin(), stringTable.end());
			debugSymbols.resize((debugSymbols.size() + 3) & ~size_t(3), 0);
			Bytes checksums(8, 0);
			put32(checksums, 0, 1);
			put32(debugSymbols, debugSymbols.size(), 0xF4);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(checksums.size()));
			debugSymbols.insert(debugSymbols.end(), checksums.begin(), checksums.end());
			Bytes lines(32, 0);
			put32(lines, 8, 1);
			put32(lines, 16, 1);
			put32(lines, 20, 20);
			put32(lines, 28, 1);
			put32(debugSymbols, debugSymbols.size(), 0xF2);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(lines.size()));
			const auto linePayload = debugSymbols.size();
			debugSymbols.insert(debugSymbols.end(), lines.begin(), lines.end());
			TestSection debugSymbolsSection{ ".debug$S", debugSymbols, 0x42000040 };
			debugSymbolsSection.relocations = { { static_cast<uint32_t>(symbolPayload + 8), 0, 0x000b },{ static_cast<uint32_t>(symbolPayload + 12), 0, 0x000a },{ static_cast<uint32_t>(linePayload), 0, 0x000b },{ static_cast<uint32_t>(linePayload + 4), 0, 0x000a } };
			const auto object = makeObject({ text, debugTypes, debugSymbolsSection }, { TestSymbol{ "main", 0, 1, 2,{} } });
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/DEBUG");
			args.push_back(workspace.file(L"debug.obj").wstring());
			workspace.write(L"debug.obj", object);
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto debugRva = get32(image, optional + 160);
			Assert::IsTrue(debugRva != 0, L"PE debug data directory is present");
			Assert::AreEqual<uint32_t>(28, get32(image, optional + 164), L"PE debug directory contains one RSDS entry");
			const auto directory = rvaOffset(image, debugRva);
			Assert::AreEqual<uint32_t>(2, get32(image, directory + 12), L"debug entry identifies CodeView data");
			const auto recordOffset = get32(image, directory + 24);
			Assert::AreEqual<uint32_t>(0x53445352, get32(image, recordOffset), L"CodeView record has the RSDS signature");
			const auto pdb = workspace.read(L"linked.pdb");
			constexpr array<uint8_t, 24> msfMagic{ 'M','i','c','r','o','s','o','f','t',' ','C','/','C','+','+',' ','M','S','F',' ','7','.','0','0' };
			Assert::IsTrue(pdb.size() > 3 * 4096 && equal(msfMagic.begin(), msfMagic.end(), pdb.begin()), L"/DEBUG writes an MSF 7.0 PDB file");
			const auto pdbInfoStream = 3 * 4096;
			Assert::IsTrue(equal(pdb.begin() + pdbInfoStream + 12, pdb.begin() + pdbInfoStream + 28, image.begin() + recordOffset + 4), L"PDB GUID matches the PE RSDS record");
			Assert::AreEqual<uint32_t>(get32(pdb, pdbInfoStream + 8), get32(image, recordOffset + 20), L"PDB age matches the PE RSDS record");

			// Interoperability: a Windows debugger-engine reader must resolve the public symbol, source file, and remapped line.
			const auto dbghelp = LoadLibraryW(L"C:\\Windows\\System32\\dbghelp.dll");
			Assert::IsNotNull(dbghelp, L"dbghelp.dll is available");
			using SymSetOptionsFn = DWORD(WINAPI*)(DWORD);
			using SymInitializeWFn = BOOL(WINAPI*)(HANDLE, PCWSTR, BOOL);
			using SymLoadModuleExWFn = DWORD64(WINAPI*)(HANDLE, HANDLE, PCWSTR, PCWSTR, DWORD64, DWORD, PVOID, DWORD);
			using SymFromNameFn = BOOL(WINAPI*)(HANDLE, PCSTR, PSYMBOL_INFO);
			using SymGetLineFn = BOOL(WINAPI*)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
			using SymCleanupFn = BOOL(WINAPI*)(HANDLE);
			const auto symSetOptions = reinterpret_cast<SymSetOptionsFn>(GetProcAddress(dbghelp, "SymSetOptions"));
			const auto symInitialize = reinterpret_cast<SymInitializeWFn>(GetProcAddress(dbghelp, "SymInitializeW"));
			const auto symLoadModule = reinterpret_cast<SymLoadModuleExWFn>(GetProcAddress(dbghelp, "SymLoadModuleExW"));
			const auto symFromName = reinterpret_cast<SymFromNameFn>(GetProcAddress(dbghelp, "SymFromName"));
			const auto symGetLine = reinterpret_cast<SymGetLineFn>(GetProcAddress(dbghelp, "SymGetLineFromAddr64"));
			const auto symCleanup = reinterpret_cast<SymCleanupFn>(GetProcAddress(dbghelp, "SymCleanup"));
			Assert::IsTrue(symSetOptions && symInitialize && symLoadModule && symFromName && symGetLine && symCleanup, L"dbghelp exports the symbol APIs");
			const auto process = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(0x4a574c32));
			symSetOptions(0x00000002 | 0x00000010); // SYMOPT_UNDNAME | SYMOPT_LOAD_LINES
			const auto imageBase = get64(image, optional + 24);
			DWORD64 loaded = 0;
			// Freshly written files can be briefly held by a scanner, which makes the first dbghelp load fail intermittently.
			for (int attempt = 0; attempt < 10 && loaded == 0; ++attempt)
			{
				if (attempt != 0) Sleep(100);
				Assert::IsTrue(symInitialize(process, workspace.file(L"").c_str(), FALSE) != FALSE, L"dbghelp initializes");
				loaded = symLoadModule(process, nullptr, workspace.file(L"linked.exe").c_str(), nullptr, imageBase, get32(image, optional + 56), nullptr, 0);
				if (loaded == 0) symCleanup(process);
			}
			Assert::IsTrue(loaded != 0, L"dbghelp loads the linked image and PDB");
			alignas(SYMBOL_INFO) BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 64]{};
			auto* symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = 63;
			const auto found = symFromName(process, "main", symbol) != FALSE;
			IMAGEHLP_LINE64 line{};
			line.SizeOfStruct = sizeof(line);
			DWORD displacement = 0;
			const auto hasLine = found && symGetLine(process, symbol->Address, &displacement, &line) != FALSE;
			const string fileName = hasLine ? string(line.FileName) : string();
			symCleanup(process);
			Assert::IsTrue(found, L"PDB public symbols resolve main");
			Assert::AreEqual<uint64_t>(imageBase + get32(image, sectionHeader(image, ".text") + 12), symbol->Address, L"PDB symbol address is the linked RVA");
			Assert::IsTrue(hasLine, L"PDB line records resolve the entry address");
			Assert::AreEqual<DWORD>(1, line.LineNumber, L"line number is preserved");
			Assert::IsTrue(fileName.find("debug.cpp") != string::npos, L"source file name is preserved");
		}

		TEST_METHOD(RealMsvcCodeViewSourceAndLineMatchMicrosoftLink)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"cl.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; real CodeView comparison skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"source.c", L"int main(void) {\n return 42;\n}\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"cl.exe", { L"/nologo", L"/c", L"/Z7", L"/Od", L"/GS-", L"/Fosource.obj", L"source.c" }));
			assertSuccess(runLinker(workspace, { L"/debug", L"/nodefaultlib", L"/entry:main", L"/out:jw2.exe", L"source.obj" }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/debug", L"/incremental:no", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:msvc.exe", L"source.obj" }));
			const auto dbghelp = LoadLibraryW(L"C:\\Windows\\System32\\dbghelp.dll");
			Assert::IsNotNull(dbghelp);
			using SetOptions = DWORD(WINAPI*)(DWORD);
			using Initialize = BOOL(WINAPI*)(HANDLE, PCWSTR, BOOL);
			using LoadModule = DWORD64(WINAPI*)(HANDLE, HANDLE, PCWSTR, PCWSTR, DWORD64, DWORD, PVOID, DWORD);
			using FromName = BOOL(WINAPI*)(HANDLE, PCSTR, PSYMBOL_INFO);
			using GetLine = BOOL(WINAPI*)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
			using Cleanup = BOOL(WINAPI*)(HANDLE);
			const auto setOptions = reinterpret_cast<SetOptions>(GetProcAddress(dbghelp, "SymSetOptions"));
			const auto initialize = reinterpret_cast<Initialize>(GetProcAddress(dbghelp, "SymInitializeW"));
			const auto loadModule = reinterpret_cast<LoadModule>(GetProcAddress(dbghelp, "SymLoadModuleExW"));
			const auto fromName = reinterpret_cast<FromName>(GetProcAddress(dbghelp, "SymFromName"));
			const auto getLine = reinterpret_cast<GetLine>(GetProcAddress(dbghelp, "SymGetLineFromAddr64"));
			const auto cleanup = reinterpret_cast<Cleanup>(GetProcAddress(dbghelp, "SymCleanup"));
			Assert::IsTrue(setOptions && initialize && loadModule && fromName && getLine && cleanup);
			setOptions(0x00000002 | 0x00000010);
			for (const auto* name : { L"jw2.exe", L"msvc.exe" })
			{
				const auto image = workspace.read(name);
				const auto optional = optionalHeader(image);
				const auto base = get64(image, optional + 24);
				const auto process = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(0x4a574c33));
				DWORD64 loaded = 0;
				for (int attempt = 0; attempt < 10 && loaded == 0; ++attempt)
				{
					if (attempt) Sleep(100);
					Assert::IsTrue(initialize(process, workspace.path.c_str(), FALSE) != FALSE);
					loaded = loadModule(process, nullptr, workspace.file(name).c_str(), nullptr, base, get32(image, optional + 56), nullptr, 0);
					if (!loaded) cleanup(process);
				}
				Assert::IsTrue(loaded != 0, L"dbghelp loads image and PDB");
				alignas(SYMBOL_INFO) BYTE buffer[sizeof(SYMBOL_INFO) + 64]{};
				auto* symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				symbol->MaxNameLen = 63;
				const auto found = fromName(process, "main", symbol) != FALSE;
				IMAGEHLP_LINE64 line{};
				line.SizeOfStruct = sizeof(line);
				DWORD displacement = 0;
				const auto hasLine = found && getLine(process, symbol->Address, &displacement, &line) != FALSE;
				const string filename = hasLine ? line.FileName : "";
				cleanup(process);
				Assert::IsTrue(found && hasLine, L"dbghelp resolves main and its source line");
				Assert::AreEqual<uint64_t>(base + get32(image, optional + 16), symbol->Address, L"public symbol is at the entry point");
				Assert::AreEqual<DWORD>(1, line.LineNumber, L"source line matches compiler input");
				Assert::IsTrue(filename.find("source.c") != string::npos, L"source path is preserved");
			}
			FreeLibrary(dbghelp);
		}

		TEST_METHOD(JwlinkDebugFormatsAndModifiersHaveDistinctArtifacts)
		{
			Workspace workspace;
			workspace.writeText(L"source.asm", L"option casemap:none\n.code\npublic main\nmain proc\n mov eax, 42\n ret\nmain endp\nend\n");
			assertSuccess(runExecutable(workspace, toolPath(L"JWasm.exe"), { L"-win64", L"-Zi", L"-Fosource.obj", L"source.asm" }));
			auto link = [&](const vector<wstring>& debug, const wstring& output)
				{
					vector<wstring> args{ L"format", L"win", L"pe", L"debug" };
					args.insert(args.end(), debug.begin(), debug.end());
					args.insert(args.end(), { L"option", L"quiet,start=main", L"file", L"source.obj", L"name", output });
					assertSuccess(runExecutable(workspace, jwlinkPath(), args));
					return workspace.read(output);
				};
			auto subsections = [&](const Bytes& image)
				{
					const auto optional = optionalHeader(image);
					const auto debugRva = get32(image, optional + 160);
					const auto size = get32(image, optional + 164);
					Assert::IsTrue(debugRva != 0 && size >= 56, L"JWlink CodeView has two PE debug entries");
					const auto entries = rvaOffset(image, debugRva);
					Assert::AreEqual<uint32_t>(4, get32(image, entries + 12), L"first entry is IMAGE_DEBUG_TYPE_MISC");
					Assert::AreEqual<uint32_t>(2, get32(image, entries + 28 + 12), L"second entry is IMAGE_DEBUG_TYPE_CODEVIEW");
					const auto offset = get32(image, entries + 28 + 24);
					const auto length = get32(image, entries + 28 + 16);
					Assert::IsTrue(offset <= image.size() && length <= image.size() - offset && length >= 24, L"NB05 data is within the image");
					Assert::AreEqual<uint32_t>(0x3530424e, get32(image, offset), L"JWlink emits NB05 rather than RSDS");
					const auto directory = static_cast<size_t>(get32(image, offset + 4));
					Assert::IsTrue(directory <= length && 16 <= length - directory, L"NB05 directory header is in bounds");
					const auto headSize = get16(image, offset + directory);
					const auto entrySize = get16(image, offset + directory + 2);
					const auto count = get32(image, offset + directory + 4);
					Assert::IsTrue(headSize >= 16 && entrySize >= 12 && headSize <= length - directory && count <= (length - directory - headSize) / entrySize, L"NB05 directory entries are in bounds");
					vector<uint16_t> kinds;
					for (uint32_t i = 0; i < count; ++i)
					{
						const auto entry = offset + directory + headSize + static_cast<size_t>(i) * entrySize;
						const auto relative = get32(image, entry + 4);
						const auto bytes = get32(image, entry + 8);
						Assert::IsTrue(relative <= length && bytes <= length - relative, L"NB05 subsection is in bounds");
						const auto kind = get16(image, entry);
						const auto start = offset + relative;
						if (kind == 0x123)
						{
							const string publicRecords(image.begin() + start, image.begin() + start + bytes);
							Assert::IsTrue(publicRecords.find("main") != string::npos, L"NB05 public symbol records include main");
						}
						if (kind == 0x127)
						{
							Assert::IsTrue(bytes >= 20, L"NB05 source module header is present");
							const auto file = get32(image, start + 4);
							Assert::IsTrue(file <= bytes && bytes - file >= 17, L"NB05 source file record is in bounds");
							const auto nameLength = image[start + file + 16];
							Assert::IsTrue(nameLength <= bytes - file - 17, L"NB05 source name is in bounds");
							const string source(image.begin() + start + file + 17, image.begin() + start + file + 17 + nameLength);
							Assert::IsTrue(source.find("source.asm") != string::npos, L"NB05 source module names the source file");
							const auto mapping = get32(image, start + file + 4);
							Assert::IsTrue(mapping <= bytes && bytes - mapping >= 4, L"NB05 source line table is in bounds");
							const auto pairs = get16(image, start + mapping + 2);
							Assert::IsTrue(pairs != 0 && pairs <= (bytes - mapping - 4) / 6, L"NB05 source line pairs are in bounds");
							Assert::AreEqual<uint16_t>(4, get16(image, start + mapping + 4 + pairs * 4), L"first JWasm source line is preserved");
						}
						kinds.push_back(kind);
					}
					return kinds;
				};
			const auto all = subsections(link({ L"codeview" }, L"all.exe"));
			const auto lines = subsections(link({ L"codeview", L"lines" }, L"lines.exe"));
			const auto typesLocals = subsections(link({ L"codeview", L"types,locals" }, L"types.exe"));
			auto has = [](const vector<uint16_t>& kinds, uint16_t kind) { return find(kinds.begin(), kinds.end(), kind) != kinds.end(); };
			Assert::IsTrue(has(all, 0x121) && has(all, 0x124) && has(all, 0x127), L"all contains type, local-symbol and source-line subsections");
			Assert::IsTrue(has(lines, 0x127) && !has(lines, 0x121) && !has(lines, 0x124), L"lines omits types and locals");
			Assert::IsTrue(!has(typesLocals, 0x127) && has(typesLocals, 0x121) && has(typesLocals, 0x124), L"types and locals omit source lines");
			for (const auto& mode : { vector<wstring>{ L"dwarf" }, vector<wstring>{ L"watcom" }, vector<wstring>{ L"all" } })
			{
				const auto image = link(mode, L"other.exe");
				Assert::AreEqual<uint32_t>(0, get32(image, optionalHeader(image) + 160), L"non-CodeView output has no PE CodeView debug directory in this fixture");
			}
			assertFailureContains(runLinker(workspace, { L"debug", L"codeview", L"lines", L"file", L"source.obj" }), "unsupported DEBUG modifier 'lines'");
			assertFailureContains(runLinker(workspace, { L"debug", L"codeview", L"types,locals", L"file", L"source.obj" }), "unsupported DEBUG modifier 'types'");
			assertFailureContains(runLinker(workspace, { L"debug", L"locals", L"file", L"source.obj" }), "unsupported DEBUG modifier 'locals'");
			assertFailureContains(runLinker(workspace, { L"debug", L"watcom", L"file", L"source.obj" }), "unsupported DEBUG format 'watcom'");
			assertFailureContains(runLinker(workspace, { L"debug", L"dwarf", L"file", L"source.obj" }), "unsupported DEBUG format 'dwarf'");
			assertFailureContains(runLinker(workspace, { L"/debug", L"/entry:main", L"/out:new.exe", L"source.obj" }), "legacy CodeView .debug$T signature 1");
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:nodebug.exe", L"source.obj" }));
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"nodebug.exe"), {}).exitCode, L"legacy debug input can still be linked without debug output");
			workspace.write(L"plain.obj", makeEntryObject());
			assertSuccess(runLinker(workspace, { L"debug", L"all", L"option", L"start=main", L"file", L"plain.obj", L"name", L"extension.exe" }));
			const auto extension = workspace.read(L"extension.exe");
			const auto directory = rvaOffset(extension, get32(extension, optionalHeader(extension) + 160));
			Assert::AreEqual<uint32_t>(2, get32(extension, directory + 12), L"JWlink2 debug all extension emits CodeView");
			Assert::AreEqual<uint32_t>(0x53445352, get32(extension, get32(extension, directory + 24)), L"JWlink2 emits RSDS rather than JWlink's default DWARF");
			Assert::IsTrue(fs::exists(workspace.file(L"extension.pdb")), L"JWlink2 debug all extension writes a PDB");
			workspace.write(L"legacy-symbols.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".debug$S",{ 1, 0, 0, 0 }, 0x42000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/debug", L"/entry:main", L"legacy-symbols.obj" }), "legacy CodeView .debug$S signature 1");
		}

		TEST_METHOD(DebugOptionAcceptsMsvcTypeAndIdRecordsAndRejectsUnknownLeaves)
		{
			auto typeStream = [](const vector<Bytes>& records)
				{
					Bytes data(4, 0);
					put32(data, 0, 4);
					for (const auto& record : records) data.insert(data.end(), record.begin(), record.end());
					return data;
				};
			auto record = [](uint16_t leaf, const Bytes& body)
				{
					Bytes result(4, 0);
					put16(result, 2, leaf);
					result.insert(result.end(), body.begin(), body.end());
					while (result.size() % 4 != 0) result.push_back(static_cast<uint8_t>(0xF0 + 4 - result.size() % 4));
					put16(result, 0, static_cast<uint16_t>(result.size() - 2));
					return result;
				};
			auto link = [&](Workspace& workspace, const Bytes& types, const wstring& pdbName)
				{
					const auto object = makeObject({ TestSection{ ".text",{ 0xc3 }, 0x60000020 }, TestSection{ ".debug$T", types, 0x42000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
					workspace.write(L"types.obj", object);
					auto args = outputArgs(workspace);
					args.insert(args.begin() + 1, L"/PDB:" + workspace.file(pdbName).wstring());
					args.push_back(workspace.file(L"types.obj").wstring());
					return runLinker(workspace, args);
				};

			Bytes field;
			put16(field, field.size(), 0x150D); put16(field, field.size(), 3); put32(field, field.size(), 0x74); put16(field, field.size(), 0); appendString(field, "x");
			Bytes structure;
			put16(structure, structure.size(), 1); put16(structure, structure.size(), 0); put32(structure, structure.size(), 0x1000); put32(structure, structure.size(), 0); put32(structure, structure.size(), 0);
			put16(structure, structure.size(), 4); appendString(structure, "S");
			Bytes stringId;
			put32(stringId, stringId.size(), 0); appendString(stringId, "a.c");
			Bytes substrings;
			put32(substrings, substrings.size(), 1); put32(substrings, substrings.size(), 0x1002);
			Bytes buildInfo;
			put16(buildInfo, buildInfo.size(), 1); put32(buildInfo, buildInfo.size(), 0x1002);
			Workspace accepted;
			assertSuccess(link(accepted, typeStream({ record(0x1203, field), record(0x1505, structure), record(0x1605, stringId), record(0x1604, substrings), record(0x1603, buildInfo) }), L"types.pdb"));
			Assert::IsTrue(fs::exists(accepted.file(L"types.pdb")), L"/PDB writes the requested PDB path");

			Workspace rejected;
			assertFailureContains(link(rejected, typeStream({ record(0x1700, Bytes(4, 0)) }), L"bad.pdb"), "unsupported CodeView type leaf");
		}

	};
}
