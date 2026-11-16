#pragma once

#define BINARYFORMATS_API 
#define BINARYFORMATS_TEMPLATE 

#include "MemoryMappedFile.h"
#include "NEHeader.h"
#include "PEHeader.h"

using namespace std;

#define THEADR 0x80
#define LHEADR 0x82
#define COMENT 0x88
#define MODEND 0x8a
#define MODEND32 0x8b
#define EXTDEF 0x8c
#define TYPDEF 0x8e
#define PUBDEF 0x90
#define PUBDEF32 0x91
#define LINNUM 0x94
#define LINNUM32 0x95
#define LNAMES 0x96
#define SEGDEF 0x98
#define SEGDEF32 0x99
#define GRPDEF 0x9a
#define FIXUPP 0x9c
#define FIXUPP32 0x9d
#define LEDATA 0xa0
#define LEDATA32 0xa1
#define LIDATA 0xa2
#define LIDATA32 0xa3
#define COMDEF 0xb0
#define BAKPAT 0xb2
#define BAKPAT32 0xb3
#define LEXTDEF 0xb4
#define LEXTDEF32 0xb5
#define LPUBDEF 0xb6
#define LPUBDEF32 0xb7
#define LCOMDEF 0xb8
#define CEXTDEF 0xbc
#define COMDAT 0xc2
#define COMDAT32 0xc3
#define LINSYM 0xc4
#define LINSYM32 0xc5
#define ALIAS 0xc6
#define NBKPAT 0xc8
#define NBKPAT32 0xc9
#define LLNAMES 0xca
#define LIBHDR 0xf0
#define LIBEND 0xf1

#define COMENT_TRANSLATOR 0x00
#define COMENT_INTEL_COPYRIGHT 0x01
#define COMENT_LIB_SPEC 0x81
#define COMENT_MSDOS_VER 0x9c
#define COMENT_MEMMODEL 0x9d
#define COMENT_DOSSEG 0x9e
#define COMENT_DEFLIB 0x9f
#define COMENT_OMFEXT 0xa0
#define COMENT_NEWOMF 0xa1
#define COMENT_LINKPASS 0xa2
#define COMENT_LIBMOD 0xa3 
#define COMENT_EXESTR 0xa4
#define COMENT_INCERR 0xa6
#define COMENT_NOPAD 0xa7
#define COMENT_WKEXT 0xa8
#define COMENT_LZEXT 0xa9
#define COMENT_PHARLAP 0xaa
#define COMENT_IBM386 0xb0
#define COMENT_RECORDER 0xb1
#define COMENT_COMMENT 0xda
#define COMENT_COMPILER 0xdb
#define COMENT_DATE 0xdc
#define COMENT_TIME 0xdd
#define COMENT_USER 0xdf
#define COMENT_DEPFILE 0xe9
#define COMENT_COMMANDLINE 0xff
#define COMENT_PUBTYPE 0xe1
#define COMENT_COMPARAM 0xea
#define COMENT_TYPDEF 0xe3
#define COMENT_STRUCTMEM 0xe2
#define COMENT_OPENSCOPE 0xe5
#define COMENT_LOCAL 0xe6
#define COMENT_ENDSCOPE 0xe7
#define COMENT_SOURCEFILE 0xe8

#define HEX_DUMP_WIDTH 16

// Subsystem Values

#define IMAGE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem.
#define IMAGE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.
#define IMAGE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem.
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   // image runs in the Posix character subsystem.
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS       8   // image is a native Win9x driver.
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI       9   // Image runs in the Windows CE subsystem.
#define IMAGE_SUBSYSTEM_EFI_APPLICATION      10  //
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11   //
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER   12  //
#define IMAGE_SUBSYSTEM_EFI_ROM              13
#define IMAGE_SUBSYSTEM_XBOX                 14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION 16
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG    17

#define IMAGE_SCN_TYPE_DSECT 0x00000001  
#define IMAGE_SCN_TYPE_NOLOAD 0x00000002  
#define IMAGE_SCN_TYPE_GROUP 0x00000004  
#define IMAGE_SCN_TYPE_COPY 0x00000010  
#define IMAGE_SCN_TYPE_OVER 0x00000400  
#define IMAGE_SCN_MEM_PROTECTED 0x00004000
#define IMAGE_SCN_MEM_SYSHEAP 0x00010000

typedef char* CharPtr;
typedef char** CharPtrPtr;
typedef void* VoidPtr;
typedef void** VoidPtrPtr;
typedef unsigned char* UCharPtr;
typedef unsigned long UInt;

typedef enum _FileType { 
	UNKNOWN, 
	DOSEXE, 
	PE16EXE, PE32EXE, PE64EXE, 
	DOSWIN16OBJ, 
	PE32OBJ, PE64OBJ, 
	ANONYMOUS, 
	LIB 
}FileType;

typedef struct BINARYFORMATS_API
{
	WORD flag;
	const char* name;
} WORD_FLAG_DESCRIPTIONS;

typedef struct BINARYFORMATS_API
{
	DWORD flag;
	const char* name;
} DWORD_FLAG_DESCRIPTIONS;

typedef struct BINARYFORMATS_API _i386RelocTypes
{
	WORD type;
	const char* name;
} i386RelocTypes;

class COFFSymbolTable;

class BINARYFORMATS_API COFFSymbol
{
	PSTR 			m_pStringTable;
	NTSymbolPtr 	m_pSymbolData;
	DWORD			m_index;
	PSTR			m_pszShortString;
	char			m_szTypeName[16];
	void	CleanUp(void);
public:
	COFFSymbol(NTSymbolPtr pSymbolData, PSTR pStringTable, DWORD index);
	~COFFSymbol();
	NTSymbolPtr GetSymbol();
	DWORD GetIndex();
	PSTR  GetName();
	DWORD GetValue();
	SHORT GetSectionNumber();
	WORD  GetType();
	PSTR  GetTypeName();
	BYTE  GetStorageClass();
	PSTR  GetStorageClassName();
	BOOL  GetNumberOfAuxSymbols();
	BOOL  GetAuxSymbolAsString(PSTR pszBuffer, unsigned cbBuffer);
	friend class COFFSymbolTable;
};

typedef BINARYFORMATS_API COFFSymbol* PCOFFSymbol;
class BINARYFORMATS_API COFFSymbolTable
{
private:
	NTSymbolPtr	m_pSymbolBase;
	unsigned 		m_cSymbols;
	PSTR			m_pStringTable;
public:
	COFFSymbolTable(PVOID pSymbolBase, unsigned cSymbols);
	~COFFSymbolTable();
	unsigned GetNumberOfSymbols(void) { return m_cSymbols; }
	PCOFFSymbol GetNextSymbol(PCOFFSymbol);
	PCOFFSymbol GetNearestSymbolFromRVA(DWORD rva, BOOL fExact);
	PCOFFSymbol GetSymbolFromIndex(DWORD index);
};
typedef BINARYFORMATS_API COFFSymbolTable* PCOFFSymbolTable;

typedef struct BINARYFORMATS_API _OBJSection
{
	NTSectionHeader header;
	NTRelocationPtr relocation;
	NTLineNumberPtr lineNumbers;
	char* sectionBuffer;
}OBJSection, * OBJSectionPtr;

typedef struct BINARYFORMATS_API _OBJFile
{
	vector<OBJSectionPtr> sectionTable;
	vector<string> stringTable;
	NTFileHeader header;
	PCOFFSymbolTable symbolTable;
	DWORD stringTableSize;
	_OBJFile();
} OBJFile, * OBJFilePtr, ** OBJFilePtrPtr;

typedef struct BINARYFORMATS_API _ResourcesEntry
{
	NTResourceDirectoryEntry entry;
	DWORD isDirectory;
	DWORD isString;
	_ResourcesEntry();
} ResourcesEntry, * ResourcesEntryPtr, ** ResourcesEntryPtrPtr;

typedef struct BINARYFORMATS_API _Resources
{
	vector<ResourcesEntryPtr> entries;
	NTResourceDirectory header;
	_Resources();
}Resources, * ResourcesPtr, ** ResourcesPtrPtr;

typedef struct BINARYFORMATS_API _ExportsFunctions
{
	DWORD entryPoint;
	DWORD ordinal;
	char* filename;
	_ExportsFunctions();
} ExportsFunctions, * ExportsFunctionsPtr, ** ExportsFunctionsPtrPtr;

typedef struct BINARYFORMATS_API _Exports
{
	NTExportDirectory exports;
	char* filename;
	vector<ExportsFunctionsPtr> functions;
	_Exports();
} Exports, * ExportsPtr;

typedef struct BINARYFORMATS_API _Thunk64
{
	NTThunkData64 thunk;
	NTImportByNamePtr ordinalname;
	_Thunk64();
} Thunk64, * Thunk64Ptr;

typedef struct BINARYFORMATS_API _Thunk32
{
	NTThunkData32 thunk;
	NTImportByNamePtr ordinalname;
	_Thunk32();
} Thunk32, * Thunk32Ptr;

typedef struct BINARYFORMATS_API _Imports
{
	NTImportDesciptor imports;
	vector<Thunk64Ptr> thunk64;
	vector<Thunk64Ptr> thunkIAT64;
	vector<Thunk32Ptr> thunk32;
	vector<Thunk32Ptr> thunkIAT32;
	char* filename;
	_Imports();
} Imports, * ImportsPtr;

typedef struct BINARYFORMATS_API _RelocsEntry
{
	WORD relocType;
	const char* szRelocType;
	_RelocsEntry();
} RelocsEntry, * RelocsEntryPtr;

typedef struct BINARYFORMATS_API _Relocs
{
	NTBaseRelocation baseReloc;
	vector<RelocsEntryPtr> entries;
	_Relocs();
} Relocs, * RelocsPtr;

typedef struct BINARYFORMATS_API _DebugEntry
{
	NTDebugDirectory entry;
	const char* debugFormat;
	_DebugEntry();
} DebugEntry, * DebugEntryPtr;

typedef struct BINARYFORMATS_API _Debug
{
	vector<DebugEntryPtr> entries;
	_Debug();
} Debug, * DebugPtr;

typedef struct BINARYFORMATS_API _EXEFile
{
	FileType fileType;
	DosHeader dosHeader;
	DWORD Signature;
	NTFileHeader FileHeader;
	union {
		NTOptionalHeader32 OptionalHeader32;
		NTOptionalHeader64 OptionalHeader64;
	} DUMMYUNIONNAME;
	Resources resourcesDirectory;
	ExportsPtr exportDirectory;
	vector<ImportsPtr> importDirectory;
	vector<RelocsPtr> baseRelocationsDirectory;
	union {
		NTLoadConfigDirectory32 loadConfiguration32BitDirectory;
		NTLoadConfigDirectory64 loadConfiguration64BitDirectory;
	} DUMMYUNIONNAME2;
	DebugPtr debugDirectory;
	vector<OBJSectionPtr> sectionTable;
	_EXEFile();
} EXEFile, * EXEFilePtr, ** EXEFilePtrPtr;

typedef struct BINARYFORMATS_API _LIBFileLinkerMembers
{
	DWORD offset;
	PSTR pSymbolName;
	_LIBFileLinkerMembers();
} LIBFileLinkerMembers, * LIBFileLinkerMembersPtr;

typedef struct BINARYFORMATS_API _LIBFileEntry
{
	IMAGE_ARCHIVE_MEMBER_HEADER header;
	vector<LIBFileLinkerMembersPtr> first;
	vector<LIBFileLinkerMembersPtr> second;
	vector<LIBFileLinkerMembersPtr> longNames;
	OBJFilePtr objFile;
	_LIBFileEntry();
} LIBFileEntry, * LIBFileEntryPtr;

typedef struct BINARYFORMATS_API _LIBFile
{
	vector<LIBFileEntryPtr> entrys;
	_LIBFile();
} LIBFile, * LIBFilePtr, ** LIBFilePtrPtr;

typedef struct BINARYFORMATS_API _DOSOBJRecord
{
	unsigned char rectype;
	unsigned long reclength;
	unsigned char *buffer;
	_DOSOBJRecord();
} DOSOBJRecord, * DOSOBJRecordPtr, ** DOSOBJRecordPtrPtr;

typedef struct BINARYFORMATS_API _DOSOBJFile
{
	vector<DOSOBJRecordPtr> records;
	_DOSOBJFile();
} DOSOBJFile, * DOSOBJFilePtr, ** DOSOBJFilePtrPtr;

typedef struct BINARYFORMATS_API _NEResourceEntry
{
	NEResourcePtr resource;
	_NEResourceEntry();
} NEResourceEntry, * NEResourceEntryPtr, ** NEResourceEntryPtrPtr;

typedef struct BINARYFORMATS_API _NEResourceTypeEntry
{
	NEResourceTypeHeaderPtr header;
	vector<NEResourceEntryPtr> entries;
	_NEResourceTypeEntry();
} NEResourceTypeEntry, * NEResourceTypeEntryPtr, ** NEResourceTypeEntryPtrPtr;

typedef struct BINARYFORMATS_API _NEResourceTable
{
	NEResourceTableHeaderPtr header;
	vector<NEResourceTypeEntryPtr> entries;
	_NEResourceTable();
}NEResourceTable, * NEResourceTablePtr, ** NEResourceTablePtrPtr;

typedef struct BINARYFORMATS_API WIN16EXEFile
{
	vector<NESegmentTableEntryPtr> segmentTable;
	NEResourceTablePtr resourceTable;
	WIN16EXEFile();
} WIN16EXEFile, * WIN16EXEFilePtr, ** WIN16EXEFilePtrPtr;

#ifdef _WIN64
#define MakePtr( cast, ptr, addValue ) (cast)( (BYTE *)(ptr) + (ULONGLONG)(addValue))
#else
#define MakePtr( cast, ptr, addValue ) (cast)( (BYTE *)(ptr) + (DWORD)(addValue))
#endif

//Binaryformats.cpp functions
BINARYFORMATS_API WORD getFileMagic(char* buffer);
BINARYFORMATS_API FileType getFileType(char* buffer, LONGLONG fileSize);
BINARYFORMATS_API const char* GetMachineTypeName(WORD wMachineType);
BINARYFORMATS_API int islistedMachineType(WORD wMachineType);
BINARYFORMATS_API void hexdump(const void* data, size_t size, size_t label);
BINARYFORMATS_API char* get_ctime_stg(time_t* pt);
BINARYFORMATS_API OBJFilePtr loadWin3264ObjFile(FileType fileType, char* buffer, LONGLONG fileSize);
//EXEDump.cpp functions
BINARYFORMATS_API DWORD GetImgDirEntryRVA(FileType fileType, PVOID pNTHdr, DWORD IDE);
BINARYFORMATS_API NTSectionHeaderPtr GetSectionHeader(FileType fileType, PSTR name, PVOID pNTHeader);
BINARYFORMATS_API DWORD GetImgDirEntrySize(FileType fileType, PVOID pNTHdr, DWORD IDE);
BINARYFORMATS_API NTSectionHeaderPtr GetEnclosingSectionHeader(FileType fileType, DWORD rva, PVOID pNTHeader);
BINARYFORMATS_API LPVOID GetPtrFromRVA(FileType fileType, DWORD rva, NTHeaders32Ptr pNTHeader, char* imageBase);
BINARYFORMATS_API void loadDOSStubEXE(EXEFilePtr result, DosHeaderPtr dosHeader);
BINARYFORMATS_API void loadPEHeaders(FileType fileType, EXEFilePtr result, NTHeaders32Ptr pImgFileHdr);
BINARYFORMATS_API void loadPESections(EXEFilePtr result, char* buffer, NTHeaders32Ptr pImgFileHdr);
BINARYFORMATS_API PSTR GetSafeFileName(PSTR fn1, PSTR filename);
BINARYFORMATS_API void loadExportsDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader);
BINARYFORMATS_API void loadImportsDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader);
BINARYFORMATS_API void loadResourcesDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader);
BINARYFORMATS_API void loadBaseRelocationsDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader);
BINARYFORMATS_API void loadDebugDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader);
BINARYFORMATS_API void loadLoadConfigDirectory(FileType fileType, EXEFilePtr result, char* buffer, NTHeaders32Ptr pNTHeader);
BINARYFORMATS_API EXEFilePtr loadWin3264ExeFile(FileType fileType, char* buffer, LONGLONG fileSize);
//LIBDump.cpp functions
BINARYFORMATS_API LIBFilePtr loadLibFile(FileType fileType, char* buffer, LONGLONG fileSize);
//Output.cpp functions
BINARYFORMATS_API void GetObjRelocationName(WORD type, PSTR buffer, DWORD cBytes);
BINARYFORMATS_API void DumpSection(int i, OBJSectionPtr ptr, WORD machine);
BINARYFORMATS_API void GetSectionName(WORD section, PSTR buffer, unsigned cbBuffer);
BINARYFORMATS_API void DumpSymbolTable(COFFSymbolTable* pSymTab);
BINARYFORMATS_API void DumpDOSHeader(DosHeaderPtr dosHeader);
BINARYFORMATS_API void DumpFileHeader(NTFileHeaderPtr pImageFileHeader);
BINARYFORMATS_API void DumpOptionalHeader64(NTOptionalHeader64Ptr optionalHeader);
BINARYFORMATS_API void DumpOptionalHeader32(NTOptionalHeader32Ptr optionalHeader);
BINARYFORMATS_API void DumpExportDirectory(ExportsPtr exportDir);
BINARYFORMATS_API void DumpImportDirectory(bool is64, vector<ImportsPtr>* imports);
BINARYFORMATS_API void DumpResourcesDirectory(ResourcesPtr resources);
BINARYFORMATS_API void DumpBaseRelocationsDirectory(vector<RelocsPtr>* relocs);
BINARYFORMATS_API void DumpDebugDirectory(DebugPtr debug);
BINARYFORMATS_API void DumpLoadConfig32Directory(NTLoadConfigDirectory32Ptr load32);
BINARYFORMATS_API void DumpLoadConfig64Directory(NTLoadConfigDirectory64Ptr load64);
BINARYFORMATS_API void DumpLibFile(LIBFilePtr ptr);
BINARYFORMATS_API void DumpDOSObjFile(DOSOBJFilePtr ptr);
//DosObjDump.cpp
BINARYFORMATS_API DOSOBJFilePtr loadDOSWin16ObjFile(FileType fileType, char* buffer, LONGLONG fileSize);
BINARYFORMATS_API WIN16EXEFilePtr loadWin16ExeFile(FileType fileType, char* buffer, LONGLONG fileSize);