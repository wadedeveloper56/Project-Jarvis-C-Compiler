#include "pch.h"
#include "ArgumentTable.h"
#include "BinaryFormat.h"
#include "MemoryMappedFile.h"

using namespace std;
using namespace BinaryFormat;


bool fShowRelocations = false;
bool fShowRawSectionData = false;
bool fShowSymbolTable = false;
bool fShowLineNumbers = false;
bool fShowIATentries = false;
bool fShowPDATA = false;
bool fShowResources = false;
bool fShowMachineType = false;
BYTE* fileBgn = 0;
BYTE* fileEnd = 0;
__int64 fileSize = 0;
int OutOfRange = 0;
char* filename = 0;

int ProcessCommandLine(int argc, char* argv[])
{
	ArgLitPtr help = argLit0("hH?", "help", "print this help and exit");
	ArgLitPtr version = argLit0("vV", "version", "print version information and exit");
	ArgLitPtr all = argLit0("aA", "ALL", "include everything in dump");
	ArgLitPtr base = argLit0("bB", "BASE", "show base relocations");
	ArgLitPtr hex = argLit0("eE", "HEX", "include hex dump of sections");
	ArgLitPtr import = argLit0("iI", "IMPORT", "include Import Address Table thunk addresses");
	ArgLitPtr line = argLit0("lL", "LINE", "include line number information");
	ArgLitPtr pdata = argLit0("pP", "PDATA", "include PDATA (runtime functions)");
	ArgLitPtr resources = argLit0("rR", "RESOURCES", "include detailed resources (stringtables and dialogs)");
	ArgLitPtr machine = argLit0("mM", "MACHINE", "Show ONLY machine type");
	ArgLitPtr symbol = argLit0("sS", "SYMBOL", "show symbol table");
	ArgFilePtr infile = argFile1(NULL, NULL, NULL, "input file");
	ArgEndPtr end = argEnd(20);
	const char* progname = "PEDump";
	int exitcode = 0, nerrors = 0;
	void* argtable[] = { all, base, hex, import, line,  pdata, resources, machine, symbol, help, version, infile, end };
	printf("PEDUMP - 16/32/64 bit  OMF/COFF EXE/OBJ/LIB file dumper v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
	printf("All Rights Reserved\n");

	if (argNullCheck(argtable) != 0)
	{
		printf("%s: insufficient memory\n", progname);
		exit(1);
	}
	nerrors = argParse(argc, argv, argtable);
	if (help->count > 0 || argc == 1)
	{
		printf("\nUsage: %s", progname);
		argPrintSyntax(stdout, argtable, "\n\n");
		argPrintGlossary(stdout, argtable, "  %-35s %s\n");
		exit(1);
	}
	if (version->count > 0)
	{
		printf("PEDUMP - Win32/Win64 COFF EXE/OBJ/LIB file dumper v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
		printf("All Rights Reserved\n");
		exit(1);
	}
	if (nerrors > 0)
	{
		argPrintErrors(stdout, end, progname);
		printf("Try '%s --help' for more information.\n", progname);
		exit(1);
	}
	fShowRawSectionData = hex->count > 0;
	fShowLineNumbers = line->count > 0;
	fShowPDATA = pdata->count > 0;
	fShowRelocations = base->count > 0;
	fShowSymbolTable = symbol->count > 0;
	fShowIATentries = import->count > 0;
	fShowResources = resources->count > 0;
	fShowMachineType = machine->count > 0;
	if (all->count > 0)
	{
		fShowRelocations = true;
		fShowRawSectionData = true;
		fShowSymbolTable = true;
		fShowLineNumbers = true;
		fShowIATentries = true;
		fShowPDATA = true;
		fShowResources = true;
		fShowMachineType = true;
	}
	if (infile->count > 0)
	{
		filename = (char*)infile->filename[0];
	}
	return 0;
}

int main(int argc, char* argv[])
{
	ProcessCommandLine(argc, argv);
	printf("Dump of file %s\n", filename);
	MemoryMappedFile* mmfile = new MemoryMappedFile((char*)filename);
	char* buffer = mmfile->getBuffer();
	LONGLONG fileSize = mmfile->getFileSize();
	printf("File Size %lld bytes\n", fileSize);
	FileType fileType = getFileType(buffer, fileSize);
	switch (fileType)
	{
		case PE16EXE:
		{
			printf("File Type: 16-bit windows executable\n\n");
			WIN16EXEFilePtr data = loadWin16ExeFile(fileType, buffer, fileSize);
			break;
		}
		case PE32EXE:
		{
			printf("File Type: 32-bit PE EXECUTABLE IMAGE\n");
			EXEFilePtr data = loadWin3264ExeFile(fileType, buffer, fileSize);
			DumpDOSHeader(&data->dosHeader);
			DumpFileHeader(&data->FileHeader);
			DumpOptionalHeader32(&data->OptionalHeader32);
			for (int i = 0; i < data->sectionTable.size(); i++)
			{
				OBJSectionPtr ptr = data->sectionTable[i];
				DumpSection(i, ptr);
			}
			if (data->exportDirectory != nullptr)
			{
				DumpExportDirectory(data->exportDirectory);
			}
			DumpImportDirectory(fileType, &data->importDirectory);
			DumpResourcesDirectory(&data->resourcesDirectory);
			DumpBaseRelocationsDirectory(&data->baseRelocationsDirectory);
			DumpDebugDirectory(data->debugDirectory);
			DumpLoadConfig32Directory(&data->loadConfiguration32BitDirectory);
			break;
		}
		case PE64EXE:
		{
			printf("File Type: 64-bit PE EXECUTABLE IMAGE\n");
			EXEFilePtr data = loadWin3264ExeFile(fileType, buffer, fileSize);
			DumpDOSHeader(&data->dosHeader);
			DumpFileHeader(&data->FileHeader);
			DumpOptionalHeader64(&data->OptionalHeader64);
			for (int i = 0; i < data->sectionTable.size(); i++)
			{
				OBJSectionPtr ptr = data->sectionTable[i];
				DumpSection(i, ptr);
			}
			if (data->exportDirectory != nullptr)
			{
				DumpExportDirectory(data->exportDirectory);
			}
			DumpImportDirectory(fileType, &data->importDirectory);
			DumpResourcesDirectory(&data->resourcesDirectory);
			DumpBaseRelocationsDirectory(&data->baseRelocationsDirectory);
			DumpDebugDirectory(data->debugDirectory);
			DumpLoadConfig64Directory(&data->loadConfiguration64BitDirectory);
			break;
		}
		case PE32OBJ:
		{
			printf("File Type: 32-bit COFF OBJECT\n\n");
			OBJFilePtr data = loadWin3264ObjFile(fileType, buffer, fileSize);
			DumpFileHeader(&data->header);
			printf("\n");
			for (int i = 0; i < data->sectionTable.size(); i++)
			{
				OBJSectionPtr ptr = data->sectionTable[i];
				DumpSection(i, ptr);
			}
			DumpSymbolTable(data->symbolTable);
			printf("\nString Table Size = 0x%0X (%ld) bytes %lld entries\n", data->stringTableSize, data->stringTableSize, (LONGLONG)data->stringTable.size());
			for (int i = 0; i < data->stringTable.size(); i++)
			{
				string s = data->stringTable[i];
				printf("stringtable[% 4d] = %s\n", i, s.c_str());
			}
			break;
		}
		case PE64OBJ:
		{
			printf("File Type: 64-bit COFF OBJECT\n\n");
			OBJFilePtr data = loadWin3264ObjFile(fileType, buffer, fileSize);
			DumpFileHeader(&data->header);
			printf("\n");
			for (int i = 0; i < data->sectionTable.size(); i++)
			{
				OBJSectionPtr ptr = data->sectionTable[i];
				DumpSection(i, ptr);
			}
			DumpSymbolTable(data->symbolTable);
			printf("\nString Table Size = 0x%0X (%ld) bytes %lld entries\n", data->stringTableSize, data->stringTableSize, (LONGLONG)data->stringTable.size());
			for (int i = 0; i < data->stringTable.size(); i++)
			{
				string s = data->stringTable[i];
				printf("stringtable[% 4d] = %s\n", i, s.c_str());
			}
			break;
		}
		case DOSWIN16OBJ:
		{
			printf("File Type: 16-bit DOS\\Windows OBJECT\n\n");
			DOSOBJFilePtr data = loadDOSWin16ObjFile(fileType, buffer, fileSize);
			DumpDOSObjFile(data);
			break;
		}
		case ANONYMOUS:
			printf("anonymous obj file\n");
			break;
		case LIB:
		{
			printf("lib file\n");
			LIBFilePtr data = loadLibFile(fileType, buffer, fileSize);
			DumpLibFile(data);
			break;
		}
		default:
			cout << "unknown file type" << endl;
			break;
	}
	delete mmfile;
	return 0;
}

