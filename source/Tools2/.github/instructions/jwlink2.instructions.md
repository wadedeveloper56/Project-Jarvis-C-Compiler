---
applyTo: "jwlink2/**"
---
# Multi-Target Linker Development Guidelines (jwlink2 - C++20)

## Clean-Room & Solution Architecture Rules
- **Build From Scratch:** You are implementing **jwlink2** completely from scratch as a greenfield C++20 codebase. **Ignore and do not read, reference, include, or reuse any legacy or existing source files** elsewhere in the workspace outside of `jwlink2/`, `jwlink2lib/`, and `jwlink2test/`.
- **Strict 3-Project Separation:**
  1. **`jwlink2lib` (Static Library):** Contains 100% of the linker engine—IR data structures, binary readers/writers, OMF/COFF `.obj` and `.lib` parsers, symbol resolver, layout engines, flat/MZ/NE/PE32/PE32+ emitters, import `.lib` generators, and CLI option parsing (`LinkerDriver::run(std::span<const std::string_view>)`).
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
    - **Mandatory PE Checksum:** Compute and write the exact 32-bit Windows PE checksum (`OptionalHeader.CheckSum`) using the standard 16-bit carry-folded sum + file length algorithm (required by the Windows kernel loader).
    - **Default Entry Point:** Infer `GsDriverEntry` or `DriverEntry` (`_DriverEntry@8` on x86) when `/ENTRY:` is omitted.
    - **Section & Relocation Rules:** Preserve `.reloc` base relocations (do not strip relocations on `/DRIVER`), mark `INIT` sections as `IMAGE_SCN_MEM_DISCARDABLE` (`0x02000000`), and support kernel imports (`ntoskrnl.exe`, `hal.dll`) via `.idata`.
  - **16-bit Windows NE EXEs, DLLs & Device Drivers (`.drv`):**
    - Encode GUI vs. Console mode in NE Program Flags (`ne_flags` bits `0x0700`): `0x0300` (`WINDOWAPI`) for GUI mode and `0x0200` (`WINDOWCOMPAT`) for Console/character-mode apps.
    - For **16-bit Windows Device Drivers (`/DRIVER` on `NE16`, `.drv`)**: set the NE Library/Driver module flag (`0x8000` in `ne_flags`) with `SINGLEDATA` (`0x0001`), mark code/data segments containing interrupt service routines as fixed (`~MOVEABLE`) and `PRELOAD` (`0x0040`), and export `DriverProc` / `WEP` in the Entry and Resident Name Tables.
- **Flat Raw Binaries (`FLAT16`, `FLAT32`, `FLAT64`):**
  - Emit raw machine code and initialized data with **zero file headers** and **zero runtime relocation tables**.
  - Resolve all symbol addresses and static relocations directly against `/BASE:<addr>` (defaulting to `0x0100` for `FLAT16` and `0x0000` for `FLAT32`/`FLAT64`).
  - Reject any unresolved external imports and materialize interior `.bss` sections as zero-filled bytes while omitting trailing `.bss` padding unless `/PADBSS` is set.
- **COFF Archives (`.lib` Input & Output):**
  - Parse and emit `!<arch>\n`, even-byte member padding (`\n`), First Linker Member (`/` big-endian), Second Linker Member (`/` little-endian sorted index), Longnames Member (`//`), standard COFF object members, and `IMPORT_OBJECT_HEADER` (`0x0000`, `0xFFFF`) short imports.
  - When building a 32-bit or 64-bit `/DLL` (or `/IMPLIB:<file>`), emit a matching COFF import `.lib`.
- **OMF Objects & Libraries (`.obj`, `.lib` Input & Output):**
  - Parse record streams (`THEADR`, `LNAMES`, `SEGDEF`, `GRPDEF`, `PUBDEF`, `EXTDEF`, `LEDATA`, `LIDATA`, `FIXUPP`, `MODEND`) with 1-byte checksums, `LIBHEAD` (`0xF0`), page-aligned modules, and `LIBDIC` (`0xF1`).
  - When building a 16-bit NE `/DLL` or `.drv` (or `/IMPLIB:<file>`), emit a matching OMF import `.lib` with `THEADR`, `COMENT` class `0xA0` (`OMF_IMPDEF` subtype `0x01`), `MODEND`, and `LIBDIC`.
- **16-bit DOS MZ:** Enforce 16-byte paragraph alignment for header size (`e_cparhdr`) and segment load image, emitting `segment:offset` relocation pairs.

## Verification & Testing Workflow
- Build and run `jwlink2test` after every phase to confirm all unit and integration tests pass.
- Verify PE32/PE32+ GUI/Console EXEs, DLLs, `.sys` kernel drivers (including PE header checksum validation), and COFF `.lib` import libraries generated by `jwlink2` using `dumpbin /headers /exports /imports /archivemembers`.
- Never leave stubbed `TODO` placeholders in relocation math, archive indexing, or checksum routines.
