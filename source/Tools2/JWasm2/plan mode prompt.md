Step 0: Copilot Architectural Plan Prompt (Run First)

Run this prompt first so Copilot reads your instructions, maps out the module interfaces inside jwasm2/, and confirms its plan without touching any sibling projects in your solution.
```
Save the complete architectural plan to .copilot/plans/jwasm2-architecture.md.
Read `.github/instructions/jwasm2.instructions.md` carefully. Note that this Visual Studio 2026 solution contains other existing projects outside `jwasm2/`—do not inspect, touch, modify, or link against any files outside `jwasm2/`.
Before creating any files, produce a detailed technical architecture plan for `jwasm2`, `jwasm2lib`, and `jwasm2test` inside `jwasm2/`:

Enforce this exact 3-project structure:
1. jwasm2/jwasm2/ — Contains only main.cpp with main(), forwarding CLI args to jwasm2lib.
2. jwasm2/jwasm2lib/ — Static library containing all linker headers and implementation (include/... and src/...).
3. jwasm2/jwasm2test/ — Standalone native unit test runner executable containing unit and integration tests exercising jwasm2lib.

Break the plan into 8 concrete implementation phases:
1. Project Scaffolding & Isolated Build Pipeline
2. Data-Driven ISA Table Generator (tools/ & data/)
3. Front-End (Lexer, Macro Preprocessor & MASM v8 Parser)
4. Core Instruction Encoder Engine (16-bit, 32-bit, 64-bit & VEX)
5. Populate Full ISA Definitions (x86_isa.def)
6. Multi-Pass Layout Engine & Branch Relaxation
7. Object Emitters (OMF, COFF) & Optional .map Generator
8. Differential Verification Against ml.exe & ml64.exe

List the exact file tree under jwasm2/, jwasm2lib/, and jwasm2test/ and define the core C++20 structs. Do not write .cpp files yet
```
