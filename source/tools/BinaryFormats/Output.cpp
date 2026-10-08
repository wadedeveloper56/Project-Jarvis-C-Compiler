#include "pch.h"
#include "framework.h"
#include "BinaryFormat.h"
#include "capstone/capstone.h"

using namespace std;

WORD_FLAG_DESCRIPTIONS ImageFileHeaderCharacteristics[] =
{
	{ IMAGE_FILE_RELOCS_STRIPPED, "Relocation info stripped from file" },
	{ IMAGE_FILE_EXECUTABLE_IMAGE, "Executable" },
	{ IMAGE_FILE_LINE_NUMS_STRIPPED, "LINE_NUMS_STRIPPED" },
	{ IMAGE_FILE_LOCAL_SYMS_STRIPPED, "LOCAL_SYMS_STRIPPED" },
	{ IMAGE_FILE_AGGRESIVE_WS_TRIM, "AGGRESIVE_WS_TRIM" },
	{ IMAGE_FILE_LARGE_ADDRESS_AWARE, "Application can handle large (>2GB) addresses" },
	{ IMAGE_FILE_BYTES_REVERSED_LO, "BYTES_REVERSED_LO" },
	{ IMAGE_FILE_32BIT_MACHINE, "32-Bit Machine" },
	{ IMAGE_FILE_DEBUG_STRIPPED, "DEBUG_STRIPPED" },
	{ IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "REMOVABLE_RUN_FROM_SWAP" },
	{ IMAGE_FILE_NET_RUN_FROM_SWAP, "NET_RUN_FROM_SWAP" },
	{ IMAGE_FILE_SYSTEM, "SYSTEM" },
	{ IMAGE_FILE_DLL, "DLL" },
	{ IMAGE_FILE_UP_SYSTEM_ONLY, "UP_SYSTEM_ONLY" },
	{ IMAGE_FILE_BYTES_REVERSED_HI, "BYTES_REVERSED_HI" }
};

#define NUMBER_IMAGE_HEADER_FLAGS (sizeof(ImageFileHeaderCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

WORD_FLAG_DESCRIPTIONS DllCharacteristics[] =
{
   { IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA,"Image can handle a high entropy 64-bit virtual address space" },
   { IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE,"DLL can move" },
   { IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY,"CODE_INTEGRITY" },
   { IMAGE_DLLCHARACTERISTICS_NX_COMPAT,"NX Compatible" },
   { IMAGE_DLLCHARACTERISTICS_NO_ISOLATION,"NO_ISOLATION" },
   { IMAGE_DLLCHARACTERISTICS_NO_SEH,"NO_SEH" },
   { IMAGE_DLLCHARACTERISTICS_NO_BIND,"NO_BIND" },
   { IMAGE_DLLCHARACTERISTICS_APPCONTAINER,"APP_CONTAINER" },
   { IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, "WDM_DRIVER" },
   { IMAGE_DLLCHARACTERISTICS_GUARD_CF,"CONTROL_FLOW_GUARD" },
   { IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE,"TERMINAL_SERVER_AWARE" }
};
#define NUMBER_DLL_CHARACTERISTICS (sizeof(DllCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

i386RelocTypes i386Relocations[] =
{
	{ IMAGE_REL_I386_ABSOLUTE, "ABSOLUTE" },
	{ IMAGE_REL_I386_DIR16, "DIR16" },
	{ IMAGE_REL_I386_REL16, "REL16" },
	{ IMAGE_REL_I386_DIR32, "DIR32" },
	{ IMAGE_REL_I386_DIR32NB, "DIR32NB" },
	{ IMAGE_REL_I386_SEG12, "SEG12" },
	{ IMAGE_REL_I386_SECTION, "SECTION" },
	{ IMAGE_REL_I386_SECREL, "SECREL" },
	{ IMAGE_REL_I386_REL32, "REL32" }
};
#define I386RELOCTYPECOUNT (sizeof(i386Relocations) / sizeof(i386RelocTypes))

DWORD_FLAG_DESCRIPTIONS SectionCharacteristics[] =
{
	{ IMAGE_SCN_TYPE_NO_PAD, "No Padding" },
	{ IMAGE_SCN_CNT_CODE, "Code" },
	{ IMAGE_SCN_CNT_INITIALIZED_DATA, "Initialized Data" },
	{ IMAGE_SCN_CNT_UNINITIALIZED_DATA, "Unnitialized Data" },
	{ IMAGE_SCN_LNK_OTHER, "Other" },
	{ IMAGE_SCN_LNK_INFO, "Informataion" },
	{ IMAGE_SCN_LNK_REMOVE, "Remove" },
	{ IMAGE_SCN_LNK_COMDAT, "COMDAT" },
	{ IMAGE_SCN_GPREL, "Global Pointer" },
	{ IMAGE_SCN_MEM_PURGEABLE, "Purgeable" },
	{ IMAGE_SCN_MEM_LOCKED, "Locked" },
	{ IMAGE_SCN_MEM_PRELOAD, "Preload" },
	{ IMAGE_SCN_ALIGN_1BYTES,"1_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_2BYTES,"2_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_4BYTES,"4_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_8BYTES,"8_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_16BYTES,"16_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_32BYTES,"32_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_64BYTES,"64_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_128BYTES,"128_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_256BYTES,"256_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_512BYTES,"512_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_1024BYTES,"1024_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_2048BYTES,"2048_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_4096BYTES,"4096_BYTE_ALIGN" },
	{ IMAGE_SCN_ALIGN_8192BYTES,"8192_BYTE_ALIGN" },
	{ IMAGE_SCN_LNK_NRELOC_OVFL,"EXTENDED_RELOCATIONS" },
	{ IMAGE_SCN_MEM_DISCARDABLE,"Discardable" },
	{ IMAGE_SCN_MEM_NOT_CACHED,"NO_CACHE" },
	{ IMAGE_SCN_MEM_NOT_PAGED,"NOTPAGEABLE" },
	{ IMAGE_SCN_MEM_SHARED,"Shared" },
	{ IMAGE_SCN_MEM_EXECUTE,"Executable" },
	{ IMAGE_SCN_MEM_READ,"Readable" },
	{ IMAGE_SCN_MEM_WRITE,"Writeable" },
};

#define NUMBER_SECTION_CHARACTERISTICS (sizeof(SectionCharacteristics) / sizeof(DWORD_FLAG_DESCRIPTIONS))

void GetObjRelocationName(WORD type, PSTR buffer, DWORD cBytes)
{
	DWORD i;

	for (i = 0; i < I386RELOCTYPECOUNT; i++)
	{
		if (type == i386Relocations[i].type)
		{
			strncpy(buffer, i386Relocations[i].name, cBytes);
			return;
		}
	}
	sprintf(buffer, "???_%X", type);
}

void DumpSection(int i, OBJSectionPtr ptr, WORD machine)
{
	NTSectionHeaderPtr section = &ptr->header;
	printf("SECTION HEADER #%X (%d)\n", i, i);
	printf("%8.8s name\n", section->Name);
	printf("% 8X physical address\n", section->Misc.PhysicalAddress);
	printf("% 8X virtual address\n", section->VirtualAddress);
	printf("% 8X (%d) size of raw data\n", section->SizeOfRawData, section->SizeOfRawData);
	printf("% 8X file pointer to raw data (%08X to %08X)\n", section->PointerToRawData, section->PointerToRawData, (section->PointerToRawData + section->SizeOfRawData - 1));
	printf("% 8X file pointer to relocation table\n", section->PointerToRelocations);
	printf("% 8X file pointer to line numbers\n", section->PointerToLinenumbers);
	printf("% 8X number of relocations\n", section->NumberOfRelocations);
	printf("% 8X number of line numbers\n", section->NumberOfLinenumbers);
	DWORD Chars = section->Characteristics;
	printf("% 8X flags\n", Chars);
	for (int j = 0; j < NUMBER_SECTION_CHARACTERISTICS; j++)
	{
		DWORD flag = SectionCharacteristics[j].flag;

		if (Chars & flag)
		{
			printf("         %s\n", SectionCharacteristics[j].name);
			Chars &= ~flag;

			if (Chars == 0)
			{
				break;
			}
		}
	}
	printf("\n");
	if (section->PointerToRawData > 0 && section->SizeOfRawData > 0 && ptr->sectionBuffer != nullptr)
	{
		printf("RAW DATA #%X (%d)\n", i, i);
		if (strcmp((const char*)section->Name, ".text") == 0)
		{
			if (machine == IMAGE_FILE_MACHINE_AMD64)
			{
				printf("RAW DATA #%X (%d) - %ld bytes\n", i, i, section->SizeOfRawData);
				csh handle;
				cs_insn* insn;
				size_t count;

				// Initialize Capstone for X86 64-bit
				if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
					return;

				// Disassemble the code buffer
				count = cs_disasm(handle, (unsigned char*)ptr->sectionBuffer, section->SizeOfRawData - 1, 0x0000, 0, &insn);
				if (count > 0)
				{
					size_t j;
					for (j = 0; j < count; j++)
					{
						printf("0x%lx:\t%s\t%s\n", (unsigned long)insn[j].address, insn[j].mnemonic, insn[j].op_str);
					}
					// Free allocated memory for instructions
					cs_free(insn, count);
				}
				else
				{
					printf("Failed to disassemble given code!\n");
				}

				// Close the handle
				cs_close(&handle);
			}
			if (machine == IMAGE_FILE_MACHINE_I386)
			{
				printf("RAW DATA #%X (%d) - %ld bytes\n", i, i, section->SizeOfRawData);
				csh handle;
				cs_insn* insn;
				size_t count;

				// Initialize Capstone for X86 32-bit
				if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) != CS_ERR_OK)
					return;

				// Disassemble the code buffer
				count = cs_disasm(handle, (unsigned char*)ptr->sectionBuffer, section->SizeOfRawData - 1, 0x0000, 0, &insn);
				if (count > 0)
				{
					size_t j;
					for (j = 0; j < count; j++)
					{
						printf("0x%lx:\t%s\t%s\n", (unsigned long)insn[j].address, insn[j].mnemonic, insn[j].op_str);
					}
					// Free allocated memory for instructions
					cs_free(insn, count);
				}
				else
				{
					printf("Failed to disassemble given code!\n");
				}

				// Close the handle
				cs_close(&handle);
			}
		}
		else
		{
			hexdump(ptr->sectionBuffer, section->SizeOfRawData, 0);
		}
	}
	if (section->PointerToRelocations > 0 && section->NumberOfRelocations > 0 && ptr->relocation != nullptr)
	{
		char szTypeName[32];
		printf("SECTION HEADER #%X (%d) RELOCATIONS\n", i, i);
		for (int k = 0; k < section->NumberOfRelocations; k++)
		{
			GetObjRelocationName(ptr->relocation[k].Type, szTypeName, sizeof(szTypeName));
			printf("  Address: %08X  SymIndex: %08X  Type: %s\n", ptr->relocation[k].VirtualAddress, ptr->relocation[k].SymbolTableIndex, szTypeName);
		}
	}
	if (section->PointerToLinenumbers > 0 && section->NumberOfLinenumbers > 0 && ptr->lineNumbers != nullptr)
	{
		for (int j = 0; j < section->NumberOfLinenumbers; j++)
		{
			if (ptr->lineNumbers[j].Linenumber == 0)
			{
				printf("Symbol Table Index: %08X  Line: %04u\n", ptr->lineNumbers[j].Type.SymbolTableIndex, ptr->lineNumbers[j].Linenumber);
			}
			else
			{
				printf("Virtual Address   : %08X  Line: %04u\n", ptr->lineNumbers[j].Type.VirtualAddress, ptr->lineNumbers[j].Linenumber);
			}

		}
	}
	printf("\n\n");
}

void GetSectionName(WORD section, PSTR buffer, unsigned cbBuffer)
{
	char tempbuffer[10];

	switch ((SHORT)section)
	{
		case IMAGE_SYM_UNDEFINED:
			strcpy(tempbuffer, "UNDEF");
			break;
		case IMAGE_SYM_ABSOLUTE:
			strcpy(tempbuffer, "ABS");
			break;
		case IMAGE_SYM_DEBUG:
			strcpy(tempbuffer, "DEBUG");
			break;
		default:
			sprintf(tempbuffer, "%X", section);
	}
	strncpy(buffer, tempbuffer, cbBuffer - 1);
}

void DumpSymbolTable(COFFSymbolTable* pSymTab)
{
	printf("Symbol Table - %d entries  (* = auxillary symbol)\n", pSymTab->GetNumberOfSymbols());
	printf("Indx Sectn   Value   Type  Storage Name\n");
	printf("---- ----- -------- ----- -------  --------\n");
	PCOFFSymbol pSymbol = pSymTab->GetNextSymbol(0);
	while (pSymbol)
	{
		char szSection[10];
		GetSectionName(pSymbol->GetSectionNumber(), szSection, sizeof(szSection));
		printf("%04X %5s %08X  %s %-8s %s\n", pSymbol->GetIndex(), szSection, pSymbol->GetValue(), pSymbol->GetTypeName(), pSymbol->GetStorageClassName(), pSymbol->GetName());
		if (pSymbol->GetNumberOfAuxSymbols())
		{
			char szAuxSymbol[1024];

			if (pSymbol->GetAuxSymbolAsString(szAuxSymbol, sizeof(szAuxSymbol)))
			{
				printf("     * %s\n", szAuxSymbol);
			}
		}
		pSymbol = pSymTab->GetNextSymbol(pSymbol);
	}
}

void DumpDOSHeader(DosHeaderPtr dosHeader)
{
	printf("DOS HEADER VALUES\n");
	printf("  % 16X Magic number\n", dosHeader->e_magic);
	printf("  % 16X Bytes on last page of file\n", dosHeader->e_cblp);
	printf("  % 16X Pages in file\n", dosHeader->e_cp);
	printf("  % 16X Relocations\n", dosHeader->e_crlc);
	printf("  % 16X Size of header in paragraphs\n", dosHeader->e_cparhdr);
	printf("  % 16X Minimum extra paragraphs needed\n", dosHeader->e_minalloc);
	printf("  % 16X Maximum extra paragraphs needed\n", dosHeader->e_maxalloc);
	printf("  % 16X Initial (relative) SS value\n", dosHeader->e_ss);
	printf("  % 16X Initial SP value\n", dosHeader->e_sp);
	printf("  % 16X Checksum\n", dosHeader->e_csum);
	printf("  % 16X Initial IP value\n", dosHeader->e_ip);
	printf("  % 16X Initial (relative) CS value\n", dosHeader->e_cs);
	printf("  % 16X File address of relocation table\n", dosHeader->e_lfarlc);
	printf("  % 16X Overlay number\n", dosHeader->e_ovno);
	printf("  % 16X OEM identifier (for e_oeminfo)\n", dosHeader->e_oemid);
	printf("  % 16X OEM information e_oemid specific\n", dosHeader->e_oeminfo);
	printf("  % 16X (%lu) File address of new exe header\n", dosHeader->e_lfanew, dosHeader->e_lfanew);
}

void DumpFileHeader(NTFileHeaderPtr pImageFileHeader)
{
	const char* mt = GetMachineTypeName(pImageFileHeader->Machine);
	char* time = get_ctime_stg((time_t*)&pImageFileHeader->TimeDateStamp);
	printf("FILE HEADER VALUES\n");
	printf("  % 16X Machine (%s)\n", pImageFileHeader->Machine, mt);
	printf("  % 16X (%d) Number of Sections\n", pImageFileHeader->NumberOfSections, pImageFileHeader->NumberOfSections);
	printf("  % 16X TimeDateStamp ->  %s", pImageFileHeader->TimeDateStamp, (time == NULL) ? ("n/a\n") : (time));
	printf("  % 16X PointerToSymbolTable\n", pImageFileHeader->PointerToSymbolTable);
	printf("  % 16X (%d) NumberOfSymbols\n", pImageFileHeader->NumberOfSymbols, pImageFileHeader->NumberOfSymbols);
	printf("  % 16X (%d) SizeOfOptionalHeader\n", pImageFileHeader->SizeOfOptionalHeader, pImageFileHeader->SizeOfOptionalHeader);
	WORD Chars = pImageFileHeader->Characteristics;
	printf("  % 16X Flags\n", Chars);
	for (int i = 0; i < NUMBER_IMAGE_HEADER_FLAGS; i++)
	{
		WORD flag = ImageFileHeaderCharacteristics[i].flag;
		if (Chars & flag)
		{
			printf("                   %s\n", ImageFileHeaderCharacteristics[i].name);
			Chars &= ~flag;
			if (Chars == 0)
				break;
		}
	}
	printf("\n");
}

void DumpOptionalHeader64(NTOptionalHeader64Ptr optionalHeader)
{
	printf("OPTIONAL HEADER VALUES (64-bit)\n");
	printf("  % 16X magic\n", optionalHeader->Magic);
	printf("  % 13u.%02u linker version\n", optionalHeader->MajorLinkerVersion, optionalHeader->MinorLinkerVersion);
	printf("  % 16X (%ld) size of code\n", optionalHeader->SizeOfCode, optionalHeader->SizeOfCode);
	printf("  % 16X (%ld) size of initialized data\n", optionalHeader->SizeOfInitializedData, optionalHeader->SizeOfInitializedData);
	printf("  % 16X (%ld) size of uninitialized data\n", optionalHeader->SizeOfUninitializedData, optionalHeader->SizeOfUninitializedData);
	printf("  % 16X entry point\n", optionalHeader->AddressOfEntryPoint);
	printf("  % 16X base of code\n", optionalHeader->BaseOfCode);
	printf("  % 16llX image base (%llX to %llX)\n", (LONGLONG)optionalHeader->ImageBase, (LONGLONG)optionalHeader->ImageBase, (LONGLONG)(optionalHeader->ImageBase + optionalHeader->SizeOfImage - 1));
	printf("  % 16X section alignment\n", optionalHeader->SectionAlignment);
	printf("  % 16X file alignment\n", optionalHeader->FileAlignment);
	printf("  % 13u.%02u operating system version\n", optionalHeader->MajorOperatingSystemVersion, optionalHeader->MinorOperatingSystemVersion);
	printf("  % 13u.%02u image version\n", optionalHeader->MajorImageVersion, optionalHeader->MinorImageVersion);
	printf("  % 13u.%02u subsystem version\n", optionalHeader->MajorSubsystemVersion, optionalHeader->MinorSubsystemVersion);
	printf("  % 16X Win32 version\n", optionalHeader->Win32VersionValue);
	printf("  % 16X size of image\n", optionalHeader->SizeOfImage);
	printf("  % 16X size of headers\n", optionalHeader->SizeOfHeaders);
	printf("  % 16X checksum\n", optionalHeader->Win32VersionValue);
	const char* s = "Uknown";
	switch (optionalHeader->Subsystem)
	{
		case IMAGE_SUBSYSTEM_NATIVE: s = "Native"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_GUI: s = "Windows GUI"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_CUI: s = "Windows character"; break;
		case IMAGE_SUBSYSTEM_OS2_CUI: s = "OS/2 character"; break;
		case IMAGE_SUBSYSTEM_POSIX_CUI: s = "Posix character"; break;
	}
	printf("  % 16X subsystem (%s)\n", optionalHeader->Subsystem, s);
	printf("  % 16X DLL characteristics\n", optionalHeader->DllCharacteristics);
	for (int i = 0; i < NUMBER_DLL_CHARACTERISTICS; i++)
	{
		if (optionalHeader->DllCharacteristics & DllCharacteristics[i].flag)
			printf("                  %s\n", DllCharacteristics[i].name);
	}
	printf("  % 16llX size of stack reserve\n", optionalHeader->SizeOfStackReserve);
	printf("  % 16llX size of stack commit\n", optionalHeader->SizeOfStackCommit);
	printf("  % 16llX size of heap reserve\n", optionalHeader->SizeOfHeapReserve);
	printf("  % 16llX size of heap commit\n", optionalHeader->SizeOfHeapCommit);
	printf("  % 16X loader flags\n", optionalHeader->LoaderFlags);
	printf("  % 16X number of directories\n", optionalHeader->NumberOfRvaAndSizes);
	for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
	{
		const char* name = "???";
		switch (i)
		{
			case IMAGE_DIRECTORY_ENTRY_EXPORT:name = "Export"; break;
			case IMAGE_DIRECTORY_ENTRY_IMPORT:name = "Import"; break;
			case IMAGE_DIRECTORY_ENTRY_RESOURCE:name = "Resource"; break;
			case IMAGE_DIRECTORY_ENTRY_EXCEPTION:name = "Exception"; break;
			case IMAGE_DIRECTORY_ENTRY_SECURITY:name = "Security"; break;
			case IMAGE_DIRECTORY_ENTRY_BASERELOC:name = "Base Relocation"; break;
			case IMAGE_DIRECTORY_ENTRY_DEBUG:name = "Debug"; break;
			case IMAGE_DIRECTORY_ENTRY_ARCHITECTURE:name = "Architecture"; break;
			case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:name = "Global Pointer"; break;
			case IMAGE_DIRECTORY_ENTRY_TLS:name = "TLS"; break;
			case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG:name = "Load Configuration"; break;
			case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:name = "Bound Import"; break;
			case IMAGE_DIRECTORY_ENTRY_IAT:name = "Import Address Table"; break;
			case IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT:name = "Delay Import"; break;
			case IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR:name = "COM Descriptor"; break;
			case 15:name = "Reserved"; break;
		}
		printf("            %-22s rva: %08X  size: %08X (%lu)\n", name, optionalHeader->DataDirectory[i].VirtualAddress, optionalHeader->DataDirectory[i].Size, optionalHeader->DataDirectory[i].Size);
	}
}

void DumpOptionalHeader32(NTOptionalHeader32Ptr optionalHeader)
{
	printf("OPTIONAL HEADER VALUES (32-bit)\n");
	printf("  % 16X magic\n", optionalHeader->Magic);
	printf("  % 13u.%02u linker version\n", optionalHeader->MajorLinkerVersion, optionalHeader->MinorLinkerVersion);
	printf("  % 16X (%ld) size of code\n", optionalHeader->SizeOfCode, optionalHeader->SizeOfCode);
	printf("  % 16X (%ld) size of initialized data\n", optionalHeader->SizeOfInitializedData, optionalHeader->SizeOfInitializedData);
	printf("  % 16X (%ld) size of uninitialized data\n", optionalHeader->SizeOfUninitializedData, optionalHeader->SizeOfUninitializedData);
	printf("  % 16X entry point\n", optionalHeader->AddressOfEntryPoint);
	printf("  % 16X base of code\n", optionalHeader->BaseOfCode);
	printf("  % 16llX image base (%llX to %llX)\n", (LONGLONG)optionalHeader->ImageBase, (LONGLONG)optionalHeader->ImageBase, (LONGLONG)(optionalHeader->ImageBase + optionalHeader->SizeOfImage - 1));
	printf("  % 16X section alignment\n", optionalHeader->SectionAlignment);
	printf("  % 16X file alignment\n", optionalHeader->FileAlignment);
	printf("  % 13u.%02u operating system version\n", optionalHeader->MajorOperatingSystemVersion, optionalHeader->MinorOperatingSystemVersion);
	printf("  % 13u.%02u image version\n", optionalHeader->MajorImageVersion, optionalHeader->MinorImageVersion);
	printf("  % 13u.%02u subsystem version\n", optionalHeader->MajorSubsystemVersion, optionalHeader->MinorSubsystemVersion);
	printf("  % 16X Win32 version\n", optionalHeader->Win32VersionValue);
	printf("  % 16X size of image\n", optionalHeader->SizeOfImage);
	printf("  % 16X size of headers\n", optionalHeader->SizeOfHeaders);
	printf("  % 16X checksum\n", optionalHeader->Win32VersionValue);
	const char* s = "Uknown";
	switch (optionalHeader->Subsystem)
	{
		case IMAGE_SUBSYSTEM_NATIVE: s = "Native"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_GUI: s = "Windows GUI"; break;
		case IMAGE_SUBSYSTEM_WINDOWS_CUI: s = "Windows character"; break;
		case IMAGE_SUBSYSTEM_OS2_CUI: s = "OS/2 character"; break;
		case IMAGE_SUBSYSTEM_POSIX_CUI: s = "Posix character"; break;
	}
	printf("  % 16X subsystem (%s)\n", optionalHeader->Subsystem, s);
	printf("  % 16X DLL characteristics\n", optionalHeader->DllCharacteristics);
	for (int i = 0; i < NUMBER_DLL_CHARACTERISTICS; i++)
	{
		if (optionalHeader->DllCharacteristics & DllCharacteristics[i].flag)
			printf("                   %s\n", DllCharacteristics[i].name);
	}
	printf("  % 16X size of stack reserve\n", optionalHeader->SizeOfStackReserve);
	printf("  % 16X size of stack commit\n", optionalHeader->SizeOfStackCommit);
	printf("  % 16X size of heap reserve\n", optionalHeader->SizeOfHeapReserve);
	printf("  % 16X size of heap commit\n", optionalHeader->SizeOfHeapCommit);
	printf("  % 16X loader flags\n", optionalHeader->LoaderFlags);
	printf("  % 16X number of directories\n", optionalHeader->NumberOfRvaAndSizes);
	for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
	{
		const char* name = "???";
		switch (i)
		{
			case IMAGE_DIRECTORY_ENTRY_EXPORT:name = "Export"; break;
			case IMAGE_DIRECTORY_ENTRY_IMPORT:name = "Import"; break;
			case IMAGE_DIRECTORY_ENTRY_RESOURCE:name = "Resource"; break;
			case IMAGE_DIRECTORY_ENTRY_EXCEPTION:name = "Exception"; break;
			case IMAGE_DIRECTORY_ENTRY_SECURITY:name = "Security"; break;
			case IMAGE_DIRECTORY_ENTRY_BASERELOC:name = "Base Relocation"; break;
			case IMAGE_DIRECTORY_ENTRY_DEBUG:name = "Debug"; break;
			case IMAGE_DIRECTORY_ENTRY_ARCHITECTURE:name = "Architecture"; break;
			case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:name = "Global Pointer"; break;
			case IMAGE_DIRECTORY_ENTRY_TLS:name = "TLS"; break;
			case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG:name = "Load Configuration"; break;
			case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:name = "Bound Import"; break;
			case IMAGE_DIRECTORY_ENTRY_IAT:name = "Import Address Table"; break;
			case IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT:name = "Delay Import"; break;
			case IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR:name = "COM Descriptor"; break;
			case 15:name = "Reserved"; break;
		}
		printf("            %-22s rva: %08X  size: %08X (%lu)\n", name, optionalHeader->DataDirectory[i].VirtualAddress, optionalHeader->DataDirectory[i].Size, optionalHeader->DataDirectory[i].Size);
	}
}

void DumpExportDirectory(ExportsPtr exportDir)
{
	printf("Exports table:\n");
	printf("  Name:            %s\n", exportDir->filename);
	printf("  Characteristics: %08X\n", exportDir->exports.Characteristics);
	printf("  TimeDateStamp:   %08X -> %s", exportDir->exports.TimeDateStamp, get_ctime_stg((time_t*)&exportDir->exports.TimeDateStamp));
	printf("  Version:         %u.%02u\n", exportDir->exports.MajorVersion, exportDir->exports.MinorVersion);
	printf("  Ordinal base:    %08X\n", exportDir->exports.Base);
	printf("  # of functions:  %08X\n", exportDir->exports.NumberOfFunctions);
	printf("  # of Names:      %08X\n", exportDir->exports.NumberOfNames);
	printf("\n  Ordinal   hint   RVA  Name\n");
	int i = 0;
	for (ExportsFunctionsPtr item : exportDir->functions)
	{
		printf("  % 7X  % 4u  % 5X  %s\n", item->ordinal, i, item->entryPoint, item->filename);
		i++;
	}
	printf("\n");
}

void DumpImportDirectory(bool is64, vector<ImportsPtr>* imports)
{
	printf("Exports table:\n");
	for (ImportsPtr ptr : *imports)
	{
		printf("  Name:            %s\n", ptr->filename);
		printf("  OrigFirstThunk:  %08X\n", ptr->imports.Characteristics);
		printf("  TimeDateStamp:   %08X -> %s", ptr->imports.TimeDateStamp, get_ctime_stg((time_t*)&ptr->imports.TimeDateStamp));
		printf("  ForwarderChain:  %08X\n", ptr->imports.ForwarderChain);
		printf("  First thunk RVA: %08X\n", ptr->imports.FirstThunk);
		printf("  Ordn  Name\n");
		int i = 0;
		if (is64)
		{
			for (Thunk64Ptr thunk : ptr->thunk64)
			{
				if (thunk->thunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64)
				{
					printf("  % 8llu", IMAGE_ORDINAL64(thunk->thunk.u1.Ordinal));
				}
				else
				{
					printf("  %4u  %s", thunk->ordinalname->Hint, thunk->ordinalname->Name);
				}
				if (ptr->imports.TimeDateStamp == 0)
				{
					printf(" (Bound to: %08llX)", ptr->thunkIAT64[i]->thunk.u1.Function);
				}
				printf("\n");
				i++;
			}
		}
		else
		{
			for (Thunk32Ptr thunk : ptr->thunk32)
			{
				if (thunk->thunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64)
				{
					printf("  % 8lu", IMAGE_ORDINAL64(thunk->thunk.u1.Ordinal));
				}
				else if (thunk->ordinalname != nullptr)
				{
					printf("  %4u  %s", thunk->ordinalname->Hint, thunk->ordinalname->Name);
				}
				if (ptr->imports.TimeDateStamp == 0)
				{
					printf(" (Bound to: %08lX)", ptr->thunkIAT32[i]->thunk.u1.Function);
				}
				printf("\n");
				i++;
			}
		}
		printf("\n");
	}
}

void DumpResourcesDirectory(ResourcesPtr resources)
{
	printf("Resources table:\n");
	printf("  Characteristics:         %08X\n", resources->header.Characteristics);
	printf("  TimeDateStamp:           %08X -> %s", resources->header.TimeDateStamp, get_ctime_stg((time_t*)&resources->header.TimeDateStamp));
	printf("  Version:                 %u.%02u\n", resources->header.MajorVersion, resources->header.MinorVersion);
	printf("  Number of Named Entries: %04X (%u)\n", resources->header.NumberOfNamedEntries, resources->header.NumberOfNamedEntries);
	printf("  Number of Id Entries:    %04X (%u)\n\n", resources->header.NumberOfIdEntries, resources->header.NumberOfIdEntries);
	for (ResourcesEntryPtr ptr : resources->entries)
	{
		printf("  Name Offset:             %04X\n", ptr->entry.NameOffset);
		printf("  Name Is String:          %04X\n", ptr->entry.NameIsString);
		printf("  Name:                    %04X\n", ptr->entry.Name);
		printf("  Id:                      %04X\n", ptr->entry.Id);
		printf("  Offset To Data:          %08X\n", ptr->entry.OffsetToData);
		printf("  Offset To Directory:     %08X\n", ptr->entry.OffsetToDirectory);
		printf("  Data is Directory:       %08X\n", ptr->entry.DataIsDirectory);
	}
	printf("\n");
}

void DumpBaseRelocationsDirectory(vector<RelocsPtr>* relocs)
{
	if (relocs->size() == 0)
	{
		printf("No relocations table present.\n");
	}
	else
	{
		printf("Relocations table:\n");
		for (RelocsPtr ptr : *relocs)
		{
			printf("  Page RVA:      %08X\n", ptr->baseReloc.VirtualAddress);
			printf("  Block Size:    %08X (%d)\n", ptr->baseReloc.SizeOfBlock, ptr->baseReloc.SizeOfBlock);
			for (RelocsEntryPtr entry : ptr->entries)
			{
				WORD relocType = (entry->relocType & 0xF000) >> 12;
				printf("  %08X %s", (entry->relocType & 0x0FFF) + ptr->baseReloc.VirtualAddress, entry->szRelocType);
				if (IMAGE_REL_BASED_HIGHADJ == relocType)
				{
					printf(" (%X)", entry->relocType);
				}
				printf("\n");
			}
			printf("\n");
		}
	}

}

void DumpDebugDirectory(DebugPtr debug)
{
	printf(
		"Debug Formats in File\n"
		"  Type            Type     Size     Address  FilePtr  Charactr Version  TimeDate\n"
		"  --------------- -------- -------- -------- -------- -------- -------- --------\n"
	);
	if (debug == nullptr) return;
	for (DebugEntryPtr ptr : debug->entries)
	{
		printf("  %-15s %08X %08X %08X %08X %08X %u.%02u     %s",
			ptr->debugFormat,
			ptr->entry.Type,
			ptr->entry.SizeOfData,
			ptr->entry.AddressOfRawData,
			ptr->entry.PointerToRawData,
			ptr->entry.Characteristics,
			ptr->entry.MajorVersion,
			ptr->entry.MinorVersion,
			get_ctime_stg((time_t*)&ptr->entry.TimeDateStamp)
		);
	}
	printf("\n");
}

void DumpLoadConfig32Directory(NTLoadConfigDirectory32Ptr load32)
{
	if (load32 == NULL)
	{
		printf("No LoadConfig32 table present.\n");
	}
	else
	{
		printf("LoadConfig32 table:\n");
		printf("%08lX Size\n", load32->Size);
		printf("%08X TimeDateStamp\n", load32->TimeDateStamp);
		printf("% 5u.%02u MajorVersion\n", load32->MajorVersion, load32->MinorVersion);
		printf("%08X GlobalFlagsClear\n", load32->GlobalFlagsClear);
		printf("%08X GlobalFlagsSet\n", load32->GlobalFlagsSet);
		printf("%08X CriticalSectionDefaultTimeout\n", load32->CriticalSectionDefaultTimeout);
		printf("%08X DeCommitFreeBlockThreshold\n", load32->DeCommitFreeBlockThreshold);
		printf("%08X DeCommitTotalFreeThreshold\n", load32->DeCommitTotalFreeThreshold);
		printf("%08X LockPrefixTable\n", load32->LockPrefixTable);
		printf("%08X MaximumAllocationSize\n", load32->MaximumAllocationSize);
		printf("%08X VirtualMemoryThreshold\n", load32->VirtualMemoryThreshold);
		printf("%08X ProcessHeapFlags\n", load32->ProcessHeapFlags);
		printf("%08X ProcessAffinityMask\n", load32->ProcessAffinityMask);
		printf("%08X CSDVersion\n", load32->CSDVersion);
		printf("%08X DependentLoadFlags\n", load32->DependentLoadFlags);
		printf("%08X EditList\n", load32->EditList);
		printf("%08X SecurityCookie\n", load32->SecurityCookie);
		printf("%08X SEHandlerTable\n", load32->SEHandlerTable);
		printf("%08X SEHandlerCount\n", load32->Size);
		printf("%08X GuardCFCheckFunctionPointer\n", load32->SEHandlerCount);
		printf("%08X GuardCFDispatchFunctionPointer\n", load32->GuardCFDispatchFunctionPointer);
		printf("%08X GuardCFFunctionTable\n", load32->GuardCFFunctionTable);
		printf("%08X GuardCFFunctionCount\n", load32->GuardCFFunctionCount);
		printf("%08X GuardFlags\n", load32->GuardFlags);
		printf("%08X GuardAddressTakenIatEntryTable\n", load32->GuardAddressTakenIatEntryTable);
		printf("%08X GuardAddressTakenIatEntryCount\n", load32->GuardAddressTakenIatEntryCount);
		printf("%08X GuardLongJumpTargetTable\n", load32->GuardLongJumpTargetTable);
		printf("%08X GuardLongJumpTargetCount\n", load32->GuardLongJumpTargetCount);
		printf("%08X DynamicValueRelocTable\n", load32->DynamicValueRelocTable);
		printf("%08X CHPEMetadataPointer\n", load32->CHPEMetadataPointer);
		printf("%08X GuardRFFailureRoutine\n", load32->GuardRFFailureRoutine);
		printf("%08X GuardRFFailureRoutineFunctionPointer\n", load32->GuardRFFailureRoutineFunctionPointer);
		printf("%08X DynamicValueRelocTableOffset\n", load32->DynamicValueRelocTableOffset);
		printf("%08X DynamicValueRelocTableSection\n", load32->DynamicValueRelocTableSection);
		printf("%08X Reserved2\n", load32->Reserved2);
		printf("%08X GuardRFVerifyStackPointerFunctionPointer\n", load32->GuardRFVerifyStackPointerFunctionPointer);
		printf("%08X HotPatchTableOffset\n", load32->HotPatchTableOffset);
		printf("%08X Reserved3\n", load32->Reserved3);
		printf("%08X EnclaveConfigurationPointer\n", load32->EnclaveConfigurationPointer);
		printf("%08X VolatileMetadataPointer\n", load32->VolatileMetadataPointer);
		printf("%08X GuardEHContinuationTable\n", load32->GuardEHContinuationTable);
		printf("%08X GuardEHContinuationCount\n", load32->GuardEHContinuationCount);
		printf("%08X GuardXFGCheckFunctionPointer\n", load32->GuardXFGCheckFunctionPointer);
		printf("%08X GuardXFGDispatchFunctionPointer\n", load32->GuardXFGDispatchFunctionPointer);
		printf("%08X GuardXFGTableDispatchFunctionPointer\n", load32->GuardXFGTableDispatchFunctionPointer);
		printf("%08X CastGuardOsDeterminedFailureMode\n", load32->CastGuardOsDeterminedFailureMode);
		printf("%08X GuardMemcpyFunctionPointer\n", load32->GuardMemcpyFunctionPointer);
		printf("%08X UmaFunctionPointers\n", load32->UmaFunctionPointers);
	}
}

void DumpLoadConfig64Directory(NTLoadConfigDirectory64Ptr load64)
{
	if (load64 == NULL)
	{
		printf("No LoadConfig64 table present.\n");
	}
	else
	{
		printf("LoadConfig64 table:\n");
		printf("%08X Size\n", load64->Size);
		printf("%08X TimeDateStamp\n", load64->TimeDateStamp);
		printf("% 5u.%02u MajorVersion\n", load64->MajorVersion, load64->MinorVersion);
		printf("%08X GlobalFlagsClear\n", load64->GlobalFlagsClear);
		printf("%08X GlobalFlagsSet\n", load64->GlobalFlagsSet);
		printf("%08X CriticalSectionDefaultTimeout\n", load64->CriticalSectionDefaultTimeout);
		printf("%016llX DeCommitFreeBlockThreshold\n", load64->DeCommitFreeBlockThreshold);
		printf("%016llX DeCommitTotalFreeThreshold\n", load64->DeCommitTotalFreeThreshold);
		printf("%016llX LockPrefixTable\n", load64->LockPrefixTable);
		printf("%016llX MaximumAllocationSize\n", load64->MaximumAllocationSize);
		printf("%016llX VirtualMemoryThreshold\n", load64->VirtualMemoryThreshold);
		printf("%016X ProcessHeapFlags\n", load64->ProcessHeapFlags);
		printf("%016llX ProcessAffinityMask\n", load64->ProcessAffinityMask);
		printf("%08X CSDVersion\n", load64->CSDVersion);
		printf("%08X DependentLoadFlags\n", load64->DependentLoadFlags);
		printf("%016llX EditList\n", load64->EditList);
		printf("%016llX SecurityCookie\n", load64->SecurityCookie);
		printf("%016llX SEHandlerTable\n", load64->SEHandlerTable);
		printf("%08X SEHandlerCount\n", load64->Size);
		printf("%016llX GuardCFCheckFunctionPointer\n", load64->SEHandlerCount);
		printf("%016llX GuardCFDispatchFunctionPointer\n", load64->GuardCFDispatchFunctionPointer);
		printf("%016llX GuardCFFunctionTable\n", load64->GuardCFFunctionTable);
		printf("%016llX GuardCFFunctionCount\n", load64->GuardCFFunctionCount);
		printf("%08X GuardFlags\n", load64->GuardFlags);
		printf("%016llX GuardAddressTakenIatEntryTable\n", load64->GuardAddressTakenIatEntryTable);
		printf("%016llX GuardAddressTakenIatEntryCount\n", load64->GuardAddressTakenIatEntryCount);
		printf("%016llX GuardLongJumpTargetTable\n", load64->GuardLongJumpTargetTable);
		printf("%016llX GuardLongJumpTargetCount\n", load64->GuardLongJumpTargetCount);
		printf("%016llX DynamicValueRelocTable\n", load64->DynamicValueRelocTable);
		printf("%016llX CHPEMetadataPointer\n", load64->CHPEMetadataPointer);
		printf("%016llX GuardRFFailureRoutine\n", load64->GuardRFFailureRoutine);
		printf("%016llX GuardRFFailureRoutineFunctionPointer\n", load64->GuardRFFailureRoutineFunctionPointer);
		printf("%08X DynamicValueRelocTableOffset\n", load64->DynamicValueRelocTableOffset);
		printf("%08X DynamicValueRelocTableSection\n", load64->DynamicValueRelocTableSection);
		printf("%08X Reserved2\n", load64->Reserved2);
		printf("%016llX GuardRFVerifyStackPointerFunctionPointer\n", load64->GuardRFVerifyStackPointerFunctionPointer);
		printf("%08X HotPatchTableOffset\n", load64->HotPatchTableOffset);
		printf("%08X Reserved3\n", load64->Reserved3);
		printf("%016llX EnclaveConfigurationPointer\n", load64->EnclaveConfigurationPointer);
		printf("%016llX VolatileMetadataPointer\n", load64->VolatileMetadataPointer);
		printf("%016llX GuardEHContinuationTable\n", load64->GuardEHContinuationTable);
		printf("%016llX GuardEHContinuationCount\n", load64->GuardEHContinuationCount);
		printf("%016llX GuardXFGCheckFunctionPointer\n", load64->GuardXFGCheckFunctionPointer);
		printf("%016llX GuardXFGDispatchFunctionPointer\n", load64->GuardXFGDispatchFunctionPointer);
		printf("%016llX GuardXFGTableDispatchFunctionPointer\n", load64->GuardXFGTableDispatchFunctionPointer);
		printf("%016llX CastGuardOsDeterminedFailureMode\n", load64->CastGuardOsDeterminedFailureMode);
		printf("%016llX GuardMemcpyFunctionPointer\n", load64->GuardMemcpyFunctionPointer);
		printf("%016llX UmaFunctionPointers\n", load64->UmaFunctionPointers);
	}
}

void DumpArchiveMemberHeader(PIMAGE_ARCHIVE_MEMBER_HEADER pArchHeader)
{
	printf("Archive Member Header:\n");
	printf("  Name:     %.16s\n", pArchHeader->Name);
	char szDateAsLong[64];
	sprintf(szDateAsLong, "%.12s", pArchHeader->Date);
	time_t dateAsLong = atol(szDateAsLong);

	printf("  Date:     %.12s -> %s", pArchHeader->Date, get_ctime_stg(&dateAsLong));
	printf("  UserID:   %.6s\n", pArchHeader->UserID);
	printf("  GroupID:  %.6s\n", pArchHeader->GroupID);
	printf("  Mode:     %.8s\n", pArchHeader->Mode);
	printf("  Size:     %.10s\n", pArchHeader->Size);
}

void DumpFirstLinkerMember(vector<LIBFileLinkerMembersPtr>* list)
{
	printf("First Linker Member:\n");
#ifdef _WIN64
	printf("  Symbols:         %08llX\n", list->size());
#else
	printf("  Symbols:         %08lX\n", list->size());
#endif
	printf("  MbrOffs   Name\n  --------  ----\n");
	for (LIBFileLinkerMembersPtr ptr : *list)
	{
		printf("  %08lX  %s\n", ptr->offset, ptr->pSymbolName);
	}
}

void DumpSecondLinkerMember(vector<LIBFileLinkerMembersPtr>* list)
{
	printf("Second Linker Member:\n");
#ifdef _WIN64
	printf("  Symbols:         %08llX\n", list->size());
#else
	printf("  Symbols:         %08lX\n", list->size());
#endif
	printf("  MbrOffs   Name\n  --------  ----\n");
	for (LIBFileLinkerMembersPtr ptr : *list)
	{
		printf("  %08lX  %s\n", ptr->offset, ptr->pSymbolName);
	}
}

void DumpThirdLinkerMember(vector<LIBFileLinkerMembersPtr>* list)
{
	printf("Long Nammes:\n");
#ifdef _WIN64
	printf("  Symbols:         %08llX\n", list->size());
#else
	printf("  Symbols:         %08lX\n", list->size());
#endif
	for (LIBFileLinkerMembersPtr ptr : *list)
	{
		printf("  %08lX  %s\n", ptr->offset, ptr->pSymbolName);
	}
}

void DumpLibFile(LIBFilePtr lib)
{
	int index = 0;
	for (LIBFileEntryPtr ptr : lib->entrys)
	{
		DumpArchiveMemberHeader(&ptr->header);
		switch (index)
		{
			case 0:
				DumpFirstLinkerMember(&ptr->first);
				break;
			case 1:
				DumpSecondLinkerMember(&ptr->second);
				break;
			case 2:
				DumpThirdLinkerMember(&ptr->longNames);
				break;
			default:
				DumpFileHeader(&ptr->objFile->header);
				printf("\n");
				for (int i = 0; i < ptr->objFile->sectionTable.size(); i++)
				{
					OBJSectionPtr section = ptr->objFile->sectionTable[i];
					DumpSection(i, section, ptr->objFile->header.Machine);
				}
				DumpSymbolTable(ptr->objFile->symbolTable);
				printf("\nString Table Size = 0x%0X (%ld) bytes %lld entries\n", ptr->objFile->stringTableSize, ptr->objFile->stringTableSize, (LONGLONG)ptr->objFile->stringTable.size());
				for (int i = 0; i < ptr->objFile->stringTable.size(); i++)
				{
					string s = ptr->objFile->stringTable[i];
					printf("stringtable[% 4d] = %s\n", i, s.c_str());
				}
				break;
		}
		index++;
	}
}

void DumpDOSObjFile(DOSOBJFilePtr obj)
{
	int offset = 0;
	for (DOSOBJRecordPtr ptr : obj->records)
	{
		switch (ptr->rectype)
		{
			case THEADR:
				printf("Type 0x%02X (THEADR) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case LHEADR:
				printf("Type 0x%02X (LHEADR) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case COMENT:
				printf("Type 0x%02X (COMENT) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case LNAMES:
				printf("Type 0x%02X (LNAMES) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case SEGDEF:
				printf("Type 0x%02X (SEGDEF) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case SEGDEF32:
				printf("Type 0x%02X (SEGDEF32) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case LEDATA:
				printf("Type 0x%02X (LEDATA) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case LEDATA32:
				printf("Type 0x%02X (lEDATA32) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case FIXUPP:
				printf("Type 0x%02X (FIXUPP) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case LEXTDEF:
				printf("Type 0x%02X (LEXTDEF) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			case EXTDEF:
				printf("Type 0x%02X (EXTDEF) : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
			default:
				printf("Type 0x%02X : length 0x%04lX (%ld)\n", ptr->rectype, ptr->reclength, ptr->reclength);
				break;
		}
		hexdump(ptr->buffer, ptr->reclength, offset + 3);
		printf("\n");
		offset += 3 + ptr->reclength;
	}
}