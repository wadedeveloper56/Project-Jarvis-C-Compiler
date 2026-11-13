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

Supported options include `/OUT`, `/DLL`, `/DEF`, `/EXPORT`, `/IMPLIB`, `/ENTRY`, `/NOENTRY`, `/SUBSYSTEM` (including a major.minor version), `/MACHINE:X64`, `/LIBPATH`, `/DEFAULTLIB`, `/NODEFAULTLIB`, `/DELAYLOAD:<dll>`, `/ALIGN`, `/BASE`, `/FIXED`, `/DYNAMICBASE`, `/NXCOMPAT`, `/MAP`, `/INCREMENTAL:NO`, and `@response-file`. DLLs with exports generate an import library beside the output DLL by default; `/IMPLIB:<file>` selects another path. `.def` files support `LIBRARY`/`NAME` and `EXPORTS` entries with aliases, ordinals, `NONAME`, `DATA`, and `PRIVATE`. COFF `.drectve` sections recognize `/DEFAULTLIB`, `/NODEFAULTLIB`, `/DELAYLOAD`, `/ALIGN`, `/ALIGNCOMM`, `/ENTRY`, `/EXPORT`, and `/INCLUDE`; exports produce a PE export directory, and forced includes participate in archive extraction. GNU-style `-o`, `-L`, and `-l` forms are also accepted. Malformed or unsupported directives fail explicitly. Other options with unsupported behavior also fail explicitly.

## Current scope

JWlink2 targets native Windows AMD64 PE32+ executables and DLLs, using the console or Windows subsystem. The target architecture is x64 even when the linker tool is built for Win32. It does not produce 32-bit, DOS, OS/2, Win16, VxD, Phar Lap, Novell, QNX, ELF, or raw images. See [MISSING_FEATURES.md](MISSING_FEATURES.md) for the tested boundary and repository compatibility checklist.

JWlink2 generates PE import descriptors, lookup tables, IAT entries, ordinary function thunks, and delay-import descriptors. `/DELAYLOAD:<dll>` and COFF `.drectve` delay-load directives currently support one function import in the link; requests with additional delay-loaded functions, data imports, or DLLs without selected imports are rejected explicitly. The delay IAT entry initially points to a generated AMD64 resolver which calls eager `KERNEL32.dll` imports of `LoadLibraryA` and `GetProcAddress`, preserves the Microsoft x64 argument registers, caches the resolved function address in the delay IAT, and tail-jumps to it. Missing DLLs or symbols raise noncontinuable delay-load exceptions (`0xC06D007E`/`0xC06D007F`); resolver unwind records are emitted. Delay module handles, bound/unload IATs, delayimp hooks, and unload semantics are not implemented; the resolved function retains its `LoadLibraryA` reference for the image lifetime. Ordinary imports retain their existing eager-IAT path.

`/DEBUG` writes a PE RSDS debug directory entry and an MSF 7.0 PDB beside the output (`<output>.pdb`); `/PDB:<file>` selects another path, and the RSDS GUID/age match the PDB. The linker consumes MSVC `/Z7` CodeView object data (`.debug$T` type records including field lists, method lists, string/build-info ids, and `.debug$S` symbols, file checksums, string tables, and line subsections), remaps it to final RVAs, and writes the PDB info, TPI, DBI (modules, section contributions, section map, file info), empty IPI, `/names`, GSI, PSI, symbol-record, and section-header streams plus per-module symbol/line streams. Output images are validated against PE layout invariants and the Windows loader (including rebasing of DLLs with DIR64 relocations) in `JWlink2Test`. COMDAT sections without an external symbol (MSVC `/Gy` static functions) are kept unmerged. Windows `dbghelp` resolves public symbols, source files, and lines from the result (covered by `JWlink2Test`). Unsupported CodeView leaves and subsections (inline lines, frame data, cross-scope records) are rejected explicitly; separate IPI/TPI hash streams are not produced and LLVM `llvm-pdbutil` validation has not been performed.

### JWlink directive syntax and option matrix

The repository's real build (`test\run.bat`) invokes JWlink with Watcom-style directives: `JWlink format win pe option map=<file> file a.obj, b.obj lib x.lib y.lib name out.exe`. JWlink2 accepts that form when the first argument is a directive (abbreviations such as `form`, `fi`, `op`, `na` work; `@response-file` may contain directives), translates it to its switch model, and rejects everything else explicitly. Directives and `/switches` cannot be mixed in one command. `JWlink2Test` (`OptionMatrixAcceptsAndRejectsDocumentedForms`, `JwlinkDirectiveSyntaxLinksLikeEquivalentSwitches`) exercises every row below.

| JWlink directive | Status | JWlink2 behavior |
|---|---|---|
| `format windows pe [dll\|exe]` (also `win`, `nt`) | Accepted | `dll` selects `/DLL`; other formats (OS/2, DOS, ELF, bare `format windows`) are rejected |
| `file a,b` | Accepted | Object inputs; `.obj` is appended when no extension is given |
| `library x` / `libfile x` | Accepted | Archive inputs; `.lib` is appended when no extension is given |
| `libpath p;q` | Accepted | `/LIBPATH` |
| `name out` | Accepted | `/OUT` (`.exe`/`.dll` appended; defaults to the first `file` stem) |
| `system nt \| nt_con \| nt_win \| nt_dll` | Accepted | Console or Windows subsystem; `nt_dll` also selects a DLL |
| `runtime console\|windows[=major.minor]` | Accepted | `/SUBSYSTEM` |
| `debug all \| codeview` | Accepted | `/DEBUG` (CodeView/PDB); DWARF/Watcom/Novell debug formats are rejected |
| `export name[=internal]` | Accepted | `/EXPORT`; JWlink alias/ordinal (`.`) syntax is rejected |
| `disable <n>` | Accepted, no effect | JWlink2 emits no numbered warnings |
| `option map[=file]` | Accepted | `/MAP` |
| `option quiet` (`q`) | Accepted | `/NOLOGO` |
| `option start=sym` | Accepted | `/ENTRY` |
| `option nodefaultlibs` | Accepted | `/NODEFAULTLIB` |
| `option caseexact` | Accepted, no effect | Symbol lookup is already case-sensitive |
| `option offset=n` | Accepted | `/BASE` |
| `option objalign=n` | Accepted | `/ALIGN` (section alignment) |
| `option norelocs` | Accepted | `/FIXED` |
| `option implib[=file]` | Accepted | `/IMPLIB` (requires a DLL) |
| `alias`, `import`, `order`, `segment`, `sort`, `reference`, `path`, `commit`, `language`, `modfile`, `modtrace`, `symtrace`; other `option`s (`stack`, `heap`, `eliminate`, ...) | Unsupported | Diagnosed with `unsupported JWlink directive/OPTION` |

Switch-style options not listed in the option summary above (`/MANIFEST*`, `/STACK`, `/HEAP`, `/MERGE`, `/SECTION`, `/WHOLEARCHIVE`, `/VERBOSE`, `/OPT`, `/LTCG`, `/MACHINE` other than X64, `/INCREMENTAL` other than `:NO`) are rejected with `unsupported option`; the `-l<name>` library shorthand applies only to `-` prefixed arguments.
Application manifests and `.res` resource inputs are not implemented because no existing build uses them: `/MANIFEST*` options and `.res` files are rejected explicitly, while resources supplied as COFF `.rsrc` sections (for example from `cvtres`) are passed through to the PE resource directory.

LTCG/LLVM bitcode is not linked:
