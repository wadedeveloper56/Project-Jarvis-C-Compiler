
3. Sequential Agent Mode Prompts

Once you approve the plan, start a New Thread in Copilot Chat for each phase below so Copilot builds and verifies all three projects incrementally with MSBuild.

Phase 1: Three-Project MSBuild Scaffolding, Public Library API, CLI & Test Harness

```
#jwcc2 #jwcc2lib #jwcc2test
Execute Phase 1 of the jwcc2 plan:
1. Create `jwcc2lib/jwcc2lib.vcxproj` (StaticLibrary), `jwcc2/jwcc2.vcxproj` (Application referencing `jwcc2lib`), and `jwcc2test/jwcc2test.vcxproj` (Application referencing `jwcc2lib`), plus `.vcxproj.filters` files. Configure all three for C++20 (`/std:c++20`, `/W4`, `/WX`, `x64` Debug/Release) with zero external repo dependencies.
2. In `jwcc2lib`, implement `SourceManager`, `DiagnosticEngine`, `TargetInfo` (`-m16`, `-m32`, `-m64` sizes, alignments, pointer widths, and calling conventions), and the public `CompilerDriver` API in `jwcc2lib/include/jwcc2/driver/CompilerDriver.hpp`.
3. In `jwcc2`, implement only `jwcc2/src/main.cpp` to parse CLI flags (`-m16`, `-m32`, `-m64`, `-o <file>`, `--emit-ast`, `--emit-ir`, `-h/--help`) and delegate execution to `jwcc2lib::CompilerDriver`.
4. In `jwcc2test`, implement a lightweight test runner in `jwcc2test/src/main.cpp` with initial unit tests for `TargetInfo` and `DiagnosticEngine`.
5. Build all three projects with MSBuild and run `jwcc2test.exe` to verify zero warnings or errors.
```

Phase 2: C99 Preprocessor & Lexer (jwcc2lib + jwcc2test)

```
#jwcc2lib #jwcc2test
Execute Phase 2 of the jwcc2 plan:
1. In `jwcc2lib`, implement the C99 Lexer supporting all C99 keywords, identifiers, integer/float literals (including hex floats and suffixes), character/string literals, punctuators, and source spans.
2. In `jwcc2lib`, implement the C99 Preprocessor supporting `#define` (object-like and function-like macros, `#`, `##`, `__VA_ARGS__`), `#undef`, `#include` (quote and angle-bracket search paths), `#if`/`#ifdef`/`#ifndef`/`#elif`/`#else`/`#endif`, `#line`, `#error`, `#pragma`, and predefined target macros (`__JWCC2__`, `__16BIT__`/`__32BIT__`/`__64BIT__`).
3. In `jwcc2test`, add comprehensive unit tests verifying macro expansion, variadic macros, conditional compilation, and C99 tokenization.
4. Build `jwcc2lib` and `jwcc2test` via MSBuild and run `jwcc2test.exe` to verify all tests pass.
```

Phase 3: C99 AST, Parser & Semantic Analyzer (jwcc2lib + jwcc2test)

```
#jwcc2lib #jwcc2test
Execute Phase 3 of the jwcc2 plan:
1. In `jwcc2lib`, implement `ArenaAllocator` and C99 AST nodes for declarations, types, statements, and expressions, plus an AST printer for `--emit-ast`.
2. In `jwcc2lib`, implement a C99 Parser (recursive descent for declarations/statements, Pratt parser for expressions) supporting typedef name disambiguation, designated initializers, compound literals, structs/unions/enums, variadic functions, and C99 control flow.
3. In `jwcc2lib`, implement `Sema` (Semantic Analyzer & Type Checker) enforcing C99 integer promotions, usual arithmetic conversions, pointer arithmetic scaling via `TargetInfo`, struct/union field offset and padding calculation per target architecture, and constant expression evaluation.
4. In `jwcc2test`, add unit tests validating AST construction, type checking, and struct layout differences across `-m16`, `-m32`, and `-m64`. Build and run `jwcc2test.exe`.
```

Phase 4: Custom Backend IR & ABI Lowering (jwcc2lib + jwcc2test)

```
#jwcc2lib #jwcc2test
Execute Phase 4 of the jwcc2 plan:
1. In `jwcc2lib`, implement the custom backend IR (`IRModule`, `IRFunction`, `BasicBlock`, `IRInst`, `IRValue`, and `IRType` covering `i8`, `i16`, `i32`, `i64`, `f32`, `f64`, `ptr16`, `ptr32`, `ptr64`, `void`) and an IR printer for `--emit-ir`.
2. Implement `ASTToIRLowerer` in `jwcc2lib` to lower validated C99 AST functions, local stack allocations, control flow, struct/array indexing, and global variables into custom IR.
3. Implement an ABI & Legalization pass in `jwcc2lib` that adapts function calls, return values, and oversized types (such as 32-bit/64-bit operations on 16-bit targets, or Windows x64 shadow space and `RCX/RDX/R8/R9` register parameters) for the active `TargetInfo`.
4. In `jwcc2test`, add unit tests verifying IR generation and ABI lowering across `-m16`, `-m32`, and `-m64`. Build and run `jwcc2test.exe`.
```

Phase 5: 16-, 32-, and 64-Bit MASM Emitters & Assembler Integration Tests (jwcc2lib + jwcc2test)

```
#jwcc2 #jwcc2lib #jwcc2test
Execute Phase 5 of the jwcc2 plan:
1. In `jwcc2lib`, implement register allocation (linear scan with spill-to-stack support) parameterized by each target's register file (`AX/BX/CX/DX/SI/DI/BP/SP` for 16-bit, `EAX..EDI` for 32-bit, `RAX..R15` + `XMM0..XMM15` for 64-bit).
2. In `jwcc2lib`, implement three isolated MASM emitters:
   - `Masm16Emitter`: Emits `.8086`/`.286`, `.MODEL SMALL`, `.STACK`, `.DATA`, `.CODE`, `NEAR`/`FAR` procs, `BP`-relative frames, and 16-bit addressing.
   - `Masm32Emitter`: Emits `.686P`, `.XMM`, `.MODEL FLAT, C`, `.DATA`, `.CODE`, `EBP` frames, and `cdecl` prologue/epilogue.
   - `Masm64Emitter`: Emits `ml64.exe`-compliant MASM (no `.MODEL`), `.DATA`, `.CODE`, `PROC FRAME` / `ENDP`, Windows x64 32-byte shadow space, 16-byte stack alignment, and `RIP`-relative global access.
3. In `jwcc2test`, add end-to-end integration tests that compile C99 test programs via `jwcc2lib` under `-m16`, `-m32`, and `-m64` and invoke `ml.exe` and `ml64.exe` to verify clean assembly.
4. Build `jwcc2lib`, `jwcc2`, and `jwcc2test` via MSBuild and run `jwcc2test.exe` to verify all unit and integration tests pass.
```
