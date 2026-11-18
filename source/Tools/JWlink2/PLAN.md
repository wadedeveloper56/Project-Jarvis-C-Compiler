To switch to Plan mode in Visual Studio 2026, select Plan from the agents dropdown menu at the bottom of the Copilot Chat window.
If the option is missing from your chat window, it is likely hidden by the updated chat interface introduced in recent releases. Follow these steps depending on your current setup:

Standard Method (Classic Chat Experience)

If you are using the standard chat interface, you can enable and select Plan mode directly:
• Enable the feature. Go to Tools > Options, expand All Settings > GitHub > Copilot > Copilot Chat, and check the Enable Planning box.
• Select the agent. Open the Copilot Chat window, click the mode/agents dropdown at the bottom of the prompt box, and choose Plan.
• Execute the plan. Once Copilot generates a read-only markdown plan, click Implement plan to hand it off to Agent mode to make code changes.

If Plan Mode is Missing (VS 2026 18.11 / 18.12+)

In Visual Studio 2026 versions 18.11 and 18.12, Microsoft rolled out a redesigned chat interface that temporarily hides dedicated Plan mode. According to the Visual Studio Blog, you can restore it or work around it using the following methods:
• Revert to the previous chat experience. Open Tools > Options > GitHub > Copilot > Copilot Chat, uncheck Use new Copilot chat experience, and restart Visual Studio to bring Plan mode back to the dropdown.
• Use slash commands or custom prompts. While staying in the new experience, some users on Reddit note you can try typing /plan directly in the chat prompt. Alternatively, stay in Agent mode and explicitly instruct Copilot to outline an implementation plan and wait for your approval before editing files.
You can read more about configuring planning directories and read-only tools in the Microsoft Learn documentation.
If Plan mode still doesn't appear after toggling the setting, let me know your exact Visual Studio 2026 build number (found under Help > About Microsoft Visual Studio) so I can help troubleshoot your specific version.

Building a multi-architecture linker that spans four distinct binary formats is a massive systems-programming project. You can make GitHub Copilot in Visual Studio 2026 generate this reliably by breaking the architecture down into strict, modular specifications and using Plan Mode and Agent Mode instead of asking for the entire program in one prompt.
Because 16-bit DOS MZ, 16-bit NE (New Executable), 32-bit PE32, and 64-bit PE32+ rely on completely different memory models, relocation tables, and header structures, feeding Copilot a structured, phased roadmap prevents it from hallucinating or mixing up binary layouts.

1. Configure Visual Studio 2026 for Systems Engineering

Before generating code, set up Copilot so it behaves like a strict systems programmer and can compile and test binaries automatically.
• Create a new C++ or Rust/C# console project. Choose C++ (or C) if you want direct, zero-overhead struct packing (#pragma pack(push, 1)) for binary headers.
• Open Copilot Chat and select Plan Mode. Click the agent picker at the bottom of the Visual Studio Copilot Chat window and switch from Ask to Plan. This forces Copilot to write a Markdown architecture plan in .copilot/plans/ before touching code.
• Select a high-reasoning model. In the chat model dropdown, pick a strong reasoning model (such as Claude Sonnet/Opus or GPT-5/o-series) to handle bit-level math, endianness, and section alignment accurately.
• Add a .github/copilot-instructions.md file. Create this file in your solution root so every prompt follows strict binary rules. Include rules such as:
	• Always use explicit fixed-width types (uint8_t, uint16_t, uint32_t, uint64_t).
	• Enforce 1-byte struct alignment for all executable file headers.
	• Never omit checksums, relocation entries, or stub headers.

2. Define the Binary Formats Explicitly

A single prompt saying "16-bit exe" is ambiguous because DOS and Windows 3.x use different formats. Specify the exact object inputs and executable outputs you want Copilot to implement:

|Target Output|Binary Format|Key Structural Requirements to Specify|
|---|---|---|
|16-bit DOS EXE|MZ (Mark Zbikowski)|IMAGE_DOS_HEADER (0x5A4D), paragraph-aligned header size, segment-relative relocation table (segment:offset), SS:SP and CS:IP initial registers.|
|16-bit Windows EXE / DLL|NE (New Executable)|DOS MZ stub pointing to NE header (0x454E), Segment Table, Resource Table, Resident/Non-Resident Name Tables, Module Reference Table, and Entry Table (for DLL exports).|
|32-bit Windows EXE / DLL|PE32 (Portable Executable)|DOS stub + PE\0\0 signature, COFF File Header (0x014C), Optional Header (0x010B), Section Table (.text, .data, .rdata, .reloc), Import/Export Data Directories, and IMAGE_FILE_DLL (0x2000) flag handling.|
|64-bit Windows EXE / DLL|PE32+|Same as PE32, except Machine is 0x8664 (AMD64), Optional Header Magic is 0x020B, ImageBase and stack/heap sizes expand to 64-bit (uint64_t), and BaseOfData is removed.|


3. Step-by-Step Prompting Workflow


Step 1: Generate the Architecture Plan

Switch to Plan mode in Copilot Chat and send this prompt:
"Design a modular, cross-target linker in C++20 capable of reading OMF (16-bit) and COFF (32/64-bit) object files and emitting 16-bit DOS MZ EXEs, 16-bit Windows NE EXEs/DLLs, 32-bit PE32 EXEs/DLLs, and 64-bit PE32+ EXEs/DLLs. Break the design into: 1) Intermediate Representation (IR) for sections, symbols, and relocations; 2) Input parsers (OMF and COFF); 3) Symbol resolver and virtual address/segment layout engine; 4) Format emitters for MZ, NE, PE32, and PE32+. Save the implementation plan."
Review the generated .copilot/plans/ file to verify the data structures before moving on.

Step 2: Build the Core IR and Input Parsers

Switch the chat dropdown from Plan to Agent mode. In Agent mode, Copilot can create files, run MSBuild, and fix compiler errors automatically. Prompt it to build one layer at a time:
"Implement Step 1 and Step 2 from our plan. Create the internal Section, Symbol, and Relocation IR structs, followed by a COFF .obj parser and an Intel OMF .obj parser. Build the solution and verify there are zero warnings."

Step 3: Implement Emitters One Format at a Time

Do not generate all four output backends simultaneously. Prompt Agent mode sequentially:
1. PE32 and PE32+ Emitters: Ask Copilot to generate the PE emitter supporting both 0x010B (32-bit) and 0x020B (64-bit), including .idata (Import Address Table), .edata (DLL export directory when /DLL is passed), and .reloc (base relocations).
2. 16-bit DOS MZ Emitter: Ask Copilot to generate the MZ writer that calculates 16-byte paragraph boundaries, patches CS:IP / SS:SP, and emits segment fixup records.
3. 16-bit NE Emitter: Ask Copilot to wrap the DOS MZ stub with an NE header (0x454E), building the segment table, entry table for exported DLL ordinals, and movable/fixed segment relocation records.

4. Verify Generated Binaries Automatically

Because Agent mode in Visual Studio 2026 can execute terminal commands, instruct Copilot to write an end-to-end test suite that validates every output format:
• PE32 / PE32+: Have Copilot run dumpbin /headers /exports /imports (included with Visual Studio) against your linker's output to verify Windows recognizes the headers and DLL export tables.
• 16-bit MZ / NE: Have Copilot write unit tests that inspect the binary offsets, magic bytes (MZ and NE), and relocation counts, or run the 16-bit MZ executable inside DOSBox via CLI to confirm execution.
If you'd like to tailor the prompts or starter code, let me know:
• Which implementation language you prefer (C++, C, C#, or Rust)
• Which input object file formats your linker needs to read (COFF .obj, OMF .obj, ELF, or custom assembly output)
• Whether you need DLL import library (.lib) generation alongside the executables
I can generate the exact .github/copilot-instructions.md file and Step 1 starter prompt for your setup.

Below are the updated .github/copilot-instructions.md file and Step 1 starter prompt with full support for generating COFF import libraries (.lib) for 32/64-bit DLLs and OMF import libraries (.lib) for 16-bit NE DLLs.

1. Updated .github/copilot-instructions.md

Replace your .github/copilot-instructions.md file with this version so Copilot enforces the exact binary specifications for both COFF short-import archives and OMF IMPDEF libraries.
markdown
# Multi-Target Linker Development Guidelines (C++20)

## Project Overview
You are assisting in building a deterministic, zero-dependency, cross-target linker written in **modern C++20**.
- **Inputs:** 16-bit Intel OMF (`.obj`, `.lib`), 32-bit/64-bit Microsoft COFF (`.obj`, `.lib` archive format).
- **Outputs:** 
  1. 16-bit DOS MZ Executable (`.exe`)
  2. 16-bit Windows NE (New Executable) `.exe` and `.dll` + **OMF Import Library (`.lib`)**
  3. 32-bit Windows PE32 `.exe` and `.dll` + **COFF Import Library (`.lib`)**
  4. 64-bit Windows PE32+ `.exe` and `.dll` + **COFF Import Library (`.lib`)**

## Coding & Memory Standards
- **Language Standard:** C++20 (`/std:c++20` with `/W4 /WX` warning levels).
- **Fixed-Width Types:** Strictly use `<cstdint>` types (`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`) for all file offsets, RVAs, segment selectors, and binary fields. Never use `long` or platform-dependent `int` sizes for binary structures.
- **Struct Packing & Alignment:** Wrap all on-disk binary header structs (DOS MZ, NE, COFF, PE32, PE32+, OMF records) in `#pragma pack(push, 1)` and `#pragma pack(pop)`. Validate every header struct size with `static_assert(sizeof(...) == ...)` at compile time.
- **Endianness:** Explicitly write little-endian serialization helpers (`write_u16_le`, `write_u32_le`, `write_u64_le`), plus big-endian helpers (`write_u32_be`) specifically required for COFF `.lib` archive linker member offsets.
- **Zero External Dependencies:** Rely solely on the C++20 Standard Library (`<vector>`, `<span>`, `<filesystem>`, `<variant>`, `<unordered_map>`, `<bit>`). Do not introduce third-party libraries.

## Binary Format Rules
- **COFF Archives (`.lib` Input & Output):**
  - Parse and emit the `!<arch>\n` magic header, even-byte member padding (`\n`), First Linker Member (`/` with big-endian symbol counts and offsets), Second Linker Member (`/` with little-endian offsets and sorted symbol index), and Longnames Member (`//`).
  - Support both standard COFF object members and short import descriptors (`IMPORT_OBJECT_HEADER` with `Sig1 = 0x0000`, `Sig2 = 0xFFFF`, `Machine` `0x014C` or `0x8664`, `ImportType`, and `NameType` by name vs. ordinal).
  - When building a 32-bit or 64-bit `/DLL` (or `/IMPLIB:<file>`), automatically emit a matching COFF import `.lib` containing the import descriptor header, null-terminated public symbol name, and DLL filename for every exported symbol.
- **OMF Objects & Libraries (`.obj`, `.lib` Input & Output):**
  - Parse record-based streams (`THEADR`, `LNAMES`, `SEGDEF`, `GRPDEF`, `PUBDEF`, `EXTDEF`, `LEDATA`, `LIDATA`, `FIXUPP`, `MODEND`) with valid 1-byte record checksums.
  - For OMF `.lib` files, handle `LIBHEAD` (`0xF0`), page-aligned module padding (`PageSize`, e.g., 16 or 512 bytes), and `LIBDIC` (`0xF1`).
  - When building a 16-bit NE `/DLL` (or `/IMPLIB:<file>`), automatically emit a matching OMF import `.lib` containing `THEADR`, `COMENT` class `0xA0` (`OMF_IMPDEF` subtype `0x01` specifying internal name, module name, and ordinal/entry name), `MODEND`, and the `LIBDIC` symbol dictionary.
- **16-bit DOS MZ:** Enforce 16-byte paragraph alignment for header size (`e_cparhdr`) and segment load image. Emit exact `segment:offset` relocation pairs.
- **16-bit Windows NE:** Prepend a valid DOS MZ stub with `e_lfanew` pointing to the `NE` (`0x454E`) header. Populate Segment Table, Resource Table, Resident/Non-Resident Name Tables, Module Reference Table, Imported Names Table, and Entry Table (ordinal exports for `.dll`).
- **32-bit PE32 & 64-bit PE32+:** Maintain strict separation between `FileAlignment` (typically `0x200`) and `SectionAlignment` (typically `0x1000`). Set `IMAGE_FILE_DLL` (`0x2000`) when building DLLs and emit `.edata` (Export Directory), `.idata` (Import Directory / IAT), and `.reloc` (Base Relocations).

## Verification & Testing Workflow
- After implementing or modifying any emitter, verify PE32/PE32+ DLLs and COFF `.lib` import libraries using `dumpbin /headers /exports /imports /archivemembers` via the developer terminal.
- Never leave stubbed `TODO` placeholders in relocation math, archive indexing, or checksum routines.
Use code with caution.

2. Updated Step 1 Starter Prompt (Use in Plan Mode)

Switch Copilot Chat in Visual Studio 2026 to Plan mode and paste this prompt:
Prompt:
Using our .github/copilot-instructions.md rules, create a complete architectural plan in .copilot/plans/linker-architecture.md for our C++20 multi-target linker.
Break the design into 5 concrete implementation phases:
1. Unified Intermediate Representation (IR): Define LinkerContext, InputModule, Section (supporting both 16-bit segment:offset / GROUP semantics and 32/64-bit flat RVAs), Symbol (global, weak, local, DLL import, DLL export with ordinal/name/calling-convention), and Relocation (abstracting OMF FIXUPP and COFF relocation types).
2. Input Parsers & Archive Readers:
	• COFF .obj parser (32-bit 0x014C and 64-bit 0x8664) and COFF .lib archive reader (handling standard object members and IMPORT_OBJECT_HEADER short import libraries).
	• OMF .obj parser (THEADR, LNAMES, SEGDEF, GRPDEF, PUBDEF, EXTDEF, LEDATA, LIDATA, FIXUPP, MODEND) and OMF .lib page-aligned library reader (including COMENT 0xA0 IMPDEF records).
3. Symbol Resolution & Dead-Code Elimination: Archive member extraction on demand for unresolved symbols, duplicate symbol detection, and /ENTRY + /EXPORT + .def file root retention.
4. Layout & Relocation Engine: Separate layout strategies for 16-bit segmented addressing (64KB segment limits, DGROUP folding, paragraph alignment) vs. flat 32/64-bit virtual memory (FileAlignment vs. SectionAlignment).
5. Binary Emitters, Import Library Generators & CLI Driver:
	• Command-line parser supporting /OUT:, /IMPLIB:, /DEF:, /SUBSYSTEM:, /DLL, /ENTRY:, /MACHINE: (DOS, NE16, X86, X64).
	• Executable/DLL emitters for MZ, NE, PE32, and PE32+.
	• Import Library (.lib) Emitters: A COFF short-import .lib generator (emitting !<arch>\n, First/Second Linker Members, and IMPORT_OBJECT_HEADER records for PE32/PE32+ DLLs) and an OMF .lib generator (emitting LIBHEAD, THEADR + COMENT 0xA0 IMPDEF + MODEND modules, and LIBDIC for 16-bit NE DLLs).
Include the exact directory structure, header file names, and core C++20 data structures in the plan. Do not write .cpp implementation files yet.
Once Copilot generates .copilot/plans/linker-architecture.md in Plan Mode, would you like the exact Agent Mode prompts to build and test the COFF/OMF import library round-trip (generating a DLL + .lib and linking an .exe against that .lib)?
