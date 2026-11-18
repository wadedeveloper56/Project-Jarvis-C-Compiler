Phase 1 Prompt: Project Scaffolding & Isolated Build Pipeline

Approved. Using .copilot/plans/jwlink2-architecture.md and .github/instructions/jwlink2.instructions.md Execute Phase 1 inside `jwasm2/` only (do not touch any sibling projects in the solution):
1. Create three Visual Studio 2026 C++20 (`/std:c++20`, `/W4`, `/permissive-`) projects and add them to the solution:
   - `jwasm2/jwasm2/` (Console App): Contains ONLY `main.cpp` calling `jwasm2lib::Driver::run(argc, argv)`.
   - `jwasm2/jwasm2lib/` (Static Library): Public headers in `jwasm2/jwasm2lib/include/` and sources in `jwasm2/jwasm2lib/src/`. Implement initial `driver.hpp/.cpp` and `options.hpp/.cpp` supporting `-16`, `-32`, `-64`, `-o <file>`, `-map <file>`, `-I <dir>`, `-D <sym>[=val]`, and `--help`.
   - `jwasm2/jwasm2test/` (Unit Test project): Links ONLY against `jwasm2lib` (plus the test framework) with `jwasm2/jwasm2lib/include` in its include directories.
2. Write unit tests in `jwasm2/jwasm2test/` verifying CLI flag parsing and error handling.
3. Build ONLY `jwasm2`, `jwasm2lib`, and `jwasm2test` via MSBuild and run `jwasm2test` to confirm a clean build and passing tests.
Use code with caution.

Phase 2 Prompt: Data-Driven ISA Table Generator (tools/ & data/)
Execute Phase 2 inside `jwasm2/`:
1. Design `jwasm2/jwasm2lib/include/isa_table.hpp` with compact, cache-friendly `constexpr` structs representing x86/x64/SIMD/AVX/VMX instruction encodings:
   - Mnemonic ID, operand patterns (reg, r/m, imm, rel, mem, xmm, ymm, mm, sreg, cr, dr), operand sizes (8/16/32/64/80/128/256-bit).
   - CPU feature flags (`8086`, `186`, `286`, `386`, `486`, `586`, `686`, `X64`, `PRIV`, `MMX`, `K3D`, `SSE`, `SSE2`, `SSE3`, `SSSE3`, `SSE4_1`, `SSE4_2`, `AVX`, `VMX`).
   - Encoding metadata: opcode map (`1B`, `0F`, `0F38`, `0F3A`, `0F0F`), primary/secondary opcode bytes, ModR/M reg extension (`/r` vs `/0`..`/7`), mandatory prefixes (`66`, `F2`, `F3`), `REX.W` requirement, and VEX attributes (`VEX.L`, `VEX.pp`, `VEX.mmmmm`, `VEX.W`, `vvvv` operand slot).
2. Create `jwasm2/tools/tablegen.py` and an initial seed `jwasm2/data/x86_isa.def` that compiles into a sorted, binary-searchable `constexpr` C++ header `jwasm2/jwasm2lib/src/generated_isa.inc`.
3. Add unit tests in `jwasm2/jwasm2test/` verifying table sorting, binary lookup by mnemonic, and metadata integrity.
4. Run `jwasm2/tools/tablegen.py`, build only `jwasm2lib` and `jwasm2test`, and verify all tests pass with zero `/W4` warnings.
Use code with caution.

Phase 3 Prompt: Front-End (Lexer, Macro Preprocessor & MASM v8 Parser)

text
Execute Phase 3 inside `jwasm2/`:
1. Implement the Lexer (`lexer.hpp/.cpp`) in `jwasm2lib` supporting case-insensitive MASM identifiers/keywords, Intel memory expressions (`[rbx + rcx*8 + 16]`, `DWORD PTR`, `FS:`, `OFFSET`), MASM radix literals (`0FFh`, `1010b`, `77o`, decimal), string literals, and accurate file/line/column tracking.
2. Implement the Preprocessor (`preprocessor.hpp/.cpp`) supporting `INCLUDE`, `EQU`, `=`, `TEXTEQU`, `MACRO`/`ENDM` with `LOCAL` labels and parameter substitution, `REPEAT`, `FOR`/`FORC`, and conditional assembly (`IF`, `IFE`, `IFDEF`, `IFNDEF`, `IFB`, `IFNB`, `IFIDNI`).
3. Implement the MASM v8 Parser & AST (`parser.hpp/.cpp`) supporting:
   - Segments & sections: `.MODEL`, `.DATA`, `.DATA?`, `.CONST`, `.CODE`, `SEGMENT`/`ENDS`, `GROUP`, `ASSUME`, `OPTION`.
   - Procedures & calls: `PROC`/`ENDP` (with `PUBLIC`/`PRIVATE`, `USES`, and 64-bit `FRAME`), `PROTO`, `INVOKE`, `LOCAL`.
   - Types & data: `STRUCT`/`UNION`/`ENDS`, `RECORD`, `TYPEDEF`, `DB`, `DW`, `DD`, `DQ`, `TBYTE`, `OWORD`, `YWORD`, `DUP`, `ALIGN`, `EVEN`, `PUBLIC`, `EXTERN`/`EXTRN`.
   - CPU directives: `.8086`, `.186`, `.286`/`.286P`, `.386`/`.386P`, `.486`/`.486P`, `.586`/`.586P`, `.686`/`.686P`, `.MMX`, `.K3D`, `.XMM`.
   - High-level directives: `.IF`/`.ELSEIF`/`.ELSE`/`.ENDIF`, `.WHILE`/`.ENDW`, `.REPEAT`/`.UNTIL`.
4. Add unit tests in `jwasm2/jwasm2test/` covering macros, nested structs, `INVOKE`, and MASM directives. Build and run `jwasm2test`.
Use code with caution.

Phase 4 Prompt: Core Instruction Encoder Engine (16-bit, 32-bit, 64-bit & VEX)

text
Execute Phase 4 inside `jwasm2/`:
1. Implement `encoder.hpp/.cpp` in `jwasm2lib` to encode AST instruction nodes using the `constexpr` ISA tables across `-16`, `-32`, and `-64` modes:
   - Emit legacy prefixes (`LOCK`, `REP`/`REPE`/`REPNE`, segment overrides) and automatically insert `66h` (operand-size) and `67h` (address-size) overrides when operand or addressing width differs from the active mode.
   - Encode 16-bit ModR/M addressing (`[bx+si]`, `[bp+di+disp8/16]`, `[disp16]`) and 32/64-bit ModR/M + SIB addressing (`[base + index*scale + disp8/32]`), including 64-bit RIP-relative addressing (`[rel sym]` and default 64-bit label references).
   - Generate `REX` prefixes (`40h–4Fh`) for 64-bit operand size (`REX.W`) and extended registers (`R8–R15`, `XMM8–XMM15`, `SIL`, `DIL`, `BPL`, `SPL`), emitting a diagnostic error if `REX` is combined with legacy high-byte registers (`AH`, `BH`, `CH`, `DH`).
   - Generate 2-byte (`C5h`) and 3-byte (`C4h`) `VEX` prefixes for AVX instructions (`VEX.128`, `VEX.256`, `vvvv` non-destructive operand encoding).
   - Enforce active CPU feature flags (rejecting instructions not enabled by the current CPU directive or target mode).
2. Add comprehensive encoder unit tests in `jwasm2/jwasm2test/` validating exact machine code hex bytes for 16-bit, 32-bit, and 64-bit addressing modes, overrides, REX, and VEX prefixes. Build and run `jwasm2test`.
Use code with caution.

Phase 5 Prompt: Populate Full ISA Definitions (x86_isa.def)

text
Execute Phase 5 inside `jwasm2/`:
Expand `jwasm2/data/x86_isa.def` to cover the complete requested instruction set, regenerating `generated_isa.inc` via `jwasm2/tools/tablegen.py` and adding byte-level unit tests in `jwasm2/jwasm2test/` for each group:
1. Base x86 (16-bit & 32-bit): Complete `8086`, `80186`, `80286`, `80386`, `80486`, `Pentium`, and `Pentium Pro` instruction sets (data transfer, ALU, bit manipulation, string ops, control flow, system/privileged instructions).
2. x86-64 (64-bit): Full `ML64` 64-bit instruction set (`MOVSXD`, `CDQE`, `CQO`, `CMOVcc`, `CMPXCHG16B`, `SWAPGS`, `SYSCALL`/`SYSRET`, 64-bit GPR variants).
3. SIMD & Vector Extensions:
   - `MMX` and `3DNow!` (`K3D` `0Fh 0Fh` + imm8 suffix opcodes).
   - `SSE`, `SSE2`, `SSE3`, `SSSE3`, `SSE4.1`, and `SSE4.2`.
   - `AVX` (VEX.128 and VEX.256 floating-point and integer vector instructions).
4. Virtualization: Intel `VMX` (`VMXON`, `VMXOFF`, `VMCALL`, `VMLAUNCH`, `VMRESUME`, `VMPTRLD`, `VMPTRST`, `VMCLEAR`, `VMREAD`, `VMWRITE`, `INVEPT`, `INVVPID`).

Build only `jwasm2lib` and `jwasm2test` and verify all instruction encoding tests pass.
Use code with caution.

Phase 6 Prompt: Multi-Pass Layout Engine & Branch Relaxation

text
Execute Phase 6 inside `jwasm2/`:
1. Implement the multi-pass assembly engine (`assembler.hpp/.cpp`) in `jwasm2lib`:
   - Pass 1: Build the symbol table, compute struct/record layouts, evaluate constant expressions, and assign initial segment/section offsets.
   - Branch Relaxation Loop: Iteratively resolve forward and backward jump targets, automatically selecting short (`rel8`) vs. near (`rel16`/`rel32`) encodings for `JMP` and `Jcc` until all symbol offsets converge without phase errors.
   - Pass 2: Emit final section byte buffers and record internal/external fixups and relocation entries.
2. Add unit tests in `jwasm2/jwasm2test/` testing forward jumps over large blocks (forcing `rel8` -> `rel16`/`rel32` promotion), tight backward loops (`rel8`), `ALIGN` padding, and unresolved symbol diagnostics.
3. Build only `jwasm2lib` and `jwasm2test` and verify all tests pass.
Use code with caution.

Phase 7 Prompt: Object Emitters (OMF, COFF) & Optional .map Generator

text
Execute Phase 7 inside `jwasm2/`:
Implement the output writers in `jwasm2lib`:
1. **16-bit OMF Writer (`omf_writer.hpp/.cpp`)** for `-16`:
   - Emit valid Intel 16-bit OMF `.obj` files containing `THEADR`, `LNAMES`, `SEGDEF`, `GRPDEF`, `PUBDEF`, `EXTDEF`, `LEDATA`, `FIXUPP` (segment/offset self-relative and pointer fixups), and `MODEND` (with optional start address), calculating accurate record lengths and checksums.
2. **32-bit & 64-bit COFF Writer (`coff_writer.hpp/.cpp`)** for `-32` and `-64`:
   - Emit valid Microsoft PE/COFF `.obj` files (`IMAGE_FILE_MACHINE_I386` for `-32`, `IMAGE_FILE_MACHINE_AMD64` for `-64`) with COFF file header, section headers (`.text`, `.data`, `.bss`, `.rdata`), raw section data, COFF relocation tables (`IMAGE_REL_I386_*` / `IMAGE_REL_AMD64_*`), symbol table, and string table.
   - Support `.pdata` and `.xdata` unwind info generation for 64-bit `PROC FRAME` procedures.
3. **Optional Map File Writer (`map_writer.hpp/.cpp`)** for `-map <file>`:
   - Emit a linker-style `.map` report listing module name, timestamp, segment/section start addresses, lengths, and classes, public symbols sorted both alphabetically by name and numerically by address, and the entry point address.
4. Wire all three emitters into `jwasm2lib::Driver::run`. Add unit tests in `jwasm2/jwasm2test/` validating OMF record checksums, COFF header/relocation structures, and `.map` formatting. Build and run `jwasm2test`.
Use code with caution.

Phase 8 Prompt: Differential Verification Against ml.exe & ml64.exe

text
Execute Phase 8 inside `jwasm2/`:
1. Create an end-to-end differential test suite in `jwasm2/jwasm2test/` with `.asm` test fixtures covering:
   - 16-bit real-mode segmented assembly (`-16` -> OMF `.obj` + `.map`).
   - 32-bit flat-model assembly with `INVOKE`, macros, structs, MMX, 3DNow!, and SSE–SSE4.2 (`-32` -> COFF `.obj` + `.map`).
   - 64-bit assembly with `PROC FRAME`, RIP-relative addressing, AVX (128/256-bit VEX), and Intel VMX instructions (`-64` -> COFF `.obj` + `.map`).
2. Assemble the 32-bit and 64-bit fixtures using both `jwasm2.exe` and MSVC's `ml.exe` / `ml64.exe`, run `dumpbin /all` on the generated `.obj` files, and compare section bytes, symbol tables, and relocation types.
3. Fix any encoding, relocation, or directive edge cases discovered during comparison, and verify that `jwasm2`, `jwasm2lib`, and `jwasm2test` build cleanly and pass 100% of tests.
Use code with caution.
