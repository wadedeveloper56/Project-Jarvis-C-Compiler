#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

namespace jwlink2
{
	namespace fs = filesystem;

	struct ExportDirective
	{
		string name;
		string internalName;
		optional<uint16_t> ordinal;
		bool noname = false;
		bool data = false;
		bool privateExport = false;
	};

	ExportDirective parseExportDirective(string_view value, const string& context);

	struct LinkOptions
	{
		fs::path output = "a.exe";
		optional<string> entry;
		optional<fs::path> mapFile;
		optional<fs::path> pdbFile;
		optional<fs::path> definitionFile;
		optional<fs::path> importLibrary;
		string moduleName;
		vector<fs::path> libraryPaths;
		uint64_t imageBase = 0x140000000ULL;
		uint16_t subsystem = 3;
		uint16_t subsystemVersionMajor = 6;
		uint16_t subsystemVersionMinor = 0;
		bool dynamicBase = true;
		bool nxCompat = true;
		bool noLogo = false;
		bool debug = false;
		bool noDefaultLibraries = false;
		vector<string> defaultLibraries;
		vector<string> excludedDefaultLibraries;
		vector<string> delayLoadLibraries;
		vector<string> includeSymbols;
		vector<pair<string, uint32_t>> commonAlignments;
		vector<ExportDirective> exports;
		uint32_t sectionAlignment = 0x1000;
		uint32_t fileAlignment = 0x200;
		uint32_t commonAlignment = 8;
		bool entrySpecified = false;
		bool sectionAlignmentSpecified = false;
		bool outputSpecified = false;
		bool dll = false;
		bool noEntry = false;
		bool importLibrarySpecified = false;
		vector<fs::path> inputs;
	};

	LinkOptions parseArguments(int argc, char** argv);
	void runLinker(LinkOptions options);
}
