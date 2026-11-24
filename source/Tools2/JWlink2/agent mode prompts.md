Once .copilot/plans/jwlink2-architecture.md is generated, switch Copilot Chat to Agent mode and run these prompts one by one:

Agent Prompt 1: Core IR & COFF/OMF Parsers (jwlink2lib)
```
Using .copilot/plans/jwlink2-architecture.md and .github/copilot-instructions.md (ignoring all legacy code outside jwlink2*), implement Phase 1 (Core IR & Binary Utilities) and Phase 2 (COFF and OMF .obj/.lib Parsers) inside jwlink2lib/. Include SubsystemMode (Console, WindowsGui, Native, Dos) in LinkerContext. Compile jwlink2lib with /std:c++20 /W4 /WX and fix any warnings or errors.
```
Agent Prompt 2: Symbol Resolver, Thin main(), and Flat Binary Emitter (jwlink2lib, jwlink2, jwlink2test)
```
Implement Phase 3:
1. Add the symbol resolver (iterative .lib extraction, .def parsing, dead-code elimination, and automatic GUI vs. Console entry-point inference when /ENTRY: is omitted) and Driver::run() CLI parser (/SUBSYSTEM:CONSOLE, /SUBSYSTEM:WINDOWS, /OUT:, /MACHINE:, /BASE:, /PADBSS, /ENTRY:, /DLL, /IMPLIB:, /DEF:) to jwlink2lib/.
2. Create jwlink2/main.cpp containing only main() calling jwlink2lib's Driver::run().
3. Implement the FLAT16, FLAT32, and FLAT64 raw binary emitter in jwlink2lib/.
4. Implement the test harness and synthetic object unit tests for the parsers, symbol resolver, entry inference, and flat binary outputs in jwlink2test/.
Build all three projects (jwlink2lib, jwlink2, jwlink2test) with zero warnings and run jwlink2test.exe to verify all tests pass.
```
Agent Prompt 3: 16-Bit DOS MZ, Windows NE, and OMF Import Library (jwlink2lib, jwlink2test)
```
Implement Phase 4 in jwlink2lib/:
1. Build the 16-bit segmented layout engine (DGROUP consolidation, paragraph alignment, 64KB segment overflow checks).
2. Implement the 16-bit DOS MZ .exe emitter (/MACHINE:DOS).
3. Implement the 16-bit Windows NE .exe and /DLL emitter (/MACHINE:NE16), setting NE program application type flags to WINDOWAPI (0x0300) for /SUBSYSTEM:WINDOWS (GUI) and WINDOWCOMPAT (0x0200) for /SUBSYSTEM:CONSOLE, plus the OMF IMPDEF .lib import library generator.
4. Add unit and round-trip integration tests in jwlink2test/ verifying MZ relocations, NE GUI vs. Console flags, NE segment/entry tables, and linking an NE .exe against a generated OMF .lib.
Build the solution and run jwlink2test.exe until all tests pass.
```
Agent Prompt 4: 32-Bit PE32, 64-Bit PE32+, and COFF Import Library (jwlink2lib, jwlink2test)
```
Implement Phase 5 in jwlink2lib/:
1. Build the paged virtual layout engine (FileAlignment 0x200 vs. SectionAlignment 0x1000).
2. Implement PE32 (/MACHINE:X86) and PE32+ (/MACHINE:X64) emitters for .exe and /DLL, setting OptionalHeader.Subsystem to 2 (IMAGE_SUBSYSTEM_WINDOWS_GUI) for GUI or 3 (IMAGE_SUBSYSTEM_WINDOWS_CUI) for Console, and synthesizing .idata, .edata, .reloc, and valid PE checksums.
3. Implement the COFF short-import .lib generator (!<arch>\n, First/Second Linker Members, and IMPORT_OBJECT_HEADER records).
4. Add end-to-end tests in jwlink2test/ that generate 32-bit and 64-bit DLLs + .lib files, link both GUI and Console EXEs against them, and run dumpbin /headers /exports /imports /archivemembers via the terminal to verify zero format errors and accurate subsystem headers.

```