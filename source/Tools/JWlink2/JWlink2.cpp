#include "pch.h"
#include "globals.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
using namespace CLI;

struct JwlinkConfig
{
    string system_type;
    string format_type;
    vector<string> obj_files;
    vector<string> lib_files; // Holds the comprehensive list of libraries
    string output_file;
};

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Enumeration matching token types encountered during processing
enum TokenType
{
    Directive,   // Primary keyword (e.g., FORMAT, FILE, OPTION)
    Argument,    // Target or state modifier (e.g., WINDOWS, PE)
    ListElement, // Comma-delimited entries (e.g., main.obj)
    Delimiter    // Separators like commas or equal signs
};

struct LinkerParam
{
    string value;
    enum TokenType type;
};

// Replicates the processing logic for tokens, tracking quote states
vector<LinkerParam> ProcessJwlinkArgs(const string& cmdLine) {
    vector<LinkerParam> processedParams;
    string currentToken;
    bool insideQuotes = false;

    for (size_t i = 0; i < cmdLine.length(); ++i)
    {
        char ch = cmdLine[i];

        // Toggle quote state if an apostrophe (') is encountered
        if (ch == '\'')
        {
            insideQuotes = !insideQuotes;
            continue; // Strip the apostrophe like jwlink does
        }

        if (insideQuotes)
        {
            // Treat everything literally inside single quotes
            currentToken += ch;
        }
        else
        {
            // Delimiter evaluation outside of quotes
            if (ch == ' ' || ch == '\t')
            {
                if (!currentToken.empty())
                {
                    processedParams.push_back({ currentToken, TokenType::Argument });
                    currentToken.clear();
                }
            }
            else if (ch == ',' || ch == '=')
            {
                if (!currentToken.empty())
                {
                    processedParams.push_back({ currentToken, TokenType::ListElement });
                    currentToken.clear();
                }
                string delim(1, ch);
                processedParams.push_back({ delim, TokenType::Delimiter });
            }
            else
            {
                currentToken += ch;
            }
        }
    }

    // Flush any remaining tokens at end of string
    if (!currentToken.empty())
    {
        processedParams.push_back({ currentToken, TokenType::Argument });
    }

    // Contextual Post-Processing: Assign Directive types
    // Rules mimic Watcom style: structural context dictates if an argument is a master Directive
    bool nextIsDirective = true;
    for (auto& param : processedParams)
    {
        if (nextIsDirective && param.type == TokenType::Argument)
        {
            param.type = TokenType::Directive;
            nextIsDirective = false;
        }
        // If we hit a comma delimiter, the next element continues the active list, not a directive
        if (param.type == TokenType::Delimiter && param.value == ",")
        {
            nextIsDirective = false;
        }
        // Specific top-level structural keywords reset directive state
        if (param.value == "format" || param.value == "file" || param.value == "option" || param.value == "op")
        {
            param.type = TokenType::Directive;
        }
    }

    return processedParams;
}

int main() {
    // Mimics: jwlink format windows pe file main.obj, 'my lib file.obj' op eliminate, map
    string mockInput = "format windows pe file main.obj, 'my lib file.obj' op eliminate, map";

    cout << "Processing command line string:\n\"" << mockInput << "\"\n\n";
    vector<LinkerParam> tokens = ProcessJwlinkArgs(mockInput);

    cout << "Parsed Results:\n";
    cout << "------------------------------------------\n";
    for (const auto& token : tokens)
    {
        string typeStr;
        switch (token.type)
        {
            case TokenType::Directive:   typeStr = "Directive"; break;
            case TokenType::Argument:    typeStr = "Argument"; break;
            case TokenType::ListElement: typeStr = "List Element"; break;
            case TokenType::Delimiter:   typeStr = "Delimiter"; break;
        }
        cout << "[" << typeStr << "] -> " << token.value << "\n";
    }

    return 0;
}

int main2(int argc, const char* argv[]) {

    printf("JWlink - 16/32/64 bit ELF/OMF/COFF supported linker v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
    printf("All Rights Reserved\n");

    App app("JWlink");
    JwlinkConfig config;

    app.add_option("SYS,--system", config.system_type, "Target operating system (e.g., win32, win64, hxdosr, elf64)");
    app.add_option("FORM,--format", config.format_type, "Output executable format type (e.g., windows pe, elf, os2)");
    app.add_option("-f,--files", config.obj_files, "Input object modules (.obj, .o)")->delimiter(',');
    app.add_option("-l,--lib", config.lib_files, "Specify import libraries")->delimiter(',');
    app.add_option("-o,--out", config.output_file, "Explicit output binary path");
     
    CLI11_PARSE(app, argc, argv);

    cout << "System Type: " << config.system_type << "\n";
	cout << "Format Type: " << config.format_type << "\n";
	cout << "Output File: " << config.output_file << "\n";
    cout << "Object Files (" << config.obj_files.size() << "):\n";
    for (const auto& file : config.obj_files) cout << "  - " << file << "\n";
    cout << "Libraries Linked (" << config.lib_files.size() << "):\n";
    for (const auto& lib : config.lib_files) cout << "  - " << lib << "\n";
    return 0;
}
