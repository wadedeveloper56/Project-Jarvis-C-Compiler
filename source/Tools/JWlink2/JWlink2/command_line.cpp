#include "pch.h"
#include "command_line.h"

using namespace std;

namespace jwlink2
{
	namespace
	{
		[[noreturn]] void fail(const string& message) {
			throw runtime_error(message);
		}

		string lowerAscii(string value) {
			transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
			return value;
		}

		vector<string> tokenizeResponse(string_view text, const fs::path& path) {
			vector<string> result;
			string token;
			bool quoted = false;
			bool started = false;
			for (size_t i = 0; i < text.size();)
			{
				if (text[i] == '\\')
				{
					size_t end = i;
					while (end < text.size() && text[end] == '\\')
					{
						++end;
					}
					const auto slashCount = end - i;
					if (end < text.size() && text[end] == '"')
					{
						token.append(slashCount / 2, '\\');
						if ((slashCount & 1) != 0)
						{
							token.push_back('"');
						}
						else
						{
							quoted = !quoted;
						}
						started = true;
						i = end + 1;
					}
					else
					{
						token.append(slashCount, '\\');
						started = true;
						i = end;
					}
				}
				else if (text[i] == '"')
				{
					quoted = !quoted;
					started = true;
					++i;
				}
				else if (isspace(static_cast<unsigned char>(text[i])) && !quoted)
				{
					if (started)
					{
						result.push_back(move(token));
						token.clear();
						started = false;
					}
					++i;
				}
				else
				{
					token.push_back(text[i++]);
					started = true;
				}
			}
			if (quoted)
			{
				fail(path.string() + ": unterminated quote in response file");
			}
			if (started)
			{
				result.push_back(move(token));
			}
			return result;
		}

		void expandResponseFiles(const vector<string>& arguments, vector<string>& expanded, set<fs::path>& active, unsigned depth) {
			if (depth > 32)
			{
				fail("response-file nesting exceeds 32 levels");
			}
			for (const auto& argument : arguments)
			{
				if (argument.size() <= 1 || argument[0] != '@')
				{
					expanded.push_back(argument);
					continue;
				}
				const auto path = fs::absolute(fs::path(argument.substr(1))).lexically_normal();
				if (!active.insert(path).second)
				{
					fail("response-file cycle involving '" + path.string() + "'");
				}
				ifstream input(path);
				if (!input)
				{
					fail("cannot open response file '" + path.string() + "'");
				}
				ostringstream content;
				content << input.rdbuf();
				if (!input.eof() && input.fail())
				{
					fail("failed reading response file '" + path.string() + "'");
				}
				auto nested = tokenizeResponse(content.str(), path);
				expandResponseFiles(nested, expanded, active, depth + 1);
				active.erase(path);
			}
		}

		uint64_t parseUnsignedOption(string_view text, string_view optionName) {
			int base = 10;
			if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
			{
				base = 16;
				text.remove_prefix(2);
			}
			uint64_t value = 0;
			const auto parsed = from_chars(text.data(), text.data() + text.size(), value, base);
			if (text.empty() || parsed.ec != errc{} || parsed.ptr != text.data() + text.size())
			{
				fail("invalid value for " + string(optionName));
			}
			return value;
		}

		void printUsage() {
			cout << "JWlink2 - AMD64 COFF to Windows PE32+ linker\n"
				"Usage: JWlink2 [options] input.obj [input.lib ...]\n"
				"  /OUT:<file>, -o <file>      Output image (default: a.exe or a.dll)\n"
				"  /DLL                        Create a DLL\n"
				"  /DEF:<file>                 Read module name and exports from a .def file\n"
				"  /EXPORT:<spec>              Export a symbol\n"
				"  /IMPLIB[:<file>]            Write a DLL import library\n"
				"  /ENTRY:<symbol>             Set the image entry point\n"
				"  /NOENTRY                    Omit the DLL entry point\n"
				"  /SUBSYSTEM:CONSOLE|WINDOWS  Select PE subsystem\n"
				"  /LIBPATH:<directory>        Search directory for libraries\n"
				"  /DEFAULTLIB:<library>       Add a library to the link\n"
				"  /DELAYLOAD:<dll>            Delay-load imports from a DLL\n"
				"  /DEBUG                      Generate a PDB and PE CodeView record\n"
				"  /PDB:<file>                 Set PDB output path (implies /DEBUG)\n"
				"  /ALIGN:<bytes>              Set PE section alignment\n"
				"  /BASE:<address>             Set preferred image base\n"
				"  /DYNAMICBASE | /FIXED       Enable ASLR relocations or fixed image\n"
				"  /MAP[:<file>]               Write a symbol map\n"
				"  @<file>                     Read additional options from a response file\n"
				"  -h, --help                  Show this help\n";
		}

	} // namespace

	ExportDirective parseExportDirective(string_view value, const string& context) {
		ExportDirective directive;
		const auto firstComma = value.find(',');
		const auto symbol = value.substr(0, firstComma);
		const auto equals = symbol.find('=');
		directive.name = string(symbol.substr(0, equals));
		directive.internalName = equals == string_view::npos ? directive.name : string(symbol.substr(equals + 1));
		if (directive.name.empty() || directive.internalName.empty())
		{
			fail(context + ": /EXPORT requires a public and internal symbol name");
		}
		bool ordinalSpecified = false;
		size_t cursor = firstComma;
		while (cursor != string_view::npos)
		{
			const auto next = value.find(',', cursor + 1);
			const auto attribute = value.substr(cursor + 1, next == string_view::npos ? string_view::npos : next - cursor - 1);
			const auto normalized = lowerAscii(string(attribute));
			if (normalized == "noname")
			{
				directive.noname = true;
			}
			else if (normalized == "data")
			{
				directive.data = true;
			}
			else if (normalized == "private")
			{
				directive.privateExport = true;
			}
			else if (!attribute.empty() && attribute.front() == '@')
			{
				if (ordinalSpecified)
				{
					fail(context + ": /EXPORT specifies more than one ordinal");
				}
				ordinalSpecified = true;
				const auto ordinal = parseUnsignedOption(attribute.substr(1), "/EXPORT ordinal");
				if (ordinal == 0 || ordinal > numeric_limits<uint16_t>::max())
				{
					fail(context + ": /EXPORT ordinal must be between 1 and 65535");
				}
				directive.ordinal = static_cast<uint16_t>(ordinal);
			}
			else
			{
				fail(context + ": unsupported /EXPORT attribute '" + string(attribute) + "'");
			}
			cursor = next;
		}
		if (directive.noname && !directive.ordinal)
		{
			fail(context + ": /EXPORT NONAME requires an ordinal");
		}
		return directive;
	}

	LinkOptions parseArguments(int argc, char** argv) {
		LinkOptions options;
		vector<string> supplied;
		for (int i = 1; i < argc; ++i)
		{
			supplied.emplace_back(argv[i]);
		}
		vector<string> arguments;
		set<fs::path> activeResponses;
		expandResponseFiles(supplied, arguments, activeResponses, 0);
		vector<string> excludedLibraries;
		bool mapUsesDefaultName = false;
		bool parseOptions = true;
		auto nextValue = [&](size_t& index, string_view option) -> string
			{
				if (++index >= arguments.size())
				{
					fail(string(option) + " requires a value");
				}
				return arguments[index];
			};
		for (size_t i = 0; i < arguments.size(); ++i)
		{
			const auto& argument = arguments[i];
			if (parseOptions && argument == "--")
			{
				parseOptions = false;
				continue;
			}
			if (!parseOptions || argument.empty() || (argument[0] != '-' && argument[0] != '/'))
			{
				options.inputs.emplace_back(argument);
				continue;
			}
			size_t prefixLength = argument[0] == '/' ? 1 : (argument.size() > 1 && argument[1] == '-' ? 2 : 1);
			const bool attachedOutputOption = argument.size() > 2 && argument[0] == '-' && prefixLength == 1 && (argument[1] == 'o' || argument[1] == 'O') && argument[2] != ':' && argument[2] != '=' && tolower(static_cast<unsigned char>(argument[2])) != 'u';
			const bool attachedEntryOption = argument.size() > 2 && argument[0] == '-' && prefixLength == 1 && (argument[1] == 'e' || argument[1] == 'E') && argument[2] != ':' && argument[2] != '=' && tolower(static_cast<unsigned char>(argument[2])) != 'n';
			const bool attachedPathOption = argument.size() > 2 && argument[0] == '-' && argument[1] == 'L' && prefixLength == 1 && argument[2] != ':' && argument[2] != '=';
			const bool shortPathOption = argument.size() > 1 && argument[0] == '-' && argument[1] == 'L' && prefixLength == 1;
			const auto delimiter = argument.find_first_of(":=", prefixLength);
			const string optionName = attachedOutputOption ? "o" : attachedEntryOption ? "e" : attachedPathOption ? "libpath" : lowerAscii(argument.substr(prefixLength, delimiter == string::npos ? string::npos : delimiter - prefixLength));
			optional<string> value;
			if (attachedOutputOption || attachedEntryOption || attachedPathOption)
			{
				value = argument.substr(prefixLength + 1);
			}
			else if (delimiter != string::npos)
			{
				value = argument.substr(delimiter + 1);
			}
			auto requireValue = [&](string_view spelling)
				{
					return value ? *value : nextValue(i, spelling);
				};
			if (optionName == "h" || optionName == "help" || optionName == "?")
			{
				printUsage();
				exit(0);
			}
			else if (optionName == "o" || optionName == "out")
			{
				options.output = requireValue("/OUT");
				options.outputSpecified = true;
			}
			else if (optionName == "dll")
			{
				if (value) fail("/DLL does not accept a value");
				options.dll = true;
			}
			else if (optionName == "def")
			{
				options.definitionFile = requireValue("/DEF");
			}
			else if (optionName == "export")
			{
				options.exports.push_back(parseExportDirective(requireValue("/EXPORT"), "/EXPORT"));
			}
			else if (optionName == "implib")
			{
				options.importLibrarySpecified = true;
				if (value && !value->empty()) options.importLibrary = fs::path(*value);
			}
			else if (optionName == "noentry")
			{
				if (value) fail("/NOENTRY does not accept a value");
				options.noEntry = true;
			}
			else if (optionName == "e" || optionName == "entry")
			{
				options.entry = requireValue("/ENTRY");
				options.entrySpecified = true;
			}
			else if (optionName == "subsystem")
			{
				auto subsystem = lowerAscii(requireValue("/SUBSYSTEM"));
				const auto comma = subsystem.find(',');
				if (comma != string::npos)
				{
					const auto version = subsystem.substr(comma + 1);
					const auto dot = version.find('.');
					if (dot == string::npos || dot == 0 || dot + 1 == version.size())
					{
						fail("invalid /SUBSYSTEM version; expected major.minor");
					}
					auto parseVersionPart = [&](string_view part)
						{
							unsigned int parsedValue = 0;
							const auto parsed = from_chars(part.data(), part.data() + part.size(), parsedValue);
							if (parsed.ec != errc{} || parsed.ptr != part.data() + part.size() || parsedValue > numeric_limits<uint16_t>::max())
							{
								fail("invalid /SUBSYSTEM version; expected major.minor");
							}
							return static_cast<uint16_t>(parsedValue);
						};
					options.subsystemVersionMajor = parseVersionPart(string_view(version).substr(0, dot));
					options.subsystemVersionMinor = parseVersionPart(string_view(version).substr(dot + 1));
					subsystem.resize(comma);
				}
				if (subsystem == "console" || subsystem == "3")
				{
					options.subsystem = 3;
				}
				else if (subsystem == "windows" || subsystem == "2")
				{
					options.subsystem = 2;
				}
				else
				{
					fail("unsupported subsystem '" + subsystem + "' (choose console or windows)");
				}
			}
			else if (optionName == "libpath" || shortPathOption)
			{
				const auto paths = requireValue("/LIBPATH");
				size_t start = 0;
				while (start <= paths.size())
				{
					const auto end = paths.find(';', start);
					const auto part = paths.substr(start, end == string::npos ? string::npos : end - start);
					if (!part.empty())
					{
						options.libraryPaths.emplace_back(part);
					}
					if (end == string::npos)
					{
						break;
					}
					start = end + 1;
				}
			}
			else if (optionName == "defaultlib" || (optionName.size() > 1 && optionName[0] == 'l'))
			{
				const auto library = optionName == "defaultlib" ? requireValue("/DEFAULTLIB") : optionName.substr(1);
				if (library.empty())
				{
					fail("/DEFAULTLIB requires a library name");
				}
				options.defaultLibraries.push_back(library);
			}
			else if (optionName == "delayload")
			{
				if (!value || value->empty())
				{
					fail("/DELAYLOAD requires an attached DLL name, for example /DELAYLOAD:plugin.dll");
				}
				options.delayLoadLibraries.push_back(*value);
			}
			else if (optionName == "debug")
			{
				if (value) fail("/DEBUG does not accept a value");
				options.debug = true;
			}
			else if (optionName == "pdb")
			{
				options.pdbFile = fs::path(requireValue("/PDB"));
				if (options.pdbFile->empty()) fail("/PDB requires an output path");
				options.debug = true;
			}
			else if (optionName == "align")
			{
				const auto alignment = parseUnsignedOption(requireValue("/ALIGN"), "/ALIGN");
				if (alignment < 0x200 || alignment > numeric_limits<uint32_t>::max() || (alignment & (alignment - 1)) != 0)
				{
					fail("/ALIGN must be a power of two between 512 and 0x80000000");
				}
				options.sectionAlignment = static_cast<uint32_t>(alignment);
				options.fileAlignment = alignment < 0x1000 ? static_cast<uint32_t>(alignment) : 0x200;
				options.sectionAlignmentSpecified = true;
			}
			else if (optionName == "base")
			{
				options.imageBase = parseUnsignedOption(requireValue("/BASE"), "/BASE");
				if (options.imageBase == 0 || (options.imageBase & 0xffff) != 0)
				{
					fail("/BASE must be a nonzero 64-KiB-aligned address");
				}
			}
			else if (optionName == "dynamicbase")
			{
				if (value && lowerAscii(*value) != "yes" && lowerAscii(*value) != "no")
				{
					fail("/DYNAMICBASE accepts only YES or NO");
				}
				options.dynamicBase = !value || lowerAscii(*value) == "yes";
			}
			else if (optionName == "fixed")
			{
				if (value && lowerAscii(*value) != "yes" && lowerAscii(*value) != "no")
				{
					fail("/FIXED accepts only YES or NO");
				}
				options.dynamicBase = value && lowerAscii(*value) == "no";
			}
			else if (optionName == "nodefaultlib")
			{
				if (value)
				{
					auto excluded = lowerAscii(fs::path(*value).filename().string());
					if (fs::path(excluded).extension().empty())
					{
						excluded += ".lib";
					}
					excludedLibraries.push_back(excluded);
					options.excludedDefaultLibraries.push_back(move(excluded));
				}
				else
				{
					options.noDefaultLibraries = true;
				}
			}
			else if (optionName == "map")
			{
				if (value && !value->empty())
				{
					options.mapFile = fs::path(*value);
				}
				else
				{
					mapUsesDefaultName = true;
				}
			}
			else if (optionName == "machine")
			{
				const auto machine = lowerAscii(requireValue("/MACHINE"));
				if (machine != "x64" && machine != "amd64")
				{
					fail("JWlink2 only supports /MACHINE:X64");
				}
			}
			else if (optionName == "nologo")
			{
				options.noLogo = true;
			}
			else if (optionName == "nxcompat")
			{
				if (value && lowerAscii(*value) != "yes" && lowerAscii(*value) != "no")
				{
					fail("/NXCOMPAT accepts only YES or NO");
				}
				options.nxCompat = !value || lowerAscii(*value) == "yes";
			}
			else if (optionName == "incremental")
			{
				if (!value || lowerAscii(*value) != "no")
				{
					fail("incremental linking is not supported; use /INCREMENTAL:NO");
				}
			}
			else
			{
				fail("unsupported option '" + argument + "'");
			}
		}
		if (options.noEntry && (!options.dll || options.entrySpecified))
		{
			fail("/NOENTRY is valid only for a DLL without /ENTRY");
		}
		if (options.importLibrarySpecified && !options.dll)
		{
			fail("/IMPLIB requires /DLL");
		}
		if (options.dll && !options.outputSpecified)
		{
			options.output.replace_extension(".dll");
		}
		if (options.debug && !options.pdbFile)
		{
			auto pdbPath = options.output;
			pdbPath.replace_extension(".pdb");
			options.pdbFile = move(pdbPath);
		}
		if (options.dll && !options.importLibrary)
		{
			auto libraryPath = options.output;
			libraryPath.replace_extension(".lib");
			options.importLibrary = move(libraryPath);
		}
		if (options.noDefaultLibraries)
		{
			options.defaultLibraries.clear();
		}
		else if (!excludedLibraries.empty())
		{
			erase_if(options.defaultLibraries, [&](const string& library)
				{
					auto normalized = lowerAscii(fs::path(library).filename().string());
					if (fs::path(normalized).extension().empty())
					{
						normalized += ".lib";
					}
					return find(excludedLibraries.begin(), excludedLibraries.end(), normalized) != excludedLibraries.end();
				});
		}
		if (mapUsesDefaultName && !options.mapFile)
		{
			auto mapPath = options.output;
			mapPath.replace_extension(".map");
			options.mapFile = move(mapPath);
		}
		if (options.inputs.empty() && options.defaultLibraries.empty())
		{
			fail("no input object files or libraries were supplied; use --help for usage");
		}
		return options;
	}
} // namespace jwlink2
