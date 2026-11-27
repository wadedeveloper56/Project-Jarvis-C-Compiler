Once .copilot/plans/jwlink2-architecture.md is generated, switch Copilot Chat to Agent mode and run these prompts one by one:

Agent Prompt 1: Core IR & COFF/OMF Parsers (jwlink2lib)
```
Using .copilot/plans/jwlink2-architecture.md and .github/instruction/jwlink2.instructions.md (ignoring all legacy code outside jwlink2*), 
implement Phase 1 (Numbered Diagnostic Subsystem JWL1000–JWL5999, Core IR & Binary Utilities) and 
Phase 2 (COFF and OMF .obj/.lib Parsers) inside jwlink2lib/. 
Ensure all parser errors and warnings emit structured JWL2xxx codes through DiagnosticEngine without writing directly to std::cerr. 
Compile jwlink2lib with /std:c++20 /W4 /WX and fix any warnings or errors.
```
Agent Prompt 2: Symbol Resolver, Thin main(), and Flat Binary Emitter (jwlink2lib, jwlink2, jwlink2test)
```
Implement Phase 3:
1. Add the symbol resolver (iterative .lib extraction, .def parsing, dead-code elimination, JWL3xxx unresolved/duplicate/mismatch diagnostics, and automatic entry-point inference for GUI WinMain, Console main, DLL DllMain, and Driver DriverEntry) and Driver::run() CLI parser (/WX, /NOWARN:, /SUBSYSTEM:CONSOLE, /SUBSYSTEM:WINDOWS, /SUBSYSTEM:NATIVE, /DRIVER, /DRIVER:WDM, /OUT:, /MACHINE:, /BASE:, /PADBSS, /ENTRY:, /DLL, /IMPLIB:, /DEF:) to jwlink2lib/.
2. Create jwlink2/main.cpp containing only main() calling jwlink2lib's Driver::run().
3. Implement the FLAT16, FLAT32, and FLAT64 raw binary emitter in jwlink2lib/ (emitting JWL3006 on DLL imports and JWL4003/JWL4005 on overflow).
4. Implement the test harness in jwlink2test/ with both positive tests and negative tests asserting exact JWL1xxx–JWL4xxx diagnostic codes and /WX behavior.
Build all three projects (jwlink2lib, jwlink2, jwlink2test) with zero warnings and run jwlink2test.exe to verify all tests pass.```
```
Agent Prompt 3: 16-Bit DOS MZ, Windows NE, and OMF Import Library (jwlink2lib, jwlink2test)
```
Implement Phase 4 in jwlink2lib/:
1. Build the 16-bit segmented layout engine (DGROUP consolidation, paragraph alignment, 64KB segment overflow checks emitting JWL4001/JWL4002, fixed vs. movable segment attributes).
2. Implement the 16-bit DOS MZ .exe emitter (/MACHINE:DOS).
3. Implement the 16-bit Windows NE emitter (/MACHINE:NE16) supporting GUI .exe (WINDOWAPI 0x0300), Console .exe (WINDOWCOMPAT 0x0200), /DLL, and 16-bit Device Drivers (/DRIVER .drv) with 0x8000 library/driver flags, SINGLEDATA, fixed PRELOAD segments, and DriverProc/WEP exports (emitting JWL5001/JWL5002/JWL5004 on violations), plus the OMF IMPDEF .lib import library generator.
4. Add positive and negative unit/integration tests in jwlink2test/ verifying MZ relocations, NE GUI/Console/DRV flags, JWL4001 64KB segment overflow diagnostics, and linking an NE .exe against a generated OMF .lib.
Build the solution and run jwlink2test.exe until all tests pass.
```
Agent Prompt 4: 32-Bit PE32, 64-Bit PE32+, and COFF Import Library (jwlink2lib, jwlink2test)
```
Implement Phase 5 in jwlink2lib/:
1. Build the paged virtual layout engine (FileAlignment 0x200 vs. SectionAlignment 0x1000, INIT discardable section flags).
2. Implement PE32 (/MACHINE:X86) and PE32+ (/MACHINE:X64) emitters supporting:
	• GUI .exe (Subsystem = 2) and Console .exe (Subsystem = 3)
	• /DLL (IMAGE_FILE_DLL 0x2000)
	• 32-bit and 64-bit Windows Kernel Drivers (/DRIVER / /SUBSYSTEM:NATIVE, .sys) with Subsystem = 1, WDM flag support, preserved .reloc base relocations, DriverEntry entry inference (JWL5004 if missing), and mandatory valid PE header checksums
	• .idata, .edata, and .reloc synthesis.
3. Implement the COFF short-import .lib generator (!<arch>\n, First/Second Linker Members, and IMPORT_OBJECT_HEADER records).
4. Add end-to-end tests in jwlink2test/ that generate 32-bit and 64-bit DLLs + .lib files, GUI and Console EXEs, and .sys kernel drivers, test diagnostic reporting (JWL3001, JWL3002, JWL5004), and run dumpbin /headers /exports /imports /archivemembers via the terminal to verify zero format errors and valid PE checksums.

```