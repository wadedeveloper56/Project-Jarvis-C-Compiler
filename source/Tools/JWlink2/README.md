# JWlink2

JWlink2 is an independent AMD64 linker that reads 64-bit COFF objects and Microsoft `.lib` archives and emits Windows PE32+ executables or DLLs. The existing JWlink remains unchanged and is still the full-featured linker.

## Build

Build the `JWlink2` project in `source\Tools\Tools.sln` for either Win32 or x64. The generated linker executable is written to the repository's `bin\tools\<Configuration>\<Platform>` directory.

## Usage

```text
JWlink2 /OUT:app.exe /ENTRY:main /SUBSYSTEM:CONSOLE app.obj helpers.lib
JWlink2 /DLL /DEF:exports.def /OUT:plugin.dll plugin.obj
JWlink2 @link.rsp
```

For executables, the default entry point is `mainCRTStartup`, falling back to `main`. For DLLs, JWlink2 selects `DllMainCRTStartup` when present; otherwise the DLL entry RVA is zero. `/ENTRY:<symbol>` selects an explicit entry, while `/NOENTRY` omits it. The selected symbol is called directly by the Windows loader; JWlink2 does not initialize or link the C runtime. The default subsystem is console; use `/SUBSYSTEM:WINDOWS` for a GUI image.

Supported options include `/OUT`, `/DLL`, `/DEF`, `/EXPORT`, `/IMPLIB`, `/ENTRY`, `/NOENTRY`, `/SUBSYSTEM` (including a major.minor version), `/MACHINE:X64`, `/LIBPATH`, `/DEFAULTLIB`, `/NODEFAULTLIB`, `/ALIGN`, `/BASE`, `/FIXED`, `/DYNAMICBASE`, `/NXCOMPAT`, `/MAP`, `/INCREMENTAL:NO`, and `@response-file`. DLLs with exports generate an import library beside the output DLL by default; `/IMPLIB:<file>` selects another path. `.def` files support `LIBRARY`/`NAME` and `EXPORTS` entries with aliases, ordinals, `NONAME`, `DATA`, and `PRIVATE`. COFF `.drectve` sections recognize `/DEFAULTLIB`, `/NODEFAULTLIB`, `/ALIGN`, `/ALIGNCOMM`, `/ENTRY`, `/EXPORT`, and `/INCLUDE`; exports produce a PE export directory, and forced includes participate in archive extraction. GNU-style `-o`, `-L`, and `-l` forms are also accepted. Malformed or unsupported directives fail explicitly. Other options with unsupported behavior also fail explicitly.

## Current scope

JWlink2 targets native Windows AMD64 PE32+ executables and DLLs, using the console or Windows subsystem. The target architecture is x64 even when the linker tool is built for Win32. It does not produce 32-bit, DOS, OS/2, Win16, VxD, Phar Lap, Novell, QNX, ELF, or raw images. See [MISSING_FEATURES.md](MISSING_FEATURES.md) for the tested boundary and repository compatibility checklist.

JWlink2 supports standard AMD64 COFF and Microsoft BigObj files, including 32-bit section counts and symbol section indices, 20-byte symbol/auxiliary records, and BigObj members in static archives; it also supports short import-object members. Microsoft archive first and second linker-member symbol indexes are parsed so only indexed members needed by unresolved symbols are extracted; archives without indexes retain a member-scan fallback. It also handles archive long-name tables. JWlink2 generates PE import descriptors, lookup tables, IAT entries, and function thunks, and supports COMDAT selection (including associative sections) and weak externals. Supported AMD64 relocations are `ADDR64`, `ADDR32`, `ADDR32NB`, and `REL32` through `REL32_5`. `/DYNAMICBASE` emits base relocations for supported absolute addresses; `/FIXED` disables them. Map files report linked symbol addresses, including import thunks and IAT entries.

LTCG/LLVM bitcode is not linked: raw LLVM bitcode, the LLVM bitcode wrapper format, and COFF objects containing recognized `.llvmbc`, `.llvm.lto`, or `.lto$...` sections are rejected with an explicit diagnostic directing users to compile without LTO and provide native AMD64 COFF objects. The same detectable formats are rejected inside library archives. JWlink2 does not invoke an LTO backend. Proprietary MSVC `/GL` objects are outside the accepted input format; because they do not have a guaranteed public marker, they may be rejected by general COFF validation rather than the targeted bitcode diagnostic. Debug data, full C/C++ runtime startup, and many advanced `link.exe` options are also not implemented. Unsupported architectures, malformed input (including invalid BigObj signatures), unresolved symbols, and unsupported options or relocations are errors.
