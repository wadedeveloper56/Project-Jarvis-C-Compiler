# Multi-Target Linker Development Guidelines (jwlink2 - C++20)

## Clean-Room & Solution Architecture Rules
- **Build From Scratch:** You are implementing **jwlink2** completely from scratch as a greenfield C++20 codebase. **Ignore and do not read, reference, include, or reuse any legacy or existing source files** elsewhere in the workspace outside of `jwlink2/`, `jwlink2lib/`, and `jwlink2test/`.
- **Strict 3-Project Separation:**
  1. **`jwlink2lib` (Static Library):** Contains 100% of the linker engine—IR data structures, numbered diagnostic engine, binary readers/writers, OMF/COFF `.obj` and `.lib` parsers, symbol resolver, layout engines, flat/MZ/NE/PE32/PE32+ emitters, import `.lib` generators, and CLI option parsing (`LinkerDriver::run(std::span<const std::string_view>)`).
  2. **`jwlink2` (Console Executable):** Thin entry-point wrapper only. Contains **only `main(int argc, char* argv[])`**, which forwards command-line arguments into `jwlink2lib` and returns its exit code. Zero linker logic is permitted in `jwlink2`.
  3. **`jwlink2test` (Test Executable):** Unit and integration test suite linking solely against `jwlink2lib`.

## Project Scope
- **Inputs:** 16-bit Intel OMF (`.obj`, `.lib`), 32-bit/64-bit Microsoft COFF (`.obj`, `.lib` archive format).
- **Outputs:** 
  1. **Flat Raw Binaries (16-bit, 32-bit, 64-bit)** (`.bin`, `.com`, raw boot/kernel images)
  2. **16-bit DOS MZ Executable** (`.exe`)
  3. **16-bit Windows NE (New Executable)** `.exe` (**GUI & Console/WindowCompat**), `.dll`, and **16-bit Device Driver (`.drv`)** + **OMF Import Library (`.lib`)**
  4. **32-bit Windows PE32** `.exe` (**GUI & Console**), `.dll`, and **32-bit Kernel Device Driver (`.sys`)** + **COFF Import Library (`.lib`)**
  5. **64-bit Windows PE32+** `.exe` (**GUI & Console**), `.dll`, and **64-bit Kernel Device Driver (`.sys`)** + **COFF Import Library (`.lib`)**

## Numbered Diagnostic System (`DiagnosticEngine`)
- **No Raw Console Output in `jwlink2lib`:** Never write directly to `std::cerr` or `std::cout` inside library passes. Route all diagnostics through a central `DiagnosticEngine` stored in `LinkerContext` so both CLI runs and `jwlink2test` unit tests can inspect structured diagnostic records (`Severity`, `DiagCode`, `message`, `source_file`, `module_name`, `symbol_name`).
- **Standard Output Format:** Format emitted diagnostics consistently as:
  `jwlink2 : <fatal error|error|warning> JWL<4-digit-code>: <message> [in <module/file>]`
- **Numbered Code Ranges:**
  - **`JWL1000–JWL1999` (CLI, I/O & General):** `JWL1001` unknown CLI switch, `JWL1002` missing option argument, `JWL1003` no input files specified, `JWL1004` cannot open input file, `JWL1005` cannot write output file, `JWL1006` invalid `.def` file syntax, `JWL1007` conflicting target/subsystem options.
  - **`JWL2000–JWL2999` (Object & Archive Parsing):** `JWL2001` unrecognized object/archive magic header, `JWL2002` truncated record or section, `JWL2003` invalid OMF record checksum, `JWL2004` unsupported COFF machine architecture, `JWL2005` corrupt archive member header, `JWL2006` unsupported relocation type.
  - **`JWL3000–JWL3999` (Symbol Resolution & Exports):** `JWL3001` unresolved external symbol, `JWL3002` multiply defined symbol, `JWL3003` unresolved entry point symbol, `JWL3004` exported symbol not found, `JWL3005` duplicate export ordinal, `JWL3006` dynamic import disallowed in flat binary or DOS MZ target, `JWL3007` machine mismatch between input object and `/MACHINE:` target.
  - **`JWL4000–JWL4999` (Layout, Segment & Relocation Math):** `JWL4001` 16-bit segment exceeds 64KB (`65536` bytes), `JWL4002` `DGROUP` exceeds 64KB, `JWL4003` relocation target out of range (`REL8`/`REL16`/`REL32` overflow), `JWL4004` unaligned base address or section alignment, `JWL4005` flat binary `.com` exceeds 64KB (`0xFF00` bytes).
  - **`JWL5000–JWL5999` (Format Emitters & Import Libraries):** `JWL5001` NE segment table limit exceeded (`>254` segments), `JWL5002` NE entry ordinal out of range, `JWL5003` PE header size overflow, `JWL5004` missing `DriverEntry` or `DriverProc` for `/DRIVER`, `JWL5005` import library generation failure.
- **Severity & CLI Controls:** Support `Warning`, `Error`, and `Fatal` severities, `/WX` (treat all warnings as errors), and `/NOWARN:<code_number>` (suppress specific warning codes).

## Coding & Memory Standards
- **Language Standard:** C++20 (`/std:c++20` with `/W4 /WX` warning levels).
- **Fixed-Width Types:** Strictly use `<cstdint>` types (`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`) for all file offsets, RVAs, segment selectors, and binary fields. Never use `long` or platform-dependent `int` sizes for binary structures.
- **Struct Packing & Alignment:** Wrap all on-disk binary header structs (DOS MZ, NE, COFF, PE32, PE32+, OMF records) in `#pragma pack(push, 1)` and `#pragma pack(pop)`. Validate every header struct size with `static_assert(sizeof(...) == ...)` at compile time.
- **Endianness:** Explicitly write little-endian serialization helpers (`write_u16_le`, `write_u32_le`, `write_u64_le`), plus big-endian helpers (`write_u32_be`) specifically required for COFF `.lib` archive linker member offsets.
- **Zero External Dependencies:** Rely solely on the C++20 Standard Library (`<vector>`, `<span>`, `<filesystem>`, `<variant>`, `<unordered_map>`, `<bit>`). Do not introduce third-party libraries.

## Binary Format, Subsystem & Device Driver Rules
- **Subsystems & Driver Flags (`/SUBSYSTEM:CONSOLE`, `/SUBSYSTEM:WINDOWS`, `/SUBSYSTEM:NATIVE`, `/DRIVER`):**
  - **PE32 & PE32+ EXEs/DLLs:** Set `OptionalHeader.Subsystem` to `IMAGE_SUBSYSTEM_WINDOWS_GUI` (`2`) for GUI mode or `IMAGE_SUBSYSTEM_WINDOWS_CUI` (`3`) for Console mode. If `/ENTRY:` is omitted, infer `WinMainCRTStartup`/`wWinMainCRTStartup`/`WinMain` (GUI), `mainCRTStartup`/`wmainCRTStartup`/`main` (Console), or `_DllMainCRTStartup`/`DllMain` (`/DLL`).
  - **32-bit & 64-bit Windows Kernel Drivers (`/DRIVER` or `/SUBSYSTEM:NATIVE`, `.sys`):**
    - Set `OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_NATIVE` (`1`) and `DllCharacteristics |= IMAGE_DLLCHARACTERISTICS_WDM_DRIVER` (`0x2000`) when `/DRIVER:WDM` is passed.
    - **Mandatory PE Checksum:** Compute and write the exact 32-bit Windows PE checksum (`OptionalHeader.CheckSum`) using the standard 16-bit carry-folded sum + file length algorithm.
    - **Default Entry Point:** Infer `GsDriverEntry` or `DriverEntry` (`_DriverEntry@8` on x86) when `/ENTRY:` is omitted.
    - **Section & Relocation Rules:** Preserve `.reloc` base relocations, mark `INIT` sections as `IMAGE_SCN_MEM_DISCARDABLE` (`0x02000000`), and support kernel imports (`ntoskrnl.exe`, `hal.dll`) via `.idata`.
  - **16-bit Windows NE EXEs, DLLs & Device Drivers (`.drv`):**
    - Encode GUI vs. Console mode in NE Program Flags (`ne_flags` bits `0x0700`): `0x0300` (`WINDOWAPI`) for GUI mode and `0x0200` (`WINDOWCOMPAT`) for Console/character-mode apps.
    - For **16-bit Windows Device Drivers (`/DRIVER` on `NE16`, `.drv`)**: set the NE Library/Driver module flag (`0x8000` in `ne_flags`) with `SINGLEDATA` (`0x0001`), mark interrupt/driver code and data segments as fixed (`~MOVEABLE`) and `PRELOAD` (`0x0040`), and export `DriverProc` / `WEP` in the Entry and Resident Name Tables.
- **Flat Raw Binaries (`FLAT16`, `FLAT32`, `FLAT64`):**
  - Emit raw machine code and initialized data with **zero file headers** and **zero runtime relocation tables**.
  - Resolve all symbol addresses and static relocations directly against `/BASE:<addr>` (defaulting to `0x0100` for `FLAT16` and `0x0000` for `FLAT32`/`FLAT64`).
  - Reject any unresolved external imports (`JWL3006`) and materialize interior `.bss` sections as zero-filled bytes while omitting trailing `.bss` padding unless `/PADBSS` is set.
- **COFF & OMF Archives (`.lib` Input & Output):**
  - Parse and emit COFF `!<arch>\n` short-import `.lib` files and OMF page-aligned `IMPDEF` (`0xA0`) `.lib` files when building `/DLL`, `.drv`, or `/IMPLIB:<file>`.
- **16-bit DOS MZ:** Enforce 16-byte paragraph alignment for header size (`e_cparhdr`) and segment load image, emitting `segment:offset` relocation pairs.

## Verification & Testing Workflow
- Build and run `jwlink2test` after every phase, including negative tests that assert exact `JWLxxxx` error and warning codes.
- Verify PE32/PE32+ GUI/Console EXEs, DLLs, `.sys` kernel drivers (including PE header checksum validation), and COFF `.lib` import libraries generated by `jwlink2` using `dumpbin /headers /exports /imports /archivemembers`.
- Never leave stubbed `TODO` placeholders in relocation math, archive indexing, diagnostic emission, or checksum routines.
