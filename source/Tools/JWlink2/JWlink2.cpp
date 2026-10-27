#include "pch.h"

using namespace std;

enum class TokenType
{
    Directive,
    Argument,
    ListElement,
    Delimiter
};

struct LinkerParam
{
    string value;
    TokenType type;
};

using LinkerConfig = std::map<std::string, std::vector<std::string>>;
using SubOptionMap = std::map<std::string, std::string>;

// ============================================================================
// VALIDATION ENGINE
// ============================================================================
namespace Validation
{
    // Valid top-level jwlink directives (normalized to lowercase)
    const std::set<std::string> ValidDirectives = {
        "format", "file", "option", "op", "name", "library", "lib",
        "path", "output", "debug", "import", "export", "system", "sys"
    };

    // Valid sub-options allowed inside an 'option' directive block
    const std::set<std::string> ValidSubOptions = {
        "stack", "offset", "eliminate", "map", "quiet", "verbose",
        "caseexact", "cvpack", "nodefaultlibs", "start", "align"
    };

    // Valid platforms/subsystems allowed inside a 'format' directive block
    const std::set<std::string> ValidFormats = {
        "windows", "win", "pe", "os2", "dos", "elf", "raw", "novell", "qnx"
    };

    // Validates whether a top-level keyword is recognized
    bool IsValidDirective(const std::string& directive) {
        std::string lowerDir = directive;
        std::transform(lowerDir.begin(), lowerDir.end(), lowerDir.begin(), ::tolower);
        return ValidDirectives.find(lowerDir) != ValidDirectives.end();
    }

    // Validates sub-options and logs warnings for unknown elements
    void ValidateConfig(const LinkerConfig& config) {
        std::cout << "\n[Validation Report]\n";
        std::cout << "----------------------------------------\n";
        bool hasErrors = false;

        for (const auto& [directive, arguments] : config)
        {
            // 1. Check top-level directives
            if (directive != "global" && !IsValidDirective(directive))
            {
                std::cerr << " -> ERROR: Unknown top-level directive structural keyword: '" << directive << "'\n";
                hasErrors = true;
                continue;
            }

            // 2. Specialized internal checking for the 'option' block
            if (directive == "option")
            {
                for (const auto& arg : arguments)
                {
                    // Isolate key from potential key=value formats
                    size_t eqPos = arg.find('=');
                    std::string key = (eqPos != std::string::npos) ? arg.substr(0, eqPos) : arg;
                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

                    if (ValidSubOptions.find(key) == ValidSubOptions.end())
                    {
                        std::cerr << " -> WARNING: Unrecognized sub-option or flag inside 'option': '" << key << "'\n";
                        hasErrors = true;
                    }
                }
            }

            // 3. Specialized internal checking for the 'format' block
            if (directive == "format")
            {
                for (const auto& arg : arguments)
                {
                    std::string lowerArg = arg;
                    std::transform(lowerArg.begin(), lowerArg.end(), lowerArg.begin(), ::tolower);
                    if (ValidFormats.find(lowerArg) == ValidFormats.end())
                    {
                        std::cerr << " -> WARNING: Unknown or unsupported target system platform format: '" << arg << "'\n";
                        hasErrors = true;
                    }
                }
            }
        }

        if (!hasErrors)
        {
            std::cout << " -> Linker properties validated successfully. Zero anomalies found.\n";
        }
        std::cout << "----------------------------------------\n\n";
    }
}

// ============================================================================
// PARSING CORE PIPELINE
// ============================================================================

std::string ExpandResponseFiles(const std::string& inputStr) {
    std::stringstream source(inputStr);
    std::string word;
    std::string accumulatedCommand;

    while (source >> word)
    {
        if (!word.empty() && word[0] == '@')
        {
            std::string filePath = word.substr(1);
            if (filePath.front() == '\'' && filePath.back() == '\'')
            {
                filePath = filePath.substr(1, filePath.length() - 2);
            }

            std::ifstream file(filePath);
            if (!file.is_open())
            {
                std::cerr << "Warning: Could not open indirect file: " << filePath << "\n";
                continue;
            }

            std::string line;
            std::string fileContent;
            while (std::getline(file, line))
            {
                size_t commentPos = line.find('#');
                if (commentPos != std::string::npos)
                {
                    line = line.substr(0, commentPos);
                }
                fileContent += line + " ";
            }
            accumulatedCommand += ExpandResponseFiles(fileContent) + " ";
        }
        else
        {
            accumulatedCommand += word + " ";
        }
    }
    return accumulatedCommand;
}

std::vector<LinkerParam> TokenizeJwlinkString(const std::string& cmdLine) {
    std::vector<LinkerParam> processedParams;
    std::string currentToken;
    bool insideQuotes = false;

    for (size_t i = 0; i < cmdLine.length(); ++i)
    {
        char ch = cmdLine[i];
        if (ch == '\'')
        {
            insideQuotes = !insideQuotes;
            continue;
        }

        if (insideQuotes)
        {
            currentToken += ch;
        }
        else
        {
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            {
                if (!currentToken.empty())
                {
                    processedParams.push_back({ currentToken, TokenType::Argument });
                    currentToken.clear();
                }
            }
            else if (ch == ',')
            {
                if (!currentToken.empty())
                {
                    processedParams.push_back({ currentToken, TokenType::ListElement });
                    currentToken.clear();
                }
                processedParams.push_back({ std::string(1, ch), TokenType::Delimiter });
            }
            else
            {
                currentToken += ch;
            }
        }
    }
    if (!currentToken.empty())
    {
        processedParams.push_back({ currentToken, TokenType::Argument });
    }

    // Set directive tokens safely, deferring validation handling for downstream metrics reporting
    for (size_t i = 0; i < processedParams.size(); ++i)
    {
        std::string val = processedParams[i].value;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);

        if (Validation::ValidDirectives.find(val) != Validation::ValidDirectives.end())
        {
            processedParams[i].type = TokenType::Directive;
        }
        else if (i == 0 && processedParams[i].type == TokenType::Argument)
        {
            processedParams[i].type = TokenType::Directive;
        }
    }
    return processedParams;
}

LinkerConfig GroupDirectives(const std::vector<LinkerParam>& tokens) {
    LinkerConfig config;
    std::string currentDirective = "global";

    for (const auto& token : tokens)
    {
        if (token.type == TokenType::Delimiter) continue;

        if (token.type == TokenType::Directive)
        {
            currentDirective = token.value;
            std::transform(currentDirective.begin(), currentDirective.end(), currentDirective.begin(), ::tolower);
            if (currentDirective == "op") currentDirective = "option";

            if (config.find(currentDirective) == config.end())
            {
                config[currentDirective] = std::vector<std::string>();
            }
        }
        else
        {
            config[currentDirective].push_back(token.value);
        }
    }
    return config;
}

SubOptionMap ParseSubOptions(const std::vector<std::string>& optionArguments) {
    SubOptionMap subMap;
    for (const auto& arg : optionArguments)
    {
        size_t equalSignPos = arg.find('=');
        if (equalSignPos != std::string::npos)
        {
            std::string key = arg.substr(0, equalSignPos);
            std::string value = arg.substr(equalSignPos + 1);
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            subMap[key] = value;
        }
        else
        {
            std::string flag = arg;
            std::transform(flag.begin(), flag.end(), flag.begin(), ::tolower);
            subMap[flag] = "true";
        }
    }
    return subMap;
}

// --- Execution Demonstration ---
int main() {
    // 1. Create a mock external response file on disk for demonstration purposes
    ofstream mockFile("linkcmds.lnk");
    if (mockFile.is_open())
    {
        mockFile << "file main.obj, utils.obj\n";
        mockFile << "op stack=0x4000, eliminate, map\n"; // Option containing values and flags
        mockFile.close();
    }

    // Input command string referencing our response script using '@'
    string rawCommandLine = "format win pe op stack=0x4000 option map=%CD%\\bin\\windows\\64bit\\test1.map file %CD%\\bin\\windows\\64bit\\test1.obj, %CD%\\bin\\windows\\64bit\\c064.obj lib %LIB64%,%KLIB64%,%LLIB64%  name %CD%\\bin\\windows\\64bit\\test1.exe";
    cout << "Original CLI entry: \n  " << rawCommandLine << "\n\n";
    string fullyExpandedString = ExpandResponseFiles(rawCommandLine);
    cout << "Fully expanded macro command string: \n  " << fullyExpandedString << "\n\n";
    vector<LinkerParam> tokens = TokenizeJwlinkString(fullyExpandedString);
    LinkerConfig configMap = GroupDirectives(tokens);
    Validation::ValidateConfig(configMap);

    cout << "Structured Group Output:\n";
    cout << "========================================\n";
    for (const auto& [directive, arguments] : configMap)
    {
        cout << "Directive Block: [" << directive << "]\n";

        // If we are looking at an 'option' block, apply the SubOption parsing pass
        if (directive == "option")
        {
            SubOptionMap innerOptions = ParseSubOptions(arguments);
            cout << "  -> Extracted Inner Options (Key/Value pairs):\n";
            for (const auto& [key, value] : innerOptions)
            {
                cout << "     * " << key << " = " << value << "\n";
            }
        }
        else
        {
            // For standard lists like 'file' elements
            for (const auto& arg : arguments)
            {
                cout << "  -> Value item: " << arg << "\n";
            }
        }
        cout << "----------------------------------------\n";
    }

    // Clean up mock file
    remove("linkcmds.lnk");
    return 0;
}

