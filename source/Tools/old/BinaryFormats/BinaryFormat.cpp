#include "pch.h"
#include "framework.h"
#include "BinaryFormat.h"

using namespace std;

WORD getFileMagic(char* buffer)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	return dosHeader->e_magic;
}

const char* GetMachineTypeName(WORD wMachineType)
{
	switch (wMachineType)
	{
		case IMAGE_FILE_MACHINE_UNKNOWN:return "The content of this field is assumed to be applicable to any machine type"; break;
		case IMAGE_FILE_MACHINE_ALPHA:return "Alpha AXP, 32 - bit address space"; break;
		case IMAGE_FILE_MACHINE_ALPHA64: return "Alpha 64, 64 - bit address space"; break;
		case IMAGE_FILE_MACHINE_AM33:return "Matsushita AM33"; break;
		case IMAGE_FILE_MACHINE_AMD64:return "x64"; break;
		case IMAGE_FILE_MACHINE_ARM:return "ARM little endian"; break;
		case IMAGE_FILE_MACHINE_ARM64:return "ARM64 little endian"; break;
		case IMAGE_FILE_MACHINE_ARMNT:return "ARM Thumb - 2 little endian"; break;
		case IMAGE_FILE_MACHINE_EBC:return "EFI byte code"; break;
		case IMAGE_FILE_MACHINE_I386:return "Intel 386 or later processors and compatible processors"; break;
		case IMAGE_FILE_MACHINE_IA64:return "Intel Itanium processor family"; break;
		case IMAGE_FILE_MACHINE_M32R:return "Mitsubishi M32R little endian"; break;
		case IMAGE_FILE_MACHINE_MIPS16:return "MIPS16"; break;
		case IMAGE_FILE_MACHINE_MIPSFPU:return "MIPS with FPU"; break;
		case IMAGE_FILE_MACHINE_MIPSFPU16:return "MIPS16 with FPU"; break;
		case IMAGE_FILE_MACHINE_POWERPC:return "Power PC little endian"; break;
		case IMAGE_FILE_MACHINE_POWERPCFP:return "Power PC with floating point support"; break;
		case IMAGE_FILE_MACHINE_R3000:return "MIPS I compatible 32 - bit little endian"; break;
		case IMAGE_FILE_MACHINE_R4000:return "MIPS III compatible 64 - bit little endian"; break;
		case IMAGE_FILE_MACHINE_R10000:return "MIPS IV compatible 64 - bit little endian"; break;
		case IMAGE_FILE_MACHINE_SH3:return "Hitachi SH3"; break;
		case IMAGE_FILE_MACHINE_SH3DSP:return "Hitachi SH3 DSP"; break;
		case IMAGE_FILE_MACHINE_SH4:return "Hitachi SH4"; break;
		case IMAGE_FILE_MACHINE_SH5:return "Hitachi SH5"; break;
		case IMAGE_FILE_MACHINE_THUMB:return "Thumb"; break;
		case IMAGE_FILE_MACHINE_WCEMIPSV2:return "MIPS little - endian WCE v2"; break;
	}

	return (char*)"UNLISTED";
}

int islistedMachineType(WORD wMachineType)
{
	const char* ps = GetMachineTypeName(wMachineType);
	if (strcmp(ps, "UNLISTED") == 0)
	{
		return 0;
	}

	if (strcmp(ps, "Unknown") == 0)
	{
		return 0;
	}

	return 1;
}

FileType getFileType(char* buffer, LONGLONG fileSize)
{
	FileType result = UNKNOWN;
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)(buffer);
	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
		PIMAGE_NT_HEADERS32 pNTHeader = MakePtr(PIMAGE_NT_HEADERS32, dosHeader, dosHeader->e_lfanew);
		NEHeaderPtr neHeader = MakePtr(NEHeaderPtr, dosHeader, dosHeader->e_lfanew);
		if (dosHeader->e_lfanew < fileSize && pNTHeader->Signature == IMAGE_NT_SIGNATURE)
		{
			if (pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
			{
				result = PE32EXE;
			}
			else if (pNTHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
			{
				result = PE64EXE;
			}
		}
		else if (dosHeader->e_lfanew < fileSize && neHeader->signature == IMAGE_OS2_SIGNATURE)  //os/2??
		{
			result = PE16EXE;
		}
		else
		{
			result = DOSEXE;
		}
	}
	else if (!strncmp(buffer, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE))
	{
		return LIB;
	}
	else if (buffer[0] == (char)THEADR)
	{
		return DOSWIN16OBJ;
	}
	else
	{
		PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)buffer;
		if (pImgFileHdr->Machine == IMAGE_FILE_MACHINE_UNKNOWN)
		{
			ANON_OBJECT_HEADER* ao = (ANON_OBJECT_HEADER*)buffer;
			result = ANONYMOUS;
		}
		else if (pImgFileHdr->Machine == IMAGE_FILE_MACHINE_I386)
		{
			result = PE32OBJ;
		}
		else if (pImgFileHdr->Machine == IMAGE_FILE_MACHINE_AMD64)
		{
			result = PE64OBJ;
		}
	}
	return result;
}

void hexdump(const void* data, size_t size, size_t label) {
	const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
	const int bytes_per_row = 16;
	size_t offset = 0;

	while (offset < size) {
		cout << hex << setw(8) << setfill('0') << label << "  ";

		for (int i = 0; i < bytes_per_row; ++i) {
			if (i > 0 && i % 8 == 0) {
				cout << " ";
			}
			if (offset + i < size) {
				cout << hex << setw(2) << setfill('0') << static_cast<unsigned int>(p[offset + i]) << " ";
			}
			else {
				cout << "   ";
			}
		}

		cout << " ";
		for (int i = 0; i < bytes_per_row; ++i) {
			if (offset + i < size) {
				char ch = static_cast<char>(p[offset + i]);
				cout << (isprint(static_cast<unsigned char>(ch)) ? ch : '.');
			}
		}
		cout << endl;
		offset += bytes_per_row;
		label += bytes_per_row;
	}
}

char* get_ctime_stg(time_t* pt)
{
	char* pts = ctime(pt);
	return pts;
}

string getStringFromTable(const char* stringTable, size_t index)
{
	if (index < 4) {
		throw out_of_range("Invalid string table index.");
	}
	const char* str = stringTable + index;
	return string(str);
}

OBJFilePtr loadWin3264ObjFile(FileType fileType, char* buffer, LONGLONG fileSize)
{
	OBJFilePtr result = new OBJFile;
	PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)buffer;
	PIMAGE_SECTION_HEADER section = MakePtr(PIMAGE_SECTION_HEADER, (pImgFileHdr + 1), pImgFileHdr->SizeOfOptionalHeader);
	PIMAGE_SYMBOL symbolTableEntry = MakePtr(PIMAGE_SYMBOL, pImgFileHdr, pImgFileHdr->PointerToSymbolTable);
	DWORD* stringtable = MakePtr(DWORD*, symbolTableEntry, (sizeof(IMAGE_SYMBOL) * pImgFileHdr->NumberOfSymbols));
	result->stringTableSize = *stringtable;
	const char* pStringTable = reinterpret_cast<char*>(stringtable);
	for (size_t i = 4; i < result->stringTableSize; ++i) {
		if (pStringTable[i] != '\0') {
			result->stringTable.push_back(getStringFromTable(pStringTable, i));
			while (i < result->stringTableSize && pStringTable[i] != '\0') {
				i++;
			}
		}
	}
	result->header.Machine = pImgFileHdr->Machine;
	result->header.NumberOfSections = pImgFileHdr->NumberOfSections;
	result->header.TimeDateStamp = pImgFileHdr->TimeDateStamp;
	result->header.PointerToSymbolTable = pImgFileHdr->PointerToSymbolTable;
	result->header.NumberOfSymbols = pImgFileHdr->NumberOfSymbols;
	result->header.SizeOfOptionalHeader = pImgFileHdr->SizeOfOptionalHeader;
	result->header.Characteristics = pImgFileHdr->Characteristics;
	result->symbolTable = new COFFSymbolTable(symbolTableEntry, pImgFileHdr->NumberOfSymbols);
	for (int i = 1; i <= pImgFileHdr->NumberOfSections; i++)
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
		char* sectionBuffer = MakePtr(char*, pImgFileHdr, section->PointerToRawData);
		NTRelocationPtr pir = MakePtr(NTRelocationPtr, pImgFileHdr, section->PointerToRelocations);
		NTLineNumberPtr pln = MakePtr(NTLineNumberPtr, pImgFileHdr, section->PointerToLinenumbers);
		if (section->PointerToRawData > 0 && section->SizeOfRawData > 0)
		{
			ptr->sectionBuffer = new char[section->SizeOfRawData];
			for (int j = 0; j < section->SizeOfRawData; j++) ptr->sectionBuffer[j] = sectionBuffer[j];
		}
		else
		{
			ptr->sectionBuffer = nullptr;
		}
		if (section->PointerToRelocations > 0 && section->NumberOfRelocations > 0)
		{
			ptr->relocation = new NTRelocation[section->NumberOfRelocations];
			for (int k = 0; k < section->NumberOfRelocations; k++)
			{
				ptr->relocation[k].RelocCount = pir->RelocCount;
				ptr->relocation[k].SymbolTableIndex = pir->SymbolTableIndex;
				ptr->relocation[k].Type = pir->Type;
				ptr->relocation[k].VirtualAddress = pir->VirtualAddress;

				pir = MakePtr(NTRelocationPtr, pir, sizeof(NTRelocation));
			}
		}
		else
		{
			ptr->relocation = nullptr;
		}
		if (section->PointerToLinenumbers > 0 && section->NumberOfLinenumbers > 0)
		{
			ptr->lineNumbers = new NTLineNumber[section->NumberOfLinenumbers];
			for (int j = 0; j < section->NumberOfLinenumbers; j++)
			{
				ptr->lineNumbers[j].Linenumber = pln->Linenumber;
				ptr->lineNumbers[j].Type.SymbolTableIndex = pln->Type.SymbolTableIndex;
				ptr->lineNumbers[j].Type.VirtualAddress = pln->Type.VirtualAddress;
				if (pln->Linenumber == 0)
				{
				}
				else
				{
				}
				pln = MakePtr(NTLineNumberPtr, pln, sizeof(NTLineNumber));

			}
		}
		else
		{
			ptr->lineNumbers = nullptr;
		}
		result->sectionTable.push_back(ptr);
		section = MakePtr(PIMAGE_SECTION_HEADER, section, sizeof(IMAGE_SECTION_HEADER));
	}

	return result;
}

