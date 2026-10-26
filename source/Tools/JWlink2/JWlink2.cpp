#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

enum class TokenType
{
    Directive,
    Argument,
    ListElement,
    Delimiter
};

struct LinkerParam
{
    std::string value;
    TokenType type;
};

// Represents the deep parsed structural configuration:
// Primary Directive -> List of clean positional Arguments/Sub-options
using LinkerConfig = std::map<std::string, std::vector<std::string>>;

// Represents specific broken down assignment maps (e.g., {"stack": "0x4000"})
using SubOptionMap = std::map<std::string, std::string>;


// 1. HANDLER: Indirect response files (@file) with comment stripping
std::string ExpandResponseFiles(const std::string& inputStr) {
    std::stringstream source(inputStr);
    std::string word;
    std::string accumulatedCommand;

    while (source >> word)
    {
        // Look for the jwlink/wlink indirect reference indicator '@'
        if (!word.empty() && word[0] == '@')
        {
            std::string filePath = word.substr(1);
            // Strip wrap-around quotes if the user gave @'my options.lnk'
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
                // Strip hash comments or inline comment patterns if applicable
                size_t commentPos = line.find('#');
                if (commentPos != std::string::npos)
                {
                    line = line.substr(0, commentPos);
                }
                fileContent += line + " ";
            }

            // Recursively evaluate in case the response file references another response file
            accumulatedCommand += ExpandResponseFiles(fileContent) + " ";
        }
        else
        {
            accumulatedCommand += word + " ";
        }
    }
    return accumulatedCommand;
}


// 2. PARSER: Tokenizer identifying main directives and tokens
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
            continue; // Drop the literal quote markers
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
            { // Isolate list comma items but keep '=' for sub-option parsing
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

    // Directives marking pass
    for (size_t i = 0; i < processedParams.size(); ++i)
    {
        std::string val = processedParams[i].value;
        if (val == "format" || val == "file" || val == "option" || val == "op" || val == "lib" || val == "name")
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


// 3. EXTRACTOR: Groups components into standard structure mapping
LinkerConfig GroupDirectives(const std::vector<LinkerParam>& tokens) {
    LinkerConfig config;
    std::string currentDirective = "global";

    for (const auto& token : tokens)
    {
        if (token.type == TokenType::Delimiter) continue;

        if (token.type == TokenType::Directive)
        {
            currentDirective = token.value;
            if (currentDirective == "op") currentDirective = "option"; // Standardize shorthand

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


// 4. SUB-OPTION PARSER: Breaks down strings matching "key=value" pairs 
SubOptionMap ParseSubOptions(const std::vector<std::string>& optionArguments) {
    SubOptionMap subMap;
    for (const auto& arg : optionArguments)
    {
        size_t equalSignPos = arg.find('=');
        if (equalSignPos != std::string::npos)
        {
            std::string key = arg.substr(0, equalSignPos);
            std::string value = arg.substr(equalSignPos + 1);

            // Normalize casing to match jwlink's case-insensitive strategy
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            subMap[key] = value;
        }
        else
        {
            // Standalone flags without explicit assignments (e.g. "eliminate" or "map")
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
    std::ofstream mockFile("linkcmds.lnk");
    if (mockFile.is_open())
    {
        mockFile << "file main.obj, utils.obj\n";
        mockFile << "op stack=0x4000, eliminate, map\n"; // Option containing values and flags
        mockFile.close();
    }

    // Input command string referencing our response script using '@'
    std::string rawCommandLine = "format win pe op stack=0x4000 option map=%CD%\\bin\\windows\\64bit\\test1.map file %CD%\\bin\\windows\\64bit\\test1.obj, %CD%\\bin\\windows\\64bit\\c064.obj lib %LIB64%,%KLIB64%,%LLIB64%  name %CD%\\bin\\windows\\64bit\\test1.exe";
    std::cout << "Original CLI entry: \n  " << rawCommandLine << "\n\n";

    // Unpack response contents recursively 
    std::string fullyExpandedString = ExpandResponseFiles(rawCommandLine);
    std::cout << "Fully expanded macro command string: \n  " << fullyExpandedString << "\n\n";

    // Process parameters through our multi-phase layout
    std::vector<LinkerParam> tokens = TokenizeJwlinkString(fullyExpandedString);
    LinkerConfig configMap = GroupDirectives(tokens);

    // Output grouped mapping entries
    std::cout << "Structured Group Output:\n";
    std::cout << "========================================\n";
    for (const auto& [directive, arguments] : configMap)
    {
        std::cout << "Directive Block: [" << directive << "]\n";

        // If we are looking at an 'option' block, apply the SubOption parsing pass
        if (directive == "option")
        {
            SubOptionMap innerOptions = ParseSubOptions(arguments);
            std::cout << "  -> Extracted Inner Options (Key/Value pairs):\n";
            for (const auto& [key, value] : innerOptions)
            {
                std::cout << "     * " << key << " = " << value << "\n";
            }
        }
        else
        {
            // For standard lists like 'file' elements
            for (const auto& arg : arguments)
            {
                std::cout << "  -> Value item: " << arg << "\n";
            }
        }
        std::cout << "----------------------------------------\n";
    }

    // Clean up mock file
    std::remove("linkcmds.lnk");
    return 0;
}

