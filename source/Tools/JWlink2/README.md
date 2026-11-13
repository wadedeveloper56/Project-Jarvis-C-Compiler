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

In switch mode, the executable entry point defaults to `mainCRTStartup`, falling back to `main`. For DLLs, JWlink2 selects `DllMainCRTStartup` when present; otherwise the DLL entry RVA is zero. `/ENTRY:<symbol>` selects an explicit entry, while `/NOENTRY` omits it. In JWlink directive mode, `option start=<symbol>` or a COFF `/ENTRY` directive selects the entry. Otherwise, an EXE warns and uses the first output section, approximating JWlink's first-group fallback; a DLL has a zero entry RVA. Supply an explicit start for predictable startup, especially with multiple sections. The selected entry is called directly by the Windows loader; JWlink2 does not initialize or link the C runtime. The default subsystem is console; use `/SUBSYSTEM:WINDOWS` or `runtime windows` for a GUI image.

Switch mode defaults to `a.exe` or `a.dll`; JWlink directive mode uses the first input's stem in the current directory unless `name` is given. Libraries specified by the command line or COFF `/DEFAULTLIB` are searched as needed; `/NODEFAULTLIB` can exclude default libraries but not explicit archive inputs. No CRT library is implicitly added. Unresolved-symbol diagnostics list known referencing objects; duplicate-definition errors identify both objects. JWlink2 returns 0 on success and 1 on diagnosed errors.

Supported options include `/OUT`, `/DLL`, `/DEF`, `/EXPORT`, `/IMPLIB`, `/ENTRY`, `/NOENTRY`, `/SUBSYSTEM` (including a major.minor version), `/MACHINE:X64`, `/LIBPATH`, `/DEFAULTLIB`, `/NODEFAULTLIB`, `/DELAYLOAD:<dll>`, `/ALIGN`, `/BASE`, `/FIXED`, `/DYNAMICBASE`, `/NXCOMPAT`, `/MAP`, `/INCREMENTAL:NO`, and `@response-file`. DLLs with exports generate an import library beside the output DLL by default; `/IMPLIB:<file>` selects another path. `.def` files support `LIBRARY`/`NAME` and `EXPORTS` entries with aliases, ordinals, `NONAME`, `DATA`, and `PRIVATE`. COFF `.drectve` sections recognize `/DEFAULTLIB`, `/NODEFAULTLIB`, `/DELAYLOAD`, `/ALIGN`, `/ALIGNCOMM`, `/ENTRY`, `/EXPORT`, and `/INCLUDE`; exports produce a PE export directory, and forced includes participate in archive extraction. GNU-style `-o`, `-L`, and `-l` forms are also accepted. Malformed or unsupported directives fail explicitly. Other options with unsupported behavior also fail explicitly.

## Current scope

JWlink2 targets native Windows AMD64 PE32+ executables and DLLs, using the console or Windows subsystem. The target architecture is x64 even when the linker tool is built for Win32. It does not produce 32-bit, DOS, OS/2, Win16, VxD, Phar Lap, Novell, QNX, ELF, or raw images. See [MISSING_FEATURES.md](MISSING_FEATURES.md) for the tested boundary and repository compatibility checklist.

COFF input sections are grouped by their name before `$` and sorted by the full, case-sensitive name within each group. Equal names retain input-object and section order, including selected archive members. Alignment, relocations and selected associative COMDAT mappings follow the sorted contributions. When `/MERGE` combines different section families, those families retain first-seen order and each family is sorted separately. Runtime tests execute reversed `.tls$*` callbacks and manually invoked `.CRT$X*` constructor pointers; they do not supply CRT startup. In a separate fixture JWlink also orders A before Z but groups them in `.rdata` with different padding.

Ordinary COFF and BigObj relocation-overflow tables are supported, including archive members. The overflow flag requires a 0xffff header count and a valid first record whose count includes that record; malformed or truncated tables are rejected. The object-only overflow flag is not copied into PE section flags. Runtime AMD64 relocations support `ABSOLUTE`, `ADDR64`, `ADDR32`, `ADDR32NB`, `REL32` through `REL32_5`, `SECTION`, and `SECREL`. `SECTION` and `SECREL` use final output-section coordinates; an absolute `SECTION` target uses the index after the last output section, while an absolute `SECREL` target is rejected. `SECREL7`, managed `TOKEN`, `SREL32`, `PAIR`, and `SSPAN32` are not supported. Separately, `/DEBUG` consumes CodeView `.debug$S` `SECTION` and `SECREL` relocations in object coordinates and rejects other types; `.debug$T` relocations are unsupported. MSVC `/O1 /Z7` TLS code and MASM overflow objects are checked when the x64 tools are installed; the TLS fixture verifies linked bytes rather than TLS startup. Boundary and malformed tables are always covered by synthetic tests.

JWlink2 generates PE import descriptors, lookup tables, IAT entries, ordinary function thunks, and delay-import descriptors. `/DELAYLOAD:<dll>` and COFF `.drectve` delay-load directives currently support one function import in the link; requests with additional delay-loaded functions, data imports, or DLLs without selected imports are rejected explicitly. The delay IAT entry initially points to a generated AMD64 resolver which calls eager `KERNEL32.dll` imports of `LoadLibraryA` and `GetProcAddress`, preserves the Microsoft x64 argument registers, caches the resolved function address in the delay IAT, and tail-jumps to it. Missing DLLs or symbols raise noncontinuable delay-load exceptions (`0xC06D007E`/`0xC06D007F`); resolver unwind records are emitted. Delay module handles, bound/unload IATs, delayimp hooks, and unload semantics are not implemented; the resolved function retains its `LoadLibraryA` reference for the image lifetime. Ordinary imports retain their existing eager-IAT path.

`/DEBUG` writes a PE RSDS debug directory entry and an MSF 7.0 PDB beside the output (`<output>.pdb`); `/PDB:<file>` selects another path, and the RSDS GUID/age match the PDB. The linker consumes MSVC `/Z7` CodeView object data (`.debug$T` type records including field lists, method lists, string/build-info ids, and `.debug$S` symbols, file checksums, string tables, and line subsections), remaps it to final RVAs, and writes the PDB info, TPI, DBI (modules, section contributions, section map, file info), empty IPI, `/names`, GSI, PSI, symbol-record, and section-header streams plus per-module symbol/line streams. Output images are validated against PE layout invariants and the Windows loader (including rebasing of DLLs with DIR64 relocations) in `JWlink2Test`. COMDAT sections without an external symbol (MSVC `/Gy` static functions) are kept unmerged. Windows `dbghelp` resolves public symbols, source files, and lines from the result (covered by `JWlink2Test`). Unsupported CodeView leaves and subsections (inline lines, frame data, cross-scope records) are rejected explicitly; separate IPI/TPI hash streams are not produced and LLVM `llvm-pdbutil` validation has not been performed.

### JWlink directive syntax and option matrix

The repository's real build (`test\run.bat`) invokes JWlink with Watcom-style directives: `JWlink format win pe option map=<file> file a.obj, b.obj lib x.lib y.lib name out.exe`. JWlink2 accepts that form when the first argument is a directive (abbreviations such as `form`, `fi`, `op`, `na` work; `@response-file` may contain directives), translates it to its switch model, and rejects everything else explicitly. Directives and `/switches` cannot be mixed in one command. `JWlink2Test` (`OptionMatrixAcceptsAndRejectsDocumentedForms`, `JwlinkDirectiveSyntaxLinksLikeEquivalentSwitches`) exercises every row below.

| JWlink directive | Status | JWlink2 behavior |
|---|---|---|
| `format windows pe [dll\|exe]` (also `win`, `nt`) | Accepted | `dll` selects `/DLL`; other formats (OS/2, DOS, ELF, bare `format windows`) are rejected |
| `file a,b` | Accepted | Object inputs; `.obj` is appended when no extension is given |
| `library x` / `libfile x` | Accepted | Archive inputs; `.lib` is appended when no extension is given |
| `libpath p;q` | Accepted | Search paths; later `libpath` directives take priority over earlier ones |
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

Switch-style options not listed in the option summary above (`/OPT`, `/LTCG`, `/MACHINE` other than X64, `/INCREMENTAL` other than `:NO`, and anything else unrecognized) are rejected with `unsupported option`; the `-l<name>` library shorthand applies only to `-` prefixed arguments.

### Command sources and search paths

Response files use double quotes and Windows-style backslash/quote escaping. They may nest to 32 levels; relative response names and input paths are resolved against the process working directory, not the containing response file. `@name` without an extension searches `name.lnk`; if the file cannot be opened, `name` is looked up as an environment variable containing more command text. A token beginning `%NAME%` expands the variable as command text, with any token suffix appended. Expansion cycles and undefined variables are errors. Response files without a BOM use the process ANSI code page; UTF-8 BOM and UTF-16LE BOM files are decoded to that code page with unrepresentable characters rejected. UTF-16BE and embedded NULs are rejected. This BOM handling is a JWlink2 extension, not a claim about JWlink's encoding support.

For names without a directory, JWlink2 checks the current directory first, then explicit search paths, then environment paths. Directive-style `libpath` entries are searched newest directive first, followed by `LIBPE`, `LIB386`, and `LIB`; switch-style `/LIBPATH` entries are searched in command-line order, followed by `LIB`. Semicolon-separated entries are searched left to right. A name with a directory bypasses search paths. `/ENTRY` and `/ALIGN` on the command line override corresponding COFF directives, while repeated scalar switches such as `/OUT` and `/MAP` use the last occurrence. The tests compare environment command expansion with JWlink and exercise the other rules with controlled subprocess environments.

Compatibility is not yet complete: the directive translator splits commas even inside quoted filenames, percent expansion is supported only at the beginning of a token, and JWlink command-file comments, single-quote syntax, initialization files, full source-location-aware precedence, and object-versus-library search distinctions are not implemented. JWlink2 applies the same search paths to unqualified object names. Additional differential tests against JWlink are needed before claiming full parity.

Additional controls (MSVC-style switches; not available as JWlink directives):

| Switch | Behavior |
| --- | --- |
| `/INCLUDE:<symbol>` | Forces the symbol to be resolved, pulling in the defining archive member. |
| `/WHOLEARCHIVE[:<lib>]` | Links every object member of the named archive (or of every archive when no name is given). Import-library members stay demand-driven. |
| `/STACK:<reserve>[,<commit>]`, `/HEAP:<reserve>[,<commit>]` | Written to the PE optional header (defaults 0x100000 / 0x1000); commit may not exceed reserve. |
| `/MERGE:<from>=<to>` | Merges an output section into another. Linker-managed sections (`.pdata`, `.reloc`, `.idata`, `.edata`, `.rsrc`, `.tls`, `.didat`, `.debug`), self-merges, and chained merges are rejected. |
| `/SECTION:<name>,[ERWSDKP]` | Replaces the memory attribute flags of the named output section. |
| `/MANIFEST[:NO\|:EXTERNAL\|:EMBED[,ID=n]]` | `/MANIFEST` writes `<out>.manifest` (or the `/MANIFESTFILE` path); `:EMBED` embeds an `RT_MANIFEST` resource (ID 1 for EXE, 2 for DLL, language 0x409). `/MANIFESTINPUT:<file>` supplies the content and implies embedding; otherwise an `asInvoker` manifest is generated. Embedding fails if an input already has a `.rsrc` section (resource trees are not merged). |
| `/VERBOSE` | Prints each loaded input, the final section layout, and the output path to stdout. |
| `/DEBUG`, `/PDB:<file>` | See the debug-information section. |

`.res` inputs are still not implemented; resources supplied as COFF `.rsrc` sections (for example from `cvtres`) pass through to the PE resource directory.

LTCG/LLVM bitcode is not linked:
