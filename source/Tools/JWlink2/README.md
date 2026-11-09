# JWlink2

JWlink2 is an independent AMD64 linker that reads 64-bit COFF objects and Microsoft `.lib` archives and emits Windows PE32+ executables. The existing JWlink remains unchanged and is still the full-featured linker.

## Build

Build the `JWlink2` project in `source\Tools\Tools.sln` for either Win32 or x64. The generated linker executable is written to the repository's `bin\tools\<Configuration>\<Platform>` directory.

## Usage

```text
JWlink2 /OUT:app.exe /ENTRY:main /SUBSYSTEM:CONSOLE app.obj helpers.lib
JWlink2 @link.rsp
```

The default entry point is `mainCRTStartup`, falling back to `main`. The selected symbol is called directly by the Windows loader; JWlink2 does not initialize or link the C runtime, so the `main` fallback is for freestanding entry routines. The default subsystem is console; use `/SUBSYSTEM:WINDOWS` for a GUI image.

Supported options include `/OUT`, `/ENTRY`, `/SUBSYSTEM` (including a major.minor version), `/MACHINE:X64`, `/LIBPATH`, `/DEFAULTLIB`, `/NODEFAULTLIB`, `/ALIGN`, `/BASE`, `/FIXED`, `/DYNAMICBASE`, `/NXCOMPAT`, `/MAP`, `/INCREMENTAL:NO`, and `@response-file`. GNU-style `-o`, `-L`, and `-l` forms are also accepted. COFF `.drectve` sections recognize `/DEFAULTLIB`, `/NODEFAULTLIB`, `/ALIGN`, `/ALIGNCOMM`, `/ENTRY`, `/EXPORT`, and `/INCLUDE`; exports produce a PE export directory, and forced includes participate in archive extraction. Malformed or unsupported directives fail explicitly. Other options with unsupported behavior also fail explicitly.

## Current scope

JWlink2 targets native Windows AMD64 PE32+ **executables only**, using the console or Windows subsystem. The target architecture is x64 even when the linker tool is built for Win32. It is not a DLL or driver linker and does not produce 32-bit, DOS, OS/2, Win16, VxD, Phar Lap, Novell, QNX, ELF, or raw images. See [MISSING_FEATURES.md](MISSING_FEATURES.md) for the tested boundary and repository compatibility checklist.

JWlink2 supports AMD64 COFF objects, regular AMD64 object members in Microsoft archives, and short import-object members. It extracts archive members to satisfy unresolved references, generates PE import descriptors, lookup tables, IAT entries, and function thunks, and supports COMDAT selection (including associative sections) and weak externals. Supported AMD64 relocations are `ADDR64`, `ADDR32`, `ADDR32NB`, and `REL32` through `REL32_5`. `/DYNAMICBASE` emits base relocations for supported absolute addresses; `/FIXED` disables them. Map files report linked symbol addresses, including import thunks and IAT entries.

Bigobj/LTCG objects, debug data, full C/C++ runtime startup, and many advanced `link.exe` options are not implemented. Unsupported architectures, malformed input, unresolved symbols, and unsupported options or relocations are errors. JWlink2 is an independently growing implementation, not yet a replacement for JWlink or MSVC `link.exe` for general C/C++ programs. See [MISSING_FEATURES.md](MISSING_FEATURES.md) for the compatibility checklist and progress criteria.
