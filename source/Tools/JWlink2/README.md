# JWlink2

JWlink2 is a new, standalone AMD64 linker. Its first vertical slice reads x64 COFF `.obj` files and standard COFF `.lib` archives, extracts archive members needed to satisfy unresolved symbols, applies common AMD64 relocations, and writes a fixed-base Windows PE32+ executable.

## Build

Build the `JWlink2` project in `source\Tools\Tools.sln` for either Win32 or x64. The generated linker executable is written to the repository's `bin\tools\<Configuration>\<Platform>` directory.

## Usage

```text
JWlink2 -o app.exe --entry main app.obj helpers.lib
```

The default entry point is `mainCRTStartup`, falling back to `main`. The selected symbol is called directly by the Windows loader; JWlink2 does not initialize or link the C runtime yet, so the `main` fallback is for simple freestanding entry routines. Specify `--subsystem windows` for a GUI image; the default is `console`.

## Initial scope

This version intentionally supports only AMD64 COFF objects and regular static-object members in Microsoft library archives. It supports `ADDR64`, `ADDR32`, `ADDR32NB`, `REL32`, and `REL32_1` through `REL32_5` relocations. Import-library records, DLL imports, bigobj/LTCG objects, debug data, PE base relocations/ASLR, and most PE linker options are not implemented. Unsupported architectures, malformed input, unresolved symbols, and unsupported relocations are errors. JWlink2 is not yet a replacement for JWlink or MSVC `link.exe` for general C/C++ programs.

The next architectural stages are import-library support and PE import/IAT generation, COMDAT and weak-symbol selection, base relocations, and broader command-line compatibility. The existing JWlink remains the full-featured linker while this implementation grows independently.
