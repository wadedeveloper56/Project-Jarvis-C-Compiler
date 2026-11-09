# JWlink2 Missing Features for JWlink Compatibility

This is the working checklist for making JWlink2 a practical drop-in replacement for JWlink in native Windows AMD64 executable builds. It is not a claim of full compatibility with every JWlink target or every Microsoft `link.exe` feature. First capture the repository's real JWlink command lines and input artifacts; use those as the compatibility baseline and expand this list when they reveal gaps.

An item is complete only when implemented, covered by a regression test, and checked against the corresponding JWlink behavior. Keep unsupported behavior explicit rather than silently accepting an option without honoring it.

## Compatibility baseline

- [x] Inventory actual JWlink command lines, response files, object producers, libraries, and output types used by this repository; findings are recorded below.
- [x] Define and test the supported compatibility boundary; document intentionally unsupported JWlink targets below.
- [x] Add an end-to-end JWlink/JWlink2 comparison that links the same representative AMD64 COFF input and verifies observable output behavior rather than byte-for-byte identity.

`RepresentativeObjectMatchesJWlinkPeBehavior` in `JWlink2Test` links the same single-section AMD64 COFF object, containing a return instruction and a second marker byte, with both x64 linker executables. It checks that both outputs are AMD64 PE32+ console executables, are not DLLs, and preserve the entry-point instruction and section contents. The test deliberately does not compare image bytes or layout. The solution declares both linkers as test-project build dependencies so the comparison uses repository-built JWlink and JWlink2 binaries.

### Supported compatibility boundary

JWlink2's target is a native Windows **AMD64 (x64) PE32+ executable**. It accepts AMD64 COFF object files and the documented Microsoft library-member formats and emits an executable image using the console or Windows subsystem. This is the output target regardless of whether the JWlink2 tool itself was built for Win32 or x64. `/DLL` is not supported. An AMD64 machine field, PE32+ optional header, non-DLL executable characteristic, and default console subsystem are asserted by `ValidAmd64ObjectProducesPe32PlusWithDefaultBaseAndEntry`; unsupported DLL output and non-AMD64 input/machine requests are checked by `DllOutputIsOutsideSupportedBoundary`, `NonAmd64ObjectIsRejected`, and `CommandLineAndResponseFileErrorsAreReported` in `JWlink2Test`.

Intentionally out of scope as JWlink output targets are 16-bit DOS COM/MZ and Windows NE, OS/2 NE/LE/LX, Windows VxD, Phar Lap, Novell NLM, DOS/16M, QNX, ELF, raw BIN/HEX, and non-AMD64 Windows images. Windows PE DLLs and drivers are also outside this boundary. These are JWlink's broader formats, not compatibility promises for JWlink2. LTCG inputs and full runtime startup remain separate input/runtime limitations, even when the desired output is an AMD64 PE executable.

### Repository inventory (2026-11-09)

The only executable JWlink build invocation found in the repository is in `test\run.bat`. It links the 64-bit Windows test as follows:

```text
JWlink format win pe option map=<test1.map> file <test1.obj>, <c064.obj> lib <shell32.lib> <kernel32.lib> <msvcrt.lib> name <test1.exe>
```

The batch file supplies absolute toolchain paths through `%DIRECTORY%`, `%LIB64%`, `%KLIB64%`, and `%LLIB64%`. The SDK libraries are under Windows Kits `10.0.28000.0\um\x64`; the C runtime library is under MSVC `14.53.36911\lib\x64`. `test\link64.lnk` contains the equivalent JWlink directive-file form (including `format win pe`, the map, both objects, the same libraries, and the executable name), but `test\run.bat` does not pass that file to JWlink. The active harness uses inline directives; it does not invoke an `@response-file`. `test\info.txt` documents JWlink's `@` directive, while `test\info2.txt` describes it for a linker-emulator command line.

The inputs are produced by this path in `test\run.bat`:

1. `JWcc -b 64` compiles `test\test1.c` to `test1.asm`.
2. `JWasm -win64` assembles that output to `test1.obj`.
3. `JWasm -win64` separately assembles the hand-written AMD64 startup source `test\c064.asm` to `c064.obj`.
4. The checked-in PEDump logs for both objects identify them as 64-bit AMD64 COFF. `c064.obj` also contains a `.drectve` section specifying `-entry:mainCRTStartup`, so honoring compiler/assembler directives is part of this real link case.

The libraries passed, in order, are Windows SDK `shell32.lib`, Windows SDK `kernel32.lib`, and MSVC `msvcrt.lib`. The requested outputs are a Windows PE executable `test1.exe` and its JWlink map `test1.map`; the batch then asks PEDump to inspect the two objects and executable. `format win pe` is the only output format used by this checked-in workflow; no DLL, 32-bit, DOS, OS/2, ELF, raw, or other JWlink output invocation was found. Although the batch creates 16/32-bit output directories and declares 32-bit library variables, its compiler, assembler, linker, and inspection commands in this workflow target only `windows\64bit`.

This is an inventory of checked-in repository usage, not a claim that no developer-local or external build scripts use other JWlink features. The discovered `.drectve` entry point is already tracked by the COFF-directive item below; the inventory does not imply JWlink2 currently supports it.

## Input objects and libraries

- [x] Process COFF `.drectve` directives, including `/DEFAULTLIB`, `/NODEFAULTLIB`, `/ALIGN`, `/ALIGNCOMM`, `/ENTRY`, `/EXPORT`, and `/INCLUDE`; verify entry-point behavior against JWlink and test archive selection, PE export data, alignment, and malformed directives.
- [x] Support Microsoft AMD64 BigObj inputs, including 32-bit section counts/indices, 20-byte symbol and auxiliary records, and BigObj members in static archives; validate wide section indices, relocations, COMDAT selection, and malformed signatures in `JWlink2Test`.
- [x] Define handling for LTCG/LLVM bitcode inputs: JWlink2 does not invoke an LTO backend and explicitly rejects raw/wrapped LLVM bitcode, recognized embedded LLVM bitcode/LTO COFF sections, and detectable bitcode archive members with a rebuild-without-LTO diagnostic; cover each form in `JWlink2Test`. Proprietary MSVC `/GL` objects are outside the accepted format and may receive the generic COFF validation error because no guaranteed public marker is available.
- [x] Parse Microsoft archive linker-member symbol indexes for indexed extraction, while retaining fallback scanning for archives without indexes; validate real MSVC-generated AMD64 `.lib` archives and malformed index diagnostics in `JWlink2Test`.
- [?] Audit weak externals, common symbols, COMDAT selection, associative sections, duplicate resolution, and archive extraction against JWlink using compiler-generated fixtures.

  Partial progress: `JWlink2Test` now covers weak fallbacks, common allocation/precedence, COMDAT selection, associative sections, duplicate strong definitions, and archive extraction. Differential checks currently cover representative PE output and COMDAT ANY selection. The MSVC-indexed archive fixture is accepted by JWlink2 but crashes the legacy JWlink, so it remains a JWlink2-only test. This item remains open: the repository's JWcc and the available MSVC compiler have not yielded minimal object inputs that both linkers can consume reliably, and some handcrafted weak/archive fixtures crash or are rejected by JWlink. Do not treat the synthetic JWlink2-only cases as compiler-generated compatibility evidence.

## Windows image generation

- [ ] Support DLL output, including exports, `.def` files, `/EXPORT`, import-library generation, and DLL entry-point behavior.
- [ ] Preserve and correctly emit required runtime metadata and directories, including AMD64 exception/unwind tables, TLS, load configuration, and resources where required by baseline inputs.
- [ ] Support delay-loaded imports and their associated PE metadata when used by existing builds.
- [ ] Add debug information support, including CodeView object data and the expected PDB output workflow.
- [ ] Implement application manifests and resource inputs if required by existing builds.
- [ ] Validate PE headers, section flags/alignment, data directories, and relocation behavior with Windows loaders and standard PE inspection tools.

## Command-line compatibility

- [ ] Inventory JWlink options used by real builds and implement their semantics; maintain a tested accepted/unsupported option matrix.
- [ ] Add commonly used missing controls as required by that inventory, such as `/INCLUDE`, `/WHOLEARCHIVE`, `/DLL`, `/DEF`, `/EXPORT`, `/IMPLIB`, `/DEBUG`, `/PDB`, `/MANIFEST`, `/DELAYLOAD`, `/SECTION`, `/MERGE`, `/STACK`, `/HEAP`, and `/VERBOSE`.
- [ ] Match option precedence, quoting, path resolution, response-file nesting/encoding, environment-variable expansion, and library search order where JWlink relies on them.
- [ ] Match relevant defaults and diagnostics: output naming, entry point, subsystem, default libraries, duplicate/unresolved symbol reporting, and exit codes.

## Runtime and integration

- [ ] Link representative C and C++ programs with the startup objects and runtime libraries used by this repository; support required CRT initialization, constructors, TLS callbacks, and termination behavior.
- [ ] Verify compatibility with the repository's compiler, assembler, librarian, and build scripts without requiring source or command-line changes.
- [ ] Add a migration gate: JWlink2 becomes the default only after the baseline builds and runtime tests pass; retain JWlink as a fallback until then.

## Progress notes

- Current documented support: AMD64 standard COFF and BigObj files (including BigObj archive members and wide section indices), explicit rejection of detectable LTCG/LLVM bitcode inputs, `.drectve` handling for default libraries, default alignment, entry points, exports, and forced includes; Microsoft archive symbol-index parsing and selective extraction with fallback scanning for unindexed archives; Microsoft archives containing regular AMD64 objects and short import objects; static archive extraction; PE32+ executables; import/IAT and export-directory generation; selected AMD64 relocations; base relocations; COMDATs; weak externals; map files; response files; and a limited set of linker options. BigObj, unsupported LTO forms, archive-indexed extraction, and `.drectve` behavior are covered by `JWlink2Test`, including JWlink comparisons for output and entry-point behavior. See [README.md](README.md) for details.
- LTCG/LLVM bitcode is explicitly unsupported (recognized bitcode inputs produce a targeted error; proprietary MSVC `/GL` objects may receive a generic COFF validation error); debug data, full C/C++ runtime startup, DLL output, and many advanced linker options are also unsupported.
- Update this checklist alongside feature work and tests; move an item out of the open list only after its acceptance criteria are met.
