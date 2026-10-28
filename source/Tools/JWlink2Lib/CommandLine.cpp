#include "pch.h"
#include "CommandLine.h"
#include "globals.h"
#include <iostream>
#include <set>
#include <string>
#include <iterator>

int g_LinkStatus;

CommandLine::CommandLine(int argc, char** argv) : m_argc(argc), m_argv(argv)
{
	g_LinkStatus = LNK_OK; // Initialize global status to OK at the start of the command line processing
}

CommandLine::~CommandLine()
{
}

namespace Validation
{
    //const set<string> ValidDirectives = {
    //    "format", "file", "option", "op", "name", "library", "lib",
    //    "path", "output", "debug", "import", "export", "system", "sys"
    //};

    const set<string> ValidSubOptions = {
        "stack", "offset", "eliminate", "map", "quiet", "verbose",
        "caseexact", "cvpack", "nodefaultlibs", "start", "align"
    };

    void PrintHelp() {
        cout << "=======================================================================\n";
        cout << "jwlink Duplicate Linker Emulator - Command Line Parameter Help\n";
        cout << "=======================================================================\n\n";
        cout << "Syntax:\n";
        cout << "  jwlink <directive> <arguments> [<directive> <arguments> ...]\n";
        cout << "  jwlink @<response_file>\n\n";

        cout << "Primary Directives:\n";
        cout << "  format <target>   Specifies target operating system executable layout.\n";
        cout << "  file <obj_files>  Comma-separated list of object modules to link.\n";
        cout << "  option, op <opts> Comma-separated tuning switches / configurations.\n";
        cout << "  name <output>     Overrides default executable binary output filename.\n\n";

        cout << "Supported Sub-Options (Passed via 'option' or 'op'):\n";
        cout << "  stack=<size>      Sets the stack segment memory allocation (e.g., stack=0x4000).\n";
        cout << "  eliminate         Performs dead-code elimination on unused procedures.\n";
        cout << "  map               Generates a map file tracking addresses and symbols.\n";
        cout << "=======================================================================\n";
    }

    bool IsHelpRequest(const string& inputStr) {
        string clean = inputStr;
        clean.erase(0, clean.find_first_not_of(" \t\r\n"));
        clean.erase(clean.find_last_not_of(" \t\r\n") + 1);
        transform(clean.begin(), clean.end(), clean.begin(), ::tolower);
        return (clean == "?" || clean == "-?" || clean == "/?" || clean == "--help" || clean == "-help");
    }

    bool IsValidDirective(const string& directive) {
        string lowerDir = directive;
        transform(lowerDir.begin(), lowerDir.end(), lowerDir.begin(), ::tolower);
        set<ParseEntry> ValidDirectives(Directives, Directives + 40);
        ParseEntry target{ lowerDir, 0 };
        return ValidDirectives.find(target) != ValidDirectives.end();
    }

    void ValidateConfig(const LinkerConfig& config) {
        cout << "\n[Validation Report]\n";
        cout << "----------------------------------------\n";

        for (const auto& [directive, arguments] : config)
        {
            // Unrecognized primary block halts execution framework entirely
            if (directive != "global" && !IsValidDirective(directive))
            {
                cerr << " -> ERROR: Unknown top-level directive structural keyword: '" << directive << "'\n";
                if (g_LinkStatus < LNK_ERRORS) g_LinkStatus = LNK_ERRORS;
                continue;
            }

            if (directive == "option")
            {
                for (const auto& arg : arguments)
                {
                    size_t eqPos = arg.find('=');
                    string key = (eqPos != string::npos) ? arg.substr(0, eqPos) : arg;
                    transform(key.begin(), key.end(), key.begin(), ::tolower);

                    // Misspelled parameters fall into WARNING category without breaking operational link
                    if (ValidSubOptions.find(key) == ValidSubOptions.end())
                    {
                        cerr << " -> WARNING: Unrecognized sub-option or flag inside 'option': '" << key << "'\n";
                        if (g_LinkStatus < LNK_WARNINGS) g_LinkStatus = LNK_WARNINGS;
                    }
                }
            }
        }

        if (g_LinkStatus == LNK_OK)
        {
            cout << " -> Linker properties validated successfully. Zero anomalies found.\n";
        }
        cout << "----------------------------------------\n\n";
    }
}

string CommandLine::ExpandResponseFiles(const string& inputStr) {
    stringstream source(inputStr);
    string word;
    string accumulatedCommand;

    while (source >> word)
    {
        if (!word.empty() && word[0] == '@')
        {
            string filePath = word.substr(1);
            if (filePath.front() == '\'' && filePath.back() == '\'')
            {
                filePath = filePath.substr(1, filePath.length() - 2);
            }

            ifstream file(filePath);
            if (!file.is_open())
            {
                cerr << "FATAL: Could not resolve indirect parameter mapping layout path: " << filePath << "\n";
                g_LinkStatus = LNK_FATAL; // Set to fatal state immediately
                return "";
            }

            string line;
            string fileContent;
            while (getline(file, line))
            {
                size_t commentPos = line.find('#');
                if (commentPos != string::npos)
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

vector<LinkerParam> CommandLine::TokenizeJwlinkString(const string& cmdLine) {
    vector<LinkerParam> processedParams;
    string currentToken;
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

    for (size_t i = 0; i < processedParams.size(); ++i)
    {
        string val = processedParams[i].value;
        transform(val.begin(), val.end(), val.begin(), ::tolower);
        set<ParseEntry> Directives(Directives, Directives + 40);
        ParseEntry target{ val, 0 };
        if (Directives.find(target) != Directives.end())
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

LinkerConfig CommandLine::GroupDirectives(const vector<LinkerParam>& tokens) {
    LinkerConfig config;
    string currentDirective = "global";

    for (const auto& token : tokens)
    {
        if (token.type == TokenType::Delimiter) continue;

        if (token.type == TokenType::Directive)
        {
            currentDirective = token.value;
            transform(currentDirective.begin(), currentDirective.end(), currentDirective.begin(), ::tolower);
            if (currentDirective == "op") currentDirective = "option";

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

void CommandLine::convert(int argc, char* argv[], string& flattenedCmdLine)
{
    for (int i = 1; i < argc; ++i)
    {
        string argStr(argv[i]);
        if (argStr.find(' ') != string::npos && argStr.front() != '\'')
        {
            argStr = "'" + argStr + "'";
        }
        flattenedCmdLine += argStr + " ";
    }
}

int CommandLine::ProcessCommandLine() {
    g_LinkStatus = LNK_OK; // Reset runtime loop metrics state

    if (m_argc < 2)
    {
        cout << "Error: No linker parameters supplied.\n";
        Validation::PrintHelp();
        return LNK_ERRORS;
    }

    if (Validation::IsHelpRequest(m_argv[1]))
    {
        Validation::PrintHelp();
        return LNK_OK;
    }

    string flattenedCmdLine;
    convert(m_argc, m_argv, flattenedCmdLine);

    string fullyExpandedString = ExpandResponseFiles(flattenedCmdLine);
    if (g_LinkStatus == LNK_FATAL) return LNK_FATAL;
    vector<LinkerParam> tokens = TokenizeJwlinkString(fullyExpandedString);
    configMap = GroupDirectives(tokens);
    Validation::ValidateConfig(configMap);


    
    return g_LinkStatus;
}
