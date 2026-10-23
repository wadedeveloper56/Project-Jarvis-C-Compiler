#include "pch.h"
#include "globals.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace CLI;

struct JwlinkConfig
{
    std::vector<std::string> system_format;
    std::vector<std::string> obj_files;
    std::vector<std::string> lib_files; // Holds the comprehensive list of libraries
    std::string output_file;
};

int main(int argc, const char* argv[]) {

    printf("JWlink - 16/32/64 bit ELF/OMF/COFF supported linker v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
    printf("All Rights Reserved\n");

    App app("JWlink");
    JwlinkConfig config;

    app.add_option("-s,--sys", config.system_format, "Specify output format")->delimiter(',');
    app.add_option("files", config.obj_files, "Input object modules (.obj, .o)")->delimiter(',');
    app.add_option("-l,--lib", config.lib_files, "Specify import libraries")->delimiter(','); // Splits "-l user32,gdi32" into separate elements automatically
    app.add_option("-o,--out", config.output_file, "Explicit output binary path");
     
    CLI11_PARSE(app, argc, argv);

    cout << "System Formats (" << config.system_format.size() << "):\n";
    for (const auto& format : config.system_format) std::cout << "  - " << format  << "\n";
    cout << "Object Files (" << config.obj_files.size() << "):\n";
    for (const auto& file : config.obj_files) std::cout << "  - " << file << "\n";
    std::cout << "Libraries Linked (" << config.lib_files.size() << "):\n";
    for (const auto& lib : config.lib_files) std::cout << "  - " << lib << "\n";
    return 0;
}
