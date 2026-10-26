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

// Represents the deep parsed structural configuration:
// Primary Directive -> List of clean positional Arguments/Sub-options
using LinkerConfig = map<string, vector<string>>;

// Represents specific broken down assignment maps (e.g., {"stack": "0x4000"})
using SubOptionMap = map<string, string>;


// 1. HANDLER: Indirect response files (@file) with comment stripping
string ExpandResponseFiles(const string& inputStr) {
    stringstream source(inputStr);
    string word;
    string accumulatedCommand;

    while (source >> word)
    {
        // Look for the jwlink/wlink indirect reference indicator '@'
        if (!word.empty() && word[0] == '@')
        {
            string filePath = word.substr(1);
            // Strip wrap-around quotes if the user gave @'my options.lnk'
            if (filePath.front() == '\'' && filePath.back() == '\'')
            {
                filePath = filePath.substr(1, filePath.length() - 2);
            }

            ifstream file(filePath);
            if (!file.is_open())
            {
                cerr << "Warning: Could not open indirect file: " << filePath << "\n";
                continue;
            }

            string line;
            string fileContent;
            while (getline(file, line))
            {
                // Strip hash comments or inline comment patterns if applicable
                size_t commentPos = line.find('#');
                if (commentPos != string::npos)
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
vector<LinkerParam> TokenizeJwlinkString(const string& cmdLine) {
    vector<LinkerParam> processedParams;
    string currentToken;
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
                processedParams.push_back({ string(1, ch), TokenType::Delimiter });
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
        string val = processedParams[i].value;
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
LinkerConfig GroupDirectives(const vector<LinkerParam>& tokens) {
    LinkerConfig config;
    string currentDirective = "global";

    for (const auto& token : tokens)
    {
        if (token.type == TokenType::Delimiter) continue;

        if (token.type == TokenType::Directive)
        {
            currentDirective = token.value;
            if (currentDirective == "op") currentDirective = "option"; // Standardize shorthand

            if (config.find(currentDirective) == config.end())
            {
                config[currentDirective] = vector<string>();
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
SubOptionMap ParseSubOptions(const vector<string>& optionArguments) {
    SubOptionMap subMap;
    for (const auto& arg : optionArguments)
    {
        size_t equalSignPos = arg.find('=');
        if (equalSignPos != string::npos)
        {
            string key = arg.substr(0, equalSignPos);
            string value = arg.substr(equalSignPos + 1);

            // Normalize casing to match jwlink's case-insensitive strategy
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            subMap[key] = value;
        }
        else
        {
            // Standalone flags without explicit assignments (e.g. "eliminate" or "map")
            string flag = arg;
            transform(flag.begin(), flag.end(), flag.begin(), ::tolower);
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

    // Unpack response contents recursively 
    string fullyExpandedString = ExpandResponseFiles(rawCommandLine);
    cout << "Fully expanded macro command string: \n  " << fullyExpandedString << "\n\n";

    // Process parameters through our multi-phase layout
    vector<LinkerParam> tokens = TokenizeJwlinkString(fullyExpandedString);
    LinkerConfig configMap = GroupDirectives(tokens);

    // Output grouped mapping entries
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

