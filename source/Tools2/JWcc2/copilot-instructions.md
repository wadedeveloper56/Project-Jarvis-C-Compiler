---
applyTo: "jwcc2/**"
---
# jwcc2 Compiler Engineering Standards

## Clean-Room & Solution Architecture Rules
- **Build From Scratch:** You are implementing **jwcc2** completely from scratch as a greenfield C++20 codebase. **Ignore and do not read, reference, include, or reuse any legacy or existing source files** elsewhere in the workspace outside of `jwcc2/`, `jwcc2lib/`, and `jwcc2test/`.
- **Strict 3-Project Separation:**
  1. **`jwcc2/jwcc2lib` (Static Library):** Contains 100% of the compiler engine.
  2. **`jwcc2/jwcc2` (Console Executable):** Thin entry-point wrapper only. Contains **only `main(int argc, char* argv[])`**, which forwards command-line arguments into `jwcc2lib` and returns its exit code. Zero compiler logic is permitted in `jwcc2`.
  3. **`jwcc2/jwcc2test` (Test Executable):** Unit and integration test suite linking solely against `jwcc2lib`.
- **Compiler Flags:** Configure all three `.vcxproj` files with `<LanguageStandard>stdcpp20</LanguageStandard>`, `<WarningLevel>Level4</WarningLevel>`, and `<TreatWarningAsError>true</TreatWarningAsError>`.

## C++20 Coding Conventions
- Use modern C++20 features (`std::span`, `std::string_view`, `std::variant`, `std::format`, `std::filesystem`, `concepts`, and `constexpr`).
- Avoid raw `new`/`delete`; manage AST and IR node lifetimes with arena allocators (`ArenaAllocator`) or `std::unique_ptr`.
- Prefer value semantics, `enum class`, and explicit error handling (`DiagnosticEngine` + `std::optional` / result types) over exceptions.

## Language & Target Specifications
- **Frontend (ISO C99):** Support C99 lexing, preprocessing, parsing, and semantic analysis (`_Bool`, `long long`, `inline`, `restrict`, designated initializers, compound literals, VLAs, and C99 integer promotion rules).
- **Target ABI & Data Model Abstraction (`TargetInfo`):** Parameterize all type sizes, alignments, and pointer widths by target mode (`-m16`, `-m32`, `-m64`):
  - **`-m16` (16-bit Real Mode):** `int` = 16-bit, `long` = 32-bit, near pointers = 16-bit (far pointers = 32-bit `seg:off`), stack alignment = 2 bytes.
  - **`-m32` (32-bit x86 Flat):** `int` = 32-bit, `long` = 32-bit, pointers = 32-bit, stack alignment = 4 bytes (`cdecl` default).
  - **`-m64` (64-bit x64 Windows):** `int` = 32-bit, `long` = 32-bit (LLP64), `long long` = 64-bit, pointers = 64-bit, stack alignment = 16 bytes, 32-byte shadow space, `RCX/RDX/R8/R9` register args.
- **Backend (Custom IR to MASM):** Lower validated AST into a target-aware SSA/three-address custom IR before instruction selection. Emit valid MASM syntax compatible with `ml.exe` (`/omf` for 16-bit, flat COFF for 32-bit) and `ml64.exe` (64-bit).

## Frontend & IR Rules
- **Input Standard:** ISO/IEC 9899:1999 (C99).
- **Preprocessor:** Support `#include`, object/function-like `#define`, `#ifdef`/`#ifndef`/`#if`/`#elif`/`#else`/`#endif`, and standard predefined macros (`__STDC_VERSION__=199901L`).
- **IR:** Lower the C99 AST into a target-independent Three-Address Code (TAC) IR with explicit type widths before code generation.

## Coding & Memory Standards
- **Language Standard:** Strict ISO C++20 (`/std:c++20` with `/W4 /WX` warning levels, use `std::variant`, `std::span`, `std::string_view`, `std::expected` if available, concepts, and smart pointers; avoid raw new/delete).
- **Zero External Dependencies:** Rely solely on the C++20 Standard Library (`<vector>`, `<span>`, `<filesystem>`, `<variant>`, `<unordered_map>`, `<bit>`). Do not introduce third-party libraries.

## Verification & Testing Workflow
- Build and run `jwcc2test` after every phase to confirm all unit and integration tests pass.
- Never leave stubbed `TODO` placeholders in relocation math, archive indexing, or checksum routines.
