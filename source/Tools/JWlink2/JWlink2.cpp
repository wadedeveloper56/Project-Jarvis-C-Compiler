#include "pch.h"
#include "globals.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

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

int main(int argc, const char* argv[]) {

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
