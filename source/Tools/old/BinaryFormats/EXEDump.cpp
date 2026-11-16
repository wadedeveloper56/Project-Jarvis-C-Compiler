#include "pch.h"
#include "framework.h"
#include "BinaryFormat.h"

using namespace std;

const char* SzDebugFormats[] = {
"UNKNOWN"                ,
"COFF"                   ,
"CODEVIEW"               ,
"FPO"                    ,
"MISC"                   ,
"EXCEPTION"              ,
"FIXUP"                  ,
"OMAP_TO_SRC"            ,
"OMAP_FROM_SRC"          ,
"BORLAND"                ,
"RESERVED10"             ,
"CLSID"                  ,
"VC_FEATURE"             ,
"POGO"                   ,
"ILTCG"                  ,
"MPX"                    ,
"REPRO"                  ,
"Undefined"              ,
"SPGO"                   ,
"Undefined"              ,
"EX_DLLCHARACTERISTICS"	 ,
};

const char* SzRelocTypes[] = { "ABSOLUTE","HIGH","LOW","HIGHLOW","HIGHADJ","MIPS/ARM", "SECTION","REL32","MIPS16","DIR64", "TYPE10" };
#define RELOC_TYPE_COUNT (sizeof(SzRelocTypes) / sizeof(char *))

DWORD GetImgDirEntryRVA(FileType fileType, PVOID pNTHdr, DWORD IDE)
{
	DWORD rva = 0;
	if (fileType == PE64EXE) {
		NTHeaders64Ptr p64 = (NTHeaders64Ptr)pNTHdr;
		rva = p64->OptionalHeader.DataDirectory[IDE].VirtualAddress;
	}
	else {
		NTHeaders32Ptr p32 = (NTHeaders32Ptr)pNTHdr;
		rva = p32->OptionalHeader.DataDirectory[IDE].VirtualAddress;
	}
	return rva;
}

NTSectionHeaderPtr GetSectionHeader(FileType fileType, PSTR name, PVOID pNTHeader)
{
	NTSectionHeaderPtr section;
	int numberOfSections;
	if (fileType == PE64EXE) {
		NTHeaders64Ptr p64 = (NTHeaders64Ptr)pNTHeader;
		section = PEEXE_FIRST_SECTION(NTSectionHeaderPtr, p64);
		numberOfSections = p64->FileHeader.NumberOfSections;
	}
	else {
		NTHeaders32Ptr p32 = (NTHeaders32Ptr)pNTHeader;
		section = PEEXE_FIRST_SECTION(NTSectionHeaderPtr, p32);
		numberOfSections = p32->FileHeader.NumberOfSections;
	}

	for (int i = 0; i < numberOfSections; i++, section++)
	{
		if (0 == strncmp((char*)section->Name, name, IMAGE_SIZEOF_SHORT_NAME))
			return section;
	}

	return 0;
}

DWORD GetImgDirEntrySize(FileType fileType, PVOID pNTHdr, DWORD IDE)
{
	DWORD size = 0;
	if (fileType == PE64EXE) {
		NTHeaders64Ptr p64 = (NTHeaders64Ptr)pNTHdr;
		size = p64->OptionalHeader.DataDirectory[IDE].Size;
	}
	else {
		NTHeaders32Ptr p32 = (NTHeaders32Ptr)pNTHdr;
		size = p32->OptionalHeader.DataDirectory[IDE].Size;
	}
	return size;

}

NTSectionHeaderPtr GetEnclosingSectionHeader(FileType fileType, DWORD rva, PVOID pNTHeader)
{
	NTSectionHeaderPtr section;
	int numberOfSections;
	if (fileType == PE64EXE) {
		NTHeaders64Ptr p64 = (NTHeaders64Ptr)pNTHeader;
		section = PEEXE_FIRST_SECTION(NTSectionHeaderPtr, p64);
		numberOfSections = p64->FileHeader.NumberOfSections;
	}
	else {
		NTHeaders32Ptr p32 = (NTHeaders32Ptr)pNTHeader;
		section = PEEXE_FIRST_SECTION(NTSectionHeaderPtr, p32);
		numberOfSections = p32->FileHeader.NumberOfSections;
	}

	for (int i = 0; i < numberOfSections; i++, section++)
	{
		if ((rva >= section->VirtualAddress) && (rva < (section->VirtualAddress + section->Misc.VirtualSize)))
			return section;
	}

	return 0;
}

LPVOID GetPtrFromRVA(FileType fileType, DWORD rva, NTHeaders32Ptr pNTHeader, char* imageBase)
{
	NTSectionHeaderPtr pSectionHdr = GetEnclosingSectionHeader(fileType, rva, pNTHeader);
	if (!pSectionHdr) return 0;
	int delta = (int)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
	return (PVOID)((BYTE*)imageBase + rva - delta);
}

void loadDOSStubEXE(EXEFilePtr result, DosHeaderPtr dosHeader)
{
	result->dosHeader.e_magic = dosHeader->e_magic;
	result->dosHeader.e_cblp = dosHeader->e_cblp;
	result->dosHeader.e_cp = dosHeader->e_cp;
	result->dosHeader.e_crlc = dosHeader->e_crlc;
	result->dosHeader.e_cparhdr = dosHeader->e_cparhdr;
	result->dosHeader.e_minalloc = dosHeader->e_minalloc;
	result->dosHeader.e_maxalloc = dosHeader->e_maxalloc;
	result->dosHeader.e_ss = dosHeader->e_ss;
	result->dosHeader.e_sp = dosHeader->e_sp;
	result->dosHeader.e_csum = dosHeader->e_csum;
	result->dosHeader.e_ip = dosHeader->e_ip;
	result->dosHeader.e_cs = dosHeader->e_cs;
	result->dosHeader.e_lfarlc = dosHeader->e_lfarlc;
	result->dosHeader.e_ovno = dosHeader->e_ovno;
	result->dosHeader.e_res[0] = dosHeader->e_res[0];
	result->dosHeader.e_res[1] = dosHeader->e_res[1];
	result->dosHeader.e_res[2] = dosHeader->e_res[2];
	result->dosHeader.e_res[3] = dosHeader->e_res[3];
	result->dosHeader.e_oemid = dosHeader->e_oemid;
	result->dosHeader.e_oeminfo = dosHeader->e_oeminfo;
	for (int j = 0; j < 10; j++) result->dosHeader.e_res2[j] = dosHeader->e_res2[j];
	result->dosHeader.e_lfanew = dosHeader->e_lfanew;
}

void loadPEHeaders(FileType fileType, EXEFilePtr result, NTHeaders32Ptr pImgFileHdr)
{
	result->Signature = pImgFileHdr->Signature;
	result->FileHeader.Machine = pImgFileHdr->FileHeader.Machine;
	result->FileHeader.NumberOfSections = pImgFileHdr->FileHeader.NumberOfSections;
	result->FileHeader.TimeDateStamp = pImgFileHdr->FileHeader.TimeDateStamp;
	result->FileHeader.PointerToSymbolTable = pImgFileHdr->FileHeader.PointerToSymbolTable;
	result->FileHeader.NumberOfSymbols = pImgFileHdr->FileHeader.NumberOfSymbols;
	result->FileHeader.SizeOfOptionalHeader = pImgFileHdr->FileHeader.SizeOfOptionalHeader;
	result->FileHeader.Characteristics = pImgFileHdr->FileHeader.Characteristics;
	NTOptionalHeader64Ptr opt64 = nullptr;
	NTOptionalHeader32Ptr opt32 = nullptr;
	if (pImgFileHdr->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
	{
		opt32 = (NTOptionalHeader32Ptr)&pImgFileHdr->OptionalHeader;
		result->OptionalHeader32.Magic = opt32->Magic;
		result->OptionalHeader32.MajorLinkerVersion = opt32->MajorLinkerVersion;
		result->OptionalHeader32.MinorLinkerVersion = opt32->MinorLinkerVersion;
		result->OptionalHeader32.SizeOfCode = opt32->SizeOfCode;
		result->OptionalHeader32.SizeOfInitializedData = opt32->SizeOfInitializedData;
		result->OptionalHeader32.SizeOfUninitializedData = opt32->SizeOfUninitializedData;
		result->OptionalHeader32.AddressOfEntryPoint = opt32->AddressOfEntryPoint;
		result->OptionalHeader32.BaseOfCode = opt32->BaseOfCode;
		result->OptionalHeader32.ImageBase = opt32->ImageBase;
		result->OptionalHeader32.SectionAlignment = opt32->SectionAlignment;
		result->OptionalHeader32.FileAlignment = opt32->FileAlignment;
		result->OptionalHeader32.MajorOperatingSystemVersion = opt32->MajorOperatingSystemVersion;
		result->OptionalHeader32.MinorOperatingSystemVersion = opt32->MinorOperatingSystemVersion;
		result->OptionalHeader32.MajorImageVersion = opt32->MajorImageVersion;
		result->OptionalHeader32.MinorImageVersion = opt32->MinorImageVersion;
		result->OptionalHeader32.MajorSubsystemVersion = opt32->MajorSubsystemVersion;
		result->OptionalHeader32.MinorSubsystemVersion = opt32->MinorSubsystemVersion;
		result->OptionalHeader32.Win32VersionValue = opt32->Win32VersionValue;
		result->OptionalHeader32.SizeOfImage = opt32->SizeOfImage;
		result->OptionalHeader32.SizeOfHeaders = opt32->SizeOfHeaders;
		result->OptionalHeader32.CheckSum = opt32->CheckSum;
		result->OptionalHeader32.Subsystem = opt32->Subsystem;
		result->OptionalHeader32.DllCharacteristics = opt32->DllCharacteristics;
		result->OptionalHeader32.SizeOfStackReserve = opt32->SizeOfStackReserve;
		result->OptionalHeader32.SizeOfStackCommit = opt32->SizeOfStackCommit;
		result->OptionalHeader32.SizeOfHeapReserve = opt32->SizeOfHeapReserve;
		result->OptionalHeader32.SizeOfHeapCommit = opt32->SizeOfHeapCommit;
		result->OptionalHeader32.LoaderFlags = opt32->LoaderFlags;
		result->OptionalHeader32.NumberOfRvaAndSizes = opt32->NumberOfRvaAndSizes;
		for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		{
			result->OptionalHeader32.DataDirectory[i].Size = opt32->DataDirectory[i].Size;
			result->OptionalHeader32.DataDirectory[i].VirtualAddress = opt32->DataDirectory[i].VirtualAddress;
		}
	}
	else if (pImgFileHdr->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		opt64 = (NTOptionalHeader64Ptr)&pImgFileHdr->OptionalHeader;
		result->OptionalHeader64.Magic = opt64->Magic;
		result->OptionalHeader64.MajorLinkerVersion = opt64->MajorLinkerVersion;
		result->OptionalHeader64.MinorLinkerVersion = opt64->MinorLinkerVersion;
		result->OptionalHeader64.SizeOfCode = opt64->SizeOfCode;
		result->OptionalHeader64.SizeOfInitializedData = opt64->SizeOfInitializedData;
		result->OptionalHeader64.SizeOfUninitializedData = opt64->SizeOfUninitializedData;
		result->OptionalHeader64.AddressOfEntryPoint = opt64->AddressOfEntryPoint;
		result->OptionalHeader64.BaseOfCode = opt64->BaseOfCode;
		result->OptionalHeader64.ImageBase = opt64->ImageBase;
		result->OptionalHeader64.SectionAlignment = opt64->SectionAlignment;
		result->OptionalHeader64.FileAlignment = opt64->FileAlignment;
		result->OptionalHeader64.MajorOperatingSystemVersion = opt64->MajorOperatingSystemVersion;
		result->OptionalHeader64.MinorOperatingSystemVersion = opt64->MinorOperatingSystemVersion;
		result->OptionalHeader64.MajorImageVersion = opt64->MajorImageVersion;
		result->OptionalHeader64.MinorImageVersion = opt64->MinorImageVersion;
		result->OptionalHeader64.MajorSubsystemVersion = opt64->MajorSubsystemVersion;
		result->OptionalHeader64.MinorSubsystemVersion = opt64->MinorSubsystemVersion;
		result->OptionalHeader64.Win32VersionValue = opt64->Win32VersionValue;
		result->OptionalHeader64.SizeOfImage = opt64->SizeOfImage;
		result->OptionalHeader64.SizeOfHeaders = opt64->SizeOfHeaders;
		result->OptionalHeader64.CheckSum = opt64->CheckSum;
		result->OptionalHeader64.Subsystem = opt64->Subsystem;
		result->OptionalHeader64.DllCharacteristics = opt64->DllCharacteristics;
		result->OptionalHeader64.SizeOfStackReserve = opt64->SizeOfStackReserve;
		result->OptionalHeader64.SizeOfStackCommit = opt64->SizeOfStackCommit;
		result->OptionalHeader64.SizeOfHeapReserve = opt64->SizeOfHeapReserve;
		result->OptionalHeader64.SizeOfHeapCommit = opt64->SizeOfHeapCommit;
		result->OptionalHeader64.LoaderFlags = opt64->LoaderFlags;
		result->OptionalHeader64.NumberOfRvaAndSizes = opt64->NumberOfRvaAndSizes;
		for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		{
			result->OptionalHeader64.DataDirectory[i].Size = opt64->DataDirectory[i].Size;
			result->OptionalHeader64.DataDirectory[i].VirtualAddress = opt64->DataDirectory[i].VirtualAddress;
		}
	}
	else
	{
		printf("Is NOT a PE32 nor PE32+ magic value! Got %x\n", pImgFileHdr->OptionalHeader.Magic);
		return;
	}
}

void loadPESections(EXEFilePtr result, char* buffer, NTHeaders32Ptr pImgFileHdr)
{
	NTSectionHeaderPtr section = PEEXE_FIRST_SECTION(NTSectionHeaderPtr,pImgFileHdr);
	for (int i = 1; i <= pImgFileHdr->FileHeader.NumberOfSections; i++)
	{
		OBJSectionPtr ptr = new OBJSection;
		for (int j = 0; j < IMAGE_SIZEOF_SHORT_NAME; j++) ptr->header.Name[j] = section->Name[j];
		ptr->header.Misc.PhysicalAddress = section->Misc.PhysicalAddress;
		ptr->header.Misc.VirtualSize = section->Misc.VirtualSize;
		ptr->header.VirtualAddress = section->VirtualAddress;
		ptr->header.SizeOfRawData = section->SizeOfRawData;
		ptr->header.PointerToRawData = section->PointerToRawData;
		ptr->header.PointerToRelocations = section->PointerToRelocations;
		ptr->header.PointerToLinenumbers = section->PointerToLinenumbers;
		ptr->header.NumberOfRelocations = section->NumberOfRelocations;
		ptr->header.NumberOfLinenumbers = section->NumberOfLinenumbers;
		ptr->header.Characteristics = section->Characteristics;
		char* sectionBuffer = MakePtr(char*, buffer, section->PointerToRawData);
		if (section->PointerToRawData > 0 && section->SizeOfRawData > 0)
		{
			ptr->sectionBuffer = new char[section->SizeOfRawData];
			for (int j = 0; j < section->SizeOfRawData; j++) ptr->sectionBuffer[j] = sectionBuffer[j];
		}
		else
		{
			ptr->sectionBuffer = nullptr;
		}
		result->sectionTable.push_back(ptr);
		section = MakePtr(NTSectionHeaderPtr, section, sizeof(IMAGE_SECTION_HEADER));
	}
}

PSTR GetSafeFileName(PSTR fn1, PSTR filename)
{
	PSTR cp = new char[1024];
	int i, c, len, max = 256;
	int got0 = 0;
	*cp = 0;
	len = 0;
	for (i = 0; i < max; i++) {
		c = fn1[i] & 0xff;
		if (c == 0) {
			got0 = 1;
			break;
		}
		else if ((c >= 0x20) && (c < 128)) {
			cp[len++] = c;
			cp[len] = 0;
		}
		else
			break;
	}
	if (got0 && len)
		return cp;
	*cp = 0;
	len = 0;
	for (i = 0; i < max; i++) {
		c = fn1[i] & 0xff;
		if (c == 0) {
			got0 = 1;
			break;
		}
		else if ((c >= 0x20) && (c < 128)) {
			cp[len++] = c;
			cp[len] = 0;
		}
		else
			break;
	}
	if (got0 && len)
		return cp;
	cp = (PSTR)"N/A";
	return cp;
}

void loadExportsDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader)
{
	DWORD exportRVAStart = GetImgDirEntryRVA(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);
	DWORD exportRVASize = GetImgDirEntrySize(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);
	NTSectionHeaderPtr headerExports = GetEnclosingSectionHeader(fileType, exportRVAStart, pNTHeader);
	if (headerExports != NULL)
	{
		DWORD delta = headerExports->VirtualAddress - headerExports->PointerToRawData;
		NTExportDirectoryPtr exportDir = MakePtr(NTExportDirectoryPtr, buffer, exportRVAStart - delta);
		DWORD Rva = exportDir->Name;
		PSTR fn1 = (PSTR)(buffer + (Rva - delta));
		result->exportDirectory = new Exports;
		result->exportDirectory->filename = (PSTR)(buffer + Rva);
		result->exportDirectory->filename = GetSafeFileName(fn1, result->exportDirectory->filename);
		result->exportDirectory->exports.Characteristics = exportDir->Characteristics;
		result->exportDirectory->exports.TimeDateStamp = exportDir->TimeDateStamp;
		result->exportDirectory->exports.MajorVersion = exportDir->MajorVersion;
		result->exportDirectory->exports.MinorVersion = exportDir->MinorVersion;
		result->exportDirectory->exports.Name = exportDir->Name;
		result->exportDirectory->exports.Base = exportDir->Base;
		result->exportDirectory->exports.NumberOfFunctions = exportDir->NumberOfFunctions;
		result->exportDirectory->exports.NumberOfNames = exportDir->NumberOfNames;
		result->exportDirectory->exports.AddressOfFunctions = exportDir->AddressOfFunctions;
		result->exportDirectory->exports.AddressOfNames = exportDir->AddressOfNames;
		result->exportDirectory->exports.AddressOfNameOrdinals = exportDir->AddressOfNameOrdinals;
		PDWORD functions = (PDWORD)(buffer + (exportDir->AddressOfFunctions - delta));
		PWORD ordinals = (PWORD)(buffer + (exportDir->AddressOfNameOrdinals - delta));
		PDWORD dwpname = (PDWORD)(buffer + (exportDir->AddressOfNames - delta));
		for (int i = 0; i < exportDir->NumberOfFunctions; i++)
		{
			DWORD entryPointRVA = functions[i];
			if (entryPointRVA == 0) continue;
			ExportsFunctionsPtr function = new ExportsFunctions;
			function->entryPoint = entryPointRVA;
			function->ordinal = i + exportDir->Base;
			for (int j = 0; j < exportDir->NumberOfNames; j++)
			{
				if (ordinals[j] == i) {
					INT off = dwpname[j];
					if (off > delta) {
						char* pn = buffer + (off - delta);
						function->filename = pn;
					}
				}
			}
			result->exportDirectory->functions.push_back(function);
		}
	}
}

void loadImportsDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader)
{
	DWORD importRVAStart = GetImgDirEntryRVA(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
	DWORD importRVASize = GetImgDirEntrySize(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
	NTSectionHeaderPtr headerImports = GetEnclosingSectionHeader(fileType, importRVAStart, pNTHeader);
	NTImportDesciptorPtr importDesc = (NTImportDesciptorPtr)GetPtrFromRVA(fileType, importRVAStart, pNTHeader, buffer);
	while (1)
	{
		if ((importDesc->TimeDateStamp == 0) && (importDesc->Name == 0))
			break;
		ImportsPtr ptr = new Imports;
		ptr->imports.Characteristics = importDesc->Characteristics;
		ptr->imports.TimeDateStamp = importDesc->TimeDateStamp;
		ptr->imports.ForwarderChain = importDesc->ForwarderChain;
		ptr->imports.Name = importDesc->Name;
		ptr->imports.FirstThunk = importDesc->FirstThunk;
		ptr->filename = (char*)GetPtrFromRVA(fileType, importDesc->Name, pNTHeader, buffer);
		DWORD dwthunk = importDesc->Characteristics;
		DWORD dwthunkIAT = importDesc->FirstThunk;
		if (fileType == PE64EXE)
		{
			NTThunkData64Ptr thunk = (NTThunkData64Ptr)GetPtrFromRVA(fileType, dwthunk, pNTHeader, buffer);
			NTThunkData64Ptr thunkIAT = (NTThunkData64Ptr)GetPtrFromRVA(fileType, dwthunkIAT, pNTHeader, buffer);
			while (1)
			{
				if (thunk->u1.AddressOfData == 0)
					break;
				DWORD dwOff = (DWORD)thunk->u1.AddressOfData;
				Thunk64Ptr tempThunk = new Thunk64;
				Thunk64Ptr tempIATThunk = new Thunk64;
				tempThunk->ordinalname = (NTImportByNamePtr)GetPtrFromRVA(fileType, dwOff, pNTHeader, buffer);
				tempThunk->thunk.u1.AddressOfData = thunk->u1.AddressOfData;
				tempIATThunk->thunk.u1.AddressOfData = thunkIAT->u1.AddressOfData;
				ptr->thunk64.push_back(tempThunk);
				ptr->thunkIAT64.push_back(tempIATThunk);
				thunk++;
				thunkIAT++;
			}
		}
		else
		{
			NTThunkData32Ptr thunk = (NTThunkData32Ptr)GetPtrFromRVA(fileType, dwthunk, pNTHeader, buffer);
			NTThunkData32Ptr thunkIAT = (NTThunkData32Ptr)GetPtrFromRVA(fileType, dwthunkIAT, pNTHeader, buffer);
			while (1)
			{
				if (thunk->u1.AddressOfData == 0) break;
				Thunk32Ptr tempThunk = new Thunk32;
				Thunk32Ptr tempIATThunk = new Thunk32;
				tempThunk->thunk.u1.AddressOfData = thunk->u1.AddressOfData;
				tempIATThunk->thunk.u1.AddressOfData = thunkIAT->u1.AddressOfData;
				ptr->thunk32.push_back(tempThunk);
				ptr->thunkIAT32.push_back(tempIATThunk);
				thunk++;
				thunkIAT++;
			}
		}
		result->importDirectory.push_back(ptr);
		importDesc++;
	}
}

void loadResourcesDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader)
{
	DWORD resourceRVA = GetImgDirEntryRVA(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_RESOURCE);
	DWORD resourceRVASize = GetImgDirEntrySize(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_RESOURCE);
	NTResourceDirectoryPtr resDir = (NTResourceDirectoryPtr)GetPtrFromRVA(fileType, resourceRVA, pNTHeader, buffer);
	if (!resDir) return;
	NTResourceDirectoryEntryPtr resDirEntry = (NTResourceDirectoryEntryPtr)(resDir + 1);
	WORD nNamed = resDir->NumberOfNamedEntries;
	WORD nIds = resDir->NumberOfIdEntries;
	result->resourcesDirectory.header.Characteristics = resDir->Characteristics;
	result->resourcesDirectory.header.TimeDateStamp = resDir->TimeDateStamp;
	result->resourcesDirectory.header.MajorVersion = resDir->MajorVersion;
	result->resourcesDirectory.header.MinorVersion = resDir->MinorVersion;
	result->resourcesDirectory.header.NumberOfNamedEntries = resDir->NumberOfNamedEntries;
	result->resourcesDirectory.header.NumberOfIdEntries = resDir->NumberOfIdEntries;
	for (WORD i = 0; i <= nNamed; i++, resDirEntry++)
	{
		ResourcesEntryPtr entry = new ResourcesEntry;
		if (resDirEntry->OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY) {
			entry->isDirectory++;
		}
		else if (resDirEntry->Name & IMAGE_RESOURCE_NAME_IS_STRING) {
			entry->isString++;
		}
		entry->entry.OffsetToData = resDirEntry->OffsetToData;
		entry->entry.Name = resDirEntry->Name;
		result->resourcesDirectory.entries.push_back(entry);
	}
}

void loadBaseRelocationsDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader)
{
	DWORD baseRelocRVA = GetImgDirEntryRVA(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	DWORD baseRelocRVASize = GetImgDirEntrySize(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	NTBaseRelocationPtr baseReloc = (NTBaseRelocationPtr)GetPtrFromRVA(fileType, baseRelocRVA, pNTHeader, buffer);
	while (baseReloc->SizeOfBlock != 0)
	{
		if (0 == baseReloc->VirtualAddress)
			break;
		if (baseReloc->SizeOfBlock < sizeof(*baseReloc))
			break;
		RelocsPtr relocs = new Relocs;
		relocs->baseReloc.VirtualAddress = baseReloc->VirtualAddress;
		relocs->baseReloc.SizeOfBlock = baseReloc->SizeOfBlock;
		int cEntries = (baseReloc->SizeOfBlock - sizeof(*baseReloc)) / sizeof(WORD);
		PWORD pEntry = MakePtr(PWORD, baseReloc, sizeof(*baseReloc));
		for (int i = 0; i < cEntries; i++)
		{
			RelocsEntryPtr relocsEntry = new RelocsEntry;
			relocsEntry->relocType = *pEntry;
			WORD relocType = (*pEntry & 0xF000) >> 12;
			relocsEntry->szRelocType = (relocType < RELOC_TYPE_COUNT) ? SzRelocTypes[relocType] : "unknown";
			relocs->entries.push_back(relocsEntry);
			if (IMAGE_REL_BASED_HIGHADJ == relocType)
			{
				pEntry++;
				cEntries--;
			}
			pEntry++;
		}
		result->baseRelocationsDirectory.push_back(relocs);
		baseReloc = MakePtr(NTBaseRelocationPtr, baseReloc, baseReloc->SizeOfBlock);
	}
}

void loadDebugDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader)
{
	DWORD debugRVA = GetImgDirEntryRVA(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG);
	DWORD debugRVASize = GetImgDirEntrySize(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG);
	if (debugRVA != 0 && debugRVASize != 0)
	{
		result->debugDirectory = new Debug;
		NTSectionHeaderPtr header = GetEnclosingSectionHeader(fileType, debugRVA, pNTHeader);
		NTDebugDirectoryPtr debugDir = MakePtr(NTDebugDirectoryPtr, buffer, header->PointerToRawData + (debugRVA - header->VirtualAddress));
		DWORD cDebugFormats = debugRVASize / sizeof(IMAGE_DEBUG_DIRECTORY);
		for (DWORD i = 0; i < cDebugFormats; i++)
		{
			DebugEntryPtr ptr = new DebugEntry;
			ptr->debugFormat = (debugDir->Type <= IMAGE_DEBUG_TYPE_EX_DLLCHARACTERISTICS) ? SzDebugFormats[debugDir->Type] : "???";
			ptr->entry.Characteristics = debugDir->Characteristics;
			ptr->entry.TimeDateStamp = debugDir->TimeDateStamp;
			ptr->entry.MajorVersion = debugDir->MajorVersion;
			ptr->entry.MinorVersion = debugDir->MinorVersion;
			ptr->entry.Type = debugDir->Type;
			ptr->entry.SizeOfData = debugDir->SizeOfData;
			ptr->entry.AddressOfRawData = debugDir->AddressOfRawData;
			ptr->entry.PointerToRawData = debugDir->PointerToRawData;
			result->debugDirectory->entries.push_back(ptr);
			debugDir++;
		}
	}
}

void loadLoadConfigDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader)
{
	DWORD configRVA = GetImgDirEntryRVA(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
	DWORD configRVASize = GetImgDirEntrySize(fileType, pNTHeader, IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG);
	NTLoadConfigDirectory32Ptr load32 = NULL;
	NTLoadConfigDirectory64Ptr load64 = NULL;
	if (fileType == PE64EXE)
	{
		load64 = (NTLoadConfigDirectory64Ptr)GetPtrFromRVA(fileType, configRVA, pNTHeader, buffer);
		if (load64)
			memcpy(&result->loadConfiguration64BitDirectory, load64, sizeof(NTLoadConfigDirectory64));
	}
	else
	{
		load32 = (NTLoadConfigDirectory32Ptr)GetPtrFromRVA(fileType, configRVA, pNTHeader, buffer);
		if (load32)
			memcpy(&result->loadConfiguration32BitDirectory, load32, sizeof(NTLoadConfigDirectory32));
	}
}

EXEFilePtr loadWin3264ExeFile(FileType fileType, char* buffer, LONGLONG fileSize)
{
	EXEFilePtr result = new EXEFile();
	result->fileType = fileType;
	DosHeaderPtr dosHeader = (DosHeaderPtr)buffer;
	NTHeaders32Ptr pNTHeader = MakePtr(NTHeaders32Ptr, dosHeader, dosHeader->e_lfanew);
	loadDOSStubEXE(result, dosHeader);
	loadPEHeaders(fileType, result, pNTHeader);
	loadPESections(result, buffer, pNTHeader);
	loadExportsDirectory(fileType, result, buffer, pNTHeader);
	loadImportsDirectory(fileType, result, buffer, pNTHeader);
	loadResourcesDirectory(fileType, result, buffer, pNTHeader);
	loadBaseRelocationsDirectory(fileType, result, buffer, pNTHeader);
	loadDebugDirectory(fileType, result, buffer, pNTHeader);
	loadLoadConfigDirectory(fileType, result, buffer, pNTHeader);
	return result;
}