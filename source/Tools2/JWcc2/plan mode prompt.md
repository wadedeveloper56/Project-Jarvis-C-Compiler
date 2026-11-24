Read `.github/instructions/jwcc2.instructions.md` carefully. Note that this Visual Studio 2026 solution contains other existing projects outside `jwcc2/`—do not inspect, touch, modify, or link against any files outside `jwcc2/`.
Ignore all existing or legacy code in the workspace and design jwcc2 completely from scratch according to `.github/instructions/jwcc2.instructions.md`. Save the complete architectural plan to `.copilot/plans/jwcc2-architecture.md`.
Enforce this exact 3-project structure:
1. jwcc2/jwcc2/ — Contains only main.cpp with main(), forwarding CLI args to jwcc2lib.
2. jwcc2/jwcc2lib/ — Static library containing all linker headers and implementation (include/... and src/...).
3. jwcc2/jwcc2test/ — Standalone native unit test runner executable containing unit and integration tests exercising jwcc2lib.

Create a phased implementation plan covering:
1. MSBuild configuration for `jwcc2lib.vcxproj`, `jwcc2.vcxproj` (with project reference to `jwcc2lib`), and `jwcc2test.vcxproj` (with project reference to `jwcc2lib`).
2. Public `jwcc2lib` driver API (`CompilerDriver`, `CompileOptions`, `CompileResult`) so `jwcc2/src/main.cpp` stays minimal and `jwcc2test` can invoke full compiler pipelines in-memory or on files.
3. `TargetInfo` data model and ABI rules for `-m16`, `-m32`, and `-m64`.
4. C99 Preprocessor, Lexer, Recursive-Descent/Pratt Parser, arena-allocated AST, and Semantic Analyzer inside `jwcc2lib`.
5. Custom three-address/SSA-like Backend IR (`IRModule`, `IRFunction`, `BasicBlock`, `IRInst`, virtual registers, stack slots, and target ABI legalization pass) inside `jwcc2lib`.
6. Three MASM emitters (`Masm16Emitter`, `Masm32Emitter`, `Masm64Emitter`) inside `jwcc2lib` and an integration test harness in `jwcc2test` that invokes `ml.exe` and `ml64.exe` to verify generated `.asm` output assembles cleanly.

Do not write source files yet—present the complete directory tree across all three projects, key class interfaces, and phase breakdown for review.
