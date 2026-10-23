#include "pch.h"
#include "globals.h"
#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace CLI;

int main(int argc, const char* argv[]) {
    string output;
    vector<string> format;
    vector<string> objects;
    vector<string> libraries;
    vector<string> options;

    printf("JWlink - 16/32/64 bit ELF/OMF/COFF supported linker v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
    printf("All Rights Reserved\n");
    App app("JWlink");
    app.set_version_flag("-v,--version", string("1.0"));
    app.add_option("--output,output", output, "output file name")->required();
    app.add_option("--format,format", format, "Target format (e.g., windows pe, elf)")->required();
    app.add_option("-o,--obj", objects, "List of input object files")->expected(-1);
    app.add_option("-l,--lib", libraries, "List of input library files")->expected(-1);
    app.add_option("option", options, "Linker options/directives")->expected(-1);

    CLI11_PARSE(app, argc, argv);

	cout << "Output file: " << output << '\n';
    cout << "Received " << format.size() << " format files:\n";
    for (const auto& file : format)
    {
        cout << " - " << file << '\n';
    }
    cout << "Received " << objects.size() << " object files:\n";
    for (const auto& file : objects)
    {
        cout << " - " << file << '\n';
    }
    cout << "Received " << libraries.size() << " library files:\n";
    for (const auto& file : libraries)
    {
        std::cout << " - " << file << '\n';
    }
    return 0;
}
