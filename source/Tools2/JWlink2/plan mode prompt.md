Create three empty Visual Studio 2026 projects in your solution (jwlink2lib as a Static Library, jwlink2 as a Console App referencing jwlink2lib, and jwlink2test as a Console App referencing jwlink2lib). Then switch Copilot Chat to Plan mode and paste this prompt:
Prompt:

```
Ignore all existing or legacy code in the workspace and design jwlink2 completely from scratch according to .github/copilot-instructions.md. Save the complete architectural plan to .copilot/plans/jwlink2-architecture.md.
Enforce this exact 3-project structure:
• jwlink2/ — Contains only main.cpp with main(), forwarding CLI args to jwlink2lib.
• jwlink2lib/ — Static library containing all linker headers and implementation (include/jwlink2lib/... and src/...).
• jwlink2test/ — Standalone test runner executable containing unit and integration tests exercising jwlink2lib.
Break the plan into 5 concrete implementation phases:
1. Core IR & Binary Utilities (jwlink2lib): Little/big-endian stream readers/writers, packed format headers with static_assert checks, PE 32-bit checksum calculator, and unified IR (LinkerContext, TargetMachine, OutputKind for Executable/Dll/Driver/FlatBinary, SubsystemMode for GUI/Console/Native/DOS, InputModule, Section, Symbol, Relocation) supporting both 16-bit segmented (GROUP, paragraph alignment, fixed/movable segment flags) and 32/64-bit linear addressing.
2. COFF & OMF Parsers (jwlink2lib): COFF .obj (X86/X64) and COFF .lib (standard objects + IMPORT_OBJECT_HEADER short imports); OMF .obj (THEADR, LNAMES, SEGDEF, GRPDEF, PUBDEF, EXTDEF, LEDATA, LIDATA, FIXUPP, MODEND) and OMF .lib (LIBHEAD, page alignment, COMENT 0xA0 IMPDEF, LIBDIC).
3. Symbol Resolution, Entry Inference, CLI Driver & Flat Binary Emitter (jwlink2lib, jwlink2, jwlink2test): On-demand archive extraction, .def file parsing, default entry-point inference across GUI (WinMain/wWinMain), Console (main/wmain), DLL (DllMain), and Kernel Driver (GsDriverEntry/DriverEntry/_DriverEntry@8), dead-code elimination, jwlink2::Driver::run() + jwlink2/main.cpp, and the FLAT16/FLAT32/FLAT64 raw binary emitter with unit tests in jwlink2test.
4. 16-Bit Segmented Layout, DOS MZ & Windows NE (EXE/DLL/DRV) Emitters (jwlink2lib, jwlink2test): DGROUP folding, 64KB segment checks, DOS MZ .exe emitter, Windows 16-bit NE .exe (GUI WINDOWAPI 0x0300 & Console WINDOWCOMPAT 0x0200), /DLL, and 16-bit Device Driver (/DRIVER .drv with fixed/preload segments and DriverProc/WEP exports), plus the OMF IMPDEF .lib import library generator.
5. 32/64-Bit Paged Layout, PE32/PE32+ (EXE/DLL/SYS) & COFF Import Library Emitters (jwlink2lib, jwlink2test): FileAlignment vs. SectionAlignment, GUI (Subsystem = 2), Console (Subsystem = 3), and Kernel Driver (/DRIVER / Subsystem = 1 .sys with INIT discardable sections, preserved .reloc, WDM flag, and mandatory valid PE checksum), .idata/.edata/.reloc synthesis, COFF short-import .lib generator, and dumpbin integration tests.
List the exact file tree under jwlink2/, jwlink2lib/, and jwlink2test/ and define the core C++20 structs. Do not write .cpp files yet
```