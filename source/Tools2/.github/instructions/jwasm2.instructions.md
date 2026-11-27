---
applyTo: "jwasm2/**"
---
# Project Specification: jwasm2 Multi-Target MASM Assembler (Greenfield C++20)

## Solution Coexistence & Single-Root Isolation Rules
- **Multi-project solution awareness:** This Visual Studio 2026 solution contains other pre-existing project directories outside `jwasm/`. **Do not read, modify, delete, or reference any files, headers, libraries, or build targets outside the `jwasm/` directory.**
- **Strict `jwasm2/` directory scope:** Confine all file creation, edits, searches, and builds strictly inside `jwasm/`:
  1. `jwasm2/jwasm2/` (Console Executable): Contains **only** `main.cpp` with `int main(int argc, char* argv[])`, forwarding immediately to `jwasm2lib::Driver::run(...)`. Zero assembler, parser, encoder, or file-emitting logic belongs here.
  2. `jwasm2/jwasm2lib/` (Static Library): Contains 100% of the C++20 assembler implementation, ISA tables, emitters, and public headers directly under `jwasm/jwasm2lib/include/`, with source files under `jwasm/jwasm2lib/src/`.
  3. `jwasm2/jwasm2test/` (Unit & Integration Test Project): Links **only** against `jwasm2lib` (plus the test framework), includes headers from `jwasm/jwasm2lib/include/`, and tests all assembler functionality.
  4. `jwasm2/tools/` and `jwasm2/data/`: Dedicated subdirectories inside `jwasm/` for `tablegen.py` and ISA definition files so they never collide with sibling projects.
- **Greenfield implementation:** Write all `jwasm2*` code completely from scratch in modern C++20 (`/std:c++20`, `/W4`, `/permissive-`). Do not copy or adapt code from sibling project folders outside `jwasm/`.
- **Targeted builds:** When verifying builds via terminal or MSBuild, build only `jwasm2`, `jwasm2lib`, and `jwasm2test` rather than rebuilding unrelated solution projects.

## Target Directory Layout
```
jwasm2/
├── tools/
│   └── tablegen.py          # Compiles ISA definitions into constexpr C++ tables
├── data/
│   └── x86_isa.def          # 8086..x64, MMX, 3DNow!, SSE..SSE4.2, AVX, VMX
├── jwasm2/                  # Console App project (only main.cpp)
│   └── main.cpp
├── jwasm2lib/               # Static Library project (all core logic)
│   ├── include/             # Public headers (driver.hpp, options.hpp, diagnostics.hpp, isa_table.hpp, etc.)
│   └── src/                 # Implementation .cpp files and generated_isa.inc
└── jwasm2test/              # Unit & Integration Test project (links only jwasm2lib)
    └── ...
```

## Numbered Error & Warning Diagnostic System (`diagnostics.hpp/.cpp`)
- Implement a centralized diagnostic engine (`DiagnosticEngine`) used across all assembly stages (CLI, Lexer, Preprocessor, Parser, Semantic Analyzer, Encoder, Layout/Relaxer, and Object/Map Emitters).
- Every diagnostic must have a **strongly typed numeric code** (`enum class DiagCode : uint16_t`), severity (`Fatal`, `Error`, `Warning`, `Note`), source location (`file`, `line`, `column`, plus macro expansion call stack), and formatted Visual Studio/MASM-compatible output:
  - Format: `<file>(<line>,<col>): error A2006: undefined symbol : foo` and `<file>(<line>,<col>): warning A4001: operand size mismatch`
- **Reserved Diagnostic Number Ranges:**
  - `A1000–A1999`: **Fatal & System Errors** (file I/O failures, invalid CLI options, circular `INCLUDE` depth exceeded, internal assembler limit).
  - `A2000–A2999`: **Syntax, Semantic & Encoding Errors** (syntax errors, undefined/redefined symbols, invalid operand combinations, instruction disallowed by active CPU directive, illegal `REX` + `AH/BH/CH/DH` register usage, jump displacement out of range, segment/bitness mismatch).
  - `A3000–A3999`: **Object & Map Emitter Errors** (16-bit OMF segment size > 64KB overflow, invalid COFF relocation target, `.pdata`/`.xdata` unwind generation failure).
  - `A4000–A4999`: **Assembler Warnings** (truncation of immediate values, redundant prefixes, unreachable code after `JMP`/`RET`, implicit operand size assumptions, deprecated 16-bit/3DNow! constructs).
  - `A5000–A5999`: **Informational Notes** (macro expansion tracebacks, multi-pass branch relaxation adjustments).
- **Diagnostic Controls & Testability:**
  - Support CLI flags `-W0`..`-W4` (warning levels), `-WX` (treat warnings as errors), and `-wd<num>` (disable specific warning code).
  - Store structured `Diagnostic` objects in memory so `jwasm2test` can assert exact `DiagCode` numbers, line numbers, and error counts without parsing raw stdout strings.

## MASM Syntax & CPU Architecture Support
- **Input syntax:** MASM v8+ Intel syntax:
  - Segments & directives: `.MODEL`, `.DATA`, `.DATA?`, `.CONST`, `.CODE`, `SEGMENT`/`ENDS`, `GROUP`, `ASSUME`, `OPTION`, `ALIGN`, `EVEN`, `PUBLIC`, `EXTERN`/`EXTRN`.
  - Procedures & high-level flow: `PROC`/`ENDP` (`PUBLIC`, `PRIVATE`, `USES`, and 64-bit `FRAME`), `PROTO`, `INVOKE`, `LOCAL`, `.IF`/`.ELSEIF`/`.ELSE`/`.ENDIF`, `.WHILE`/`.ENDW`, `.REPEAT`/`.UNTIL`.
  - Types & data: `STRUCT`/`UNION`/`ENDS`, `RECORD`, `TYPEDEF`, `DB`, `DW`, `DD`, `DQ`, `TBYTE`, `OWORD`, `YWORD`, `DUP`.
  - Preprocessor & macros: `INCLUDE`, `EQU`, `=`, `TEXTEQU`, `MACRO`/`ENDM` (`LOCAL`), `REPEAT`, `FOR`/`FORC`, conditional assembly (`IF`, `IFE`, `IFDEF`, `IFNDEF`, `IFB`, `IFNB`, `IFIDNI`).
- **CPU gating directives and target bitness (`-16`, `-32`, `-64`):**
  - **Base x86 (16-bit & 32-bit):** `.8086`, `.186`, `.286`/`.286P`, `.386`/`.386P`, `.486`/`.486P`, `.586`/`.586P`, `.686`/`.686P` (8086 through Pentium Pro).
  - **x86-64 (64-bit):** Full 64-bit instruction set matching MASM's `ML64`.
  - **SIMD & Vector Extensions:**
    - `.MMX` and `.K3D` (MMX & 3DNow! `0Fh 0Fh` + imm8 suffix opcodes)
    - `.XMM` (SSE, SSE2, SSE3, SSSE3, SSE4.1, and SSE4.2)
    - AVX (VEX.128 and VEX.256 scalar and vector instructions)
  - **Virtualization:** Intel VMX (`VMXON`, `VMXOFF`, `VMCALL`, `VMLAUNCH`, `VMRESUME`, `VMPTRLD`, `VMPTRST`, `VMCLEAR`, `VMREAD`, `VMWRITE`, `INVEPT`, `INVVPID`).

## Data-Driven Encoder & Output Formats
- **Table-driven encoder:** Use compact `constexpr` lookup tables generated by `jwasm/tools/tablegen.py` from `jwasm/data/x86_isa.def`.
- **Full x86/x64 encoding pipeline:**
  - Legacy prefixes (`LOCK`, `REP`/`REPE`/`REPNE`, segment overrides `2Eh`/`36h`/`3Eh`/`26h`/`64h`/`65h`, operand-size `66h`, address-size `67h`).
  - `REX` prefixes (`40h–4Fh`) for 64-bit operand size (`REX.W`) and extended registers (`R8–R15`, `XMM8–XMM15`, `SIL`/`DIL`/`BPL`/`SPL`), emitting error `A2xxx` on illegal `REX` + `AH`/`BH`/`CH`/`DH` combinations.
  - 2-byte (`C5h`) and 3-byte (`C4h`) `VEX` prefixes for AVX (`VEX.L`, `VEX.pp`, `VEX.mmmmm`, `VEX.W`, `vvvv` operand slot).
  - `ModR/M` + `SIB` encoding for 16-bit addressing modes, 32/64-bit SIB addressing, and 64-bit RIP-relative addressing.
  - Multi-pass layout engine with iterative branch displacement relaxation (`rel8` vs. `rel16`/`rel32`).
- **Binary Object & Map Outputs:**
  - **`-16` (16-bit OMF `.obj`):** Emit valid Intel 16-bit OMF records (`THEADR`, `LNAMES`, `SEGDEF`, `GRPDEF`, `PUBDEF`, `EXTDEF`, `LEDATA`, `FIXUPP`, `MODEND`) with valid checksums.
  - **`-32` & `-64` (32/64-bit PE/COFF `.obj`):** Emit valid Microsoft PE/COFF `.obj` files (`IMAGE_FILE_MACHINE_I386` / `IMAGE_FILE_MACHINE_AMD64`), section headers, relocations, symbol/string tables, and `.pdata`/`.xdata` unwind info for 64-bit `PROC FRAME`.
  - **`-map <file>` (Optional `.map` output):** Emit a linker-style `.map` report with segment/section start addresses, lengths, classes, public symbols sorted both by name and by address, and entry point RVA.
