#pragma once

#include <Windows.h>

#include "pshpack1.h"
typedef struct _DosHeader {             // DOS .EXE header
	WORD   e_magic;                     // Magic number
	WORD   e_cblp;                      // Bytes on last page of file
	WORD   e_cp;                        // Pages in file
	WORD   e_crlc;                      // Relocations
	WORD   e_cparhdr;                   // Size of header in paragraphs
	WORD   e_minalloc;                  // Minimum extra paragraphs needed
	WORD   e_maxalloc;                  // Maximum extra paragraphs needed
	WORD   e_ss;                        // Initial (relative) SS value
	WORD   e_sp;                        // Initial SP value
	WORD   e_csum;                      // Checksum
	WORD   e_ip;                        // Initial IP value
	WORD   e_cs;                        // Initial (relative) CS value
	WORD   e_lfarlc;                    // File address of relocation table
	WORD   e_ovno;                      // Overlay number
	WORD   e_res[4];                    // Reserved words
	WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
	WORD   e_oeminfo;                   // OEM information; e_oemid specific
	WORD   e_res2[10];                  // Reserved words
	LONG   e_lfanew;                    // File address of new exe header
} DosHeader, * DosHeaderPtr, ** DosHeaderPtrPtr;

typedef struct _NTFileHeader {
	WORD    Machine;
	WORD    NumberOfSections;
	DWORD   TimeDateStamp;
	DWORD   PointerToSymbolTable;
	DWORD   NumberOfSymbols;
	WORD    SizeOfOptionalHeader;
	WORD    Characteristics;
} NTFileHeader, * NTFileHeaderPtr, ** NTFileHeaderPtrPtr;

typedef struct _NTOptionalHeader32 {
	//
	// Standard fields.
	//

	WORD    Magic;
	BYTE    MajorLinkerVersion;
	BYTE    MinorLinkerVersion;
	DWORD   SizeOfCode;
	DWORD   SizeOfInitializedData;
	DWORD   SizeOfUninitializedData;
	DWORD   AddressOfEntryPoint;
	DWORD   BaseOfCode;
	DWORD   BaseOfData;

	//
	// NT additional fields.
	//

	DWORD   ImageBase;
	DWORD   SectionAlignment;
	DWORD   FileAlignment;
	WORD    MajorOperatingSystemVersion;
	WORD    MinorOperatingSystemVersion;
	WORD    MajorImageVersion;
	WORD    MinorImageVersion;
	WORD    MajorSubsystemVersion;
	WORD    MinorSubsystemVersion;
	DWORD   Win32VersionValue;
	DWORD   SizeOfImage;
	DWORD   SizeOfHeaders;
	DWORD   CheckSum;
	WORD    Subsystem;
	WORD    DllCharacteristics;
	DWORD   SizeOfStackReserve;
	DWORD   SizeOfStackCommit;
	DWORD   SizeOfHeapReserve;
	DWORD   SizeOfHeapCommit;
	DWORD   LoaderFlags;
	DWORD   NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} NTOptionalHeader32, * NTOptionalHeader32Ptr, ** NTOptionalHeader32PtrPtr;

typedef struct _NTOptionalHeader64 {
	WORD        Magic;
	BYTE        MajorLinkerVersion;
	BYTE        MinorLinkerVersion;
	DWORD       SizeOfCode;
	DWORD       SizeOfInitializedData;
	DWORD       SizeOfUninitializedData;
	DWORD       AddressOfEntryPoint;
	DWORD       BaseOfCode;
	ULONGLONG   ImageBase;
	DWORD       SectionAlignment;
	DWORD       FileAlignment;
	WORD        MajorOperatingSystemVersion;
	WORD        MinorOperatingSystemVersion;
	WORD        MajorImageVersion;
	WORD        MinorImageVersion;
	WORD        MajorSubsystemVersion;
	WORD        MinorSubsystemVersion;
	DWORD       Win32VersionValue;
	DWORD       SizeOfImage;
	DWORD       SizeOfHeaders;
	DWORD       CheckSum;
	WORD        Subsystem;
	WORD        DllCharacteristics;
	ULONGLONG   SizeOfStackReserve;
	ULONGLONG   SizeOfStackCommit;
	ULONGLONG   SizeOfHeapReserve;
	ULONGLONG   SizeOfHeapCommit;
	DWORD       LoaderFlags;
	DWORD       NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
}  NTOptionalHeader64, * NTOptionalHeader64Ptr, ** NTOptionalHeader64PtrPtr;

typedef struct _NTHeaders64 {
	DWORD Signature;
	NTFileHeader FileHeader;
	NTOptionalHeader64 OptionalHeader;
} NTHeaders64, * NTHeaders64Ptr, ** NTHeaders64PtrPtr;

typedef struct _NTHeaders32 {
	DWORD Signature;
	NTFileHeader FileHeader;
	NTOptionalHeader32 OptionalHeader;
} NTHeaders32, * NTHeaders32Ptr, ** NTHeaders32PtrPtr;

typedef struct _NTSectionHeader {
	BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		DWORD   PhysicalAddress;
		DWORD   VirtualSize;
	} Misc;
	DWORD   VirtualAddress;
	DWORD   SizeOfRawData;
	DWORD   PointerToRawData;
	DWORD   PointerToRelocations;
	DWORD   PointerToLinenumbers;
	WORD    NumberOfRelocations;
	WORD    NumberOfLinenumbers;
	DWORD   Characteristics;
} NTSectionHeader, * NTSectionHeaderPtr, ** NTSectionHeaderPtrPtr;

typedef struct _NTExportDirectory {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Name;
	DWORD   Base;
	DWORD   NumberOfFunctions;
	DWORD   NumberOfNames;
	DWORD   AddressOfFunctions;     // RVA from base of image
	DWORD   AddressOfNames;         // RVA from base of image
	DWORD   AddressOfNameOrdinals;  // RVA from base of image
} NTExportDirectory, * NTExportDirectoryPtr, ** NTExportDirectoryPtrPtr;

typedef struct _NTImportDesciptor {
	union {
		DWORD   Characteristics;            // 0 for terminating null import descriptor
		DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
	} DUMMYUNIONNAME;
	DWORD   TimeDateStamp;                  // 0 if not bound,
	// -1 if bound, and real date\time stamp
	//     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
	// O.W. date/time stamp of DLL bound to (Old BIND)

	DWORD   ForwarderChain;                 // -1 if no forwarders
	DWORD   Name;
	DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} NTImportDesciptor, * NTImportDesciptorPtr, ** NTImportDesciptorPtrPtr;

typedef struct _NTThunkData64 {
	union {
		ULONGLONG ForwarderString;  // PBYTE 
		ULONGLONG Function;         // PDWORD
		ULONGLONG Ordinal;
		ULONGLONG AddressOfData;    // PIMAGE_IMPORT_BY_NAME
	} u1;
} NTThunkData64, * NTThunkData64Ptr, ** NTThunkData64PtrPtr;

typedef struct _NTThunkData32 {
	union {
		DWORD ForwarderString;      // PBYTE 
		DWORD Function;             // PDWORD
		DWORD Ordinal;
		DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
	} u1;
} NTThunkData32, * NTThunkData32Ptr, ** NTThunkData32PtrPtr;

typedef struct _NTResourceDirectory {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	WORD    NumberOfNamedEntries;
	WORD    NumberOfIdEntries;
	//  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[];
} NTResourceDirectory, * NTResourceDirectoryPtr, ** NTResourceDirectoryPtrPtr;

typedef struct _NTResourceDirectoryEntry {
	union {
		struct {
			DWORD NameOffset : 31;
			DWORD NameIsString : 1;
		} DUMMYSTRUCTNAME;
		DWORD   Name;
		WORD    Id;
	} DUMMYUNIONNAME;
	union {
		DWORD   OffsetToData;
		struct {
			DWORD   OffsetToDirectory : 31;
			DWORD   DataIsDirectory : 1;
		} DUMMYSTRUCTNAME2;
	} DUMMYUNIONNAME2;
}  NTResourceDirectoryEntry, * NTResourceDirectoryEntryPtr, ** NTResourceDirectoryEntryPtrPtr;

typedef struct _NTBaseRelocation {
	DWORD   VirtualAddress;
	DWORD   SizeOfBlock;
	//  WORD    TypeOffset[1];
} NTBaseRelocation, * NTBaseRelocationPtr, ** NTBaseRelocationPtrPtr;

typedef struct _NTDebugDirectory {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Type;
	DWORD   SizeOfData;
	DWORD   AddressOfRawData;
	DWORD   PointerToRawData;
} NTDebugDirectory, * NTDebugDirectoryPtr, ** NTDebugDirectoryPtrPtr;

typedef struct _NTLoadConfigCodeIntegrity {
	WORD    Flags;          // Flags to indicate if CI information is available, etc.
	WORD    Catalog;        // 0xFFFF means not available
	DWORD   CatalogOffset;
	DWORD   Reserved;       // Additional bitmask to be defined later
} NTLoadConfigCodeIntegrity, * NTLoadConfigCodeIntegrityPtr, ** NTLoadConfigCodeIntegrityPtrPtr;

typedef struct _NTLoadConfigDirectory32 {
	DWORD   Size;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   GlobalFlagsClear;
	DWORD   GlobalFlagsSet;
	DWORD   CriticalSectionDefaultTimeout;
	DWORD   DeCommitFreeBlockThreshold;
	DWORD   DeCommitTotalFreeThreshold;
	DWORD   LockPrefixTable;                // VA
	DWORD   MaximumAllocationSize;
	DWORD   VirtualMemoryThreshold;
	DWORD   ProcessHeapFlags;
	DWORD   ProcessAffinityMask;
	WORD    CSDVersion;
	WORD    DependentLoadFlags;
	DWORD   EditList;                       // VA
	DWORD   SecurityCookie;                 // VA
	DWORD   SEHandlerTable;                 // VA
	DWORD   SEHandlerCount;
	DWORD   GuardCFCheckFunctionPointer;    // VA
	DWORD   GuardCFDispatchFunctionPointer; // VA
	DWORD   GuardCFFunctionTable;           // VA
	DWORD   GuardCFFunctionCount;
	DWORD   GuardFlags;
	NTLoadConfigCodeIntegrity CodeIntegrity;
	DWORD   GuardAddressTakenIatEntryTable; // VA
	DWORD   GuardAddressTakenIatEntryCount;
	DWORD   GuardLongJumpTargetTable;       // VA
	DWORD   GuardLongJumpTargetCount;
	DWORD   DynamicValueRelocTable;         // VA
	DWORD   CHPEMetadataPointer;
	DWORD   GuardRFFailureRoutine;          // VA
	DWORD   GuardRFFailureRoutineFunctionPointer; // VA
	DWORD   DynamicValueRelocTableOffset;
	WORD    DynamicValueRelocTableSection;
	WORD    Reserved2;
	DWORD   GuardRFVerifyStackPointerFunctionPointer; // VA
	DWORD   HotPatchTableOffset;
	DWORD   Reserved3;
	DWORD   EnclaveConfigurationPointer;    // VA
	DWORD   VolatileMetadataPointer;        // VA
	DWORD   GuardEHContinuationTable;       // VA
	DWORD   GuardEHContinuationCount;
	DWORD   GuardXFGCheckFunctionPointer;   // VA
	DWORD   GuardXFGDispatchFunctionPointer; // VA
	DWORD   GuardXFGTableDispatchFunctionPointer; // VA
	DWORD   CastGuardOsDeterminedFailureMode; // VA
	DWORD   GuardMemcpyFunctionPointer;     // VA
	DWORD   UmaFunctionPointers;            // VA
} NTLoadConfigDirectory32, * NTLoadConfigDirectory32Ptr, ** NTLoadConfigDirectory32PtrPtr;

typedef struct _NTLoadConfigDirectory64 {
	DWORD      Size;
	DWORD      TimeDateStamp;
	WORD       MajorVersion;
	WORD       MinorVersion;
	DWORD      GlobalFlagsClear;
	DWORD      GlobalFlagsSet;
	DWORD      CriticalSectionDefaultTimeout;
	ULONGLONG  DeCommitFreeBlockThreshold;
	ULONGLONG  DeCommitTotalFreeThreshold;
	ULONGLONG  LockPrefixTable;                // VA
	ULONGLONG  MaximumAllocationSize;
	ULONGLONG  VirtualMemoryThreshold;
	ULONGLONG  ProcessAffinityMask;
	DWORD      ProcessHeapFlags;
	WORD       CSDVersion;
	WORD       DependentLoadFlags;
	ULONGLONG  EditList;                       // VA
	ULONGLONG  SecurityCookie;                 // VA
	ULONGLONG  SEHandlerTable;                 // VA
	ULONGLONG  SEHandlerCount;
	ULONGLONG  GuardCFCheckFunctionPointer;    // VA
	ULONGLONG  GuardCFDispatchFunctionPointer; // VA
	ULONGLONG  GuardCFFunctionTable;           // VA
	ULONGLONG  GuardCFFunctionCount;
	DWORD      GuardFlags;
	NTLoadConfigCodeIntegrity CodeIntegrity;
	ULONGLONG  GuardAddressTakenIatEntryTable; // VA
	ULONGLONG  GuardAddressTakenIatEntryCount;
	ULONGLONG  GuardLongJumpTargetTable;       // VA
	ULONGLONG  GuardLongJumpTargetCount;
	ULONGLONG  DynamicValueRelocTable;         // VA
	ULONGLONG  CHPEMetadataPointer;            // VA
	ULONGLONG  GuardRFFailureRoutine;          // VA
	ULONGLONG  GuardRFFailureRoutineFunctionPointer; // VA
	DWORD      DynamicValueRelocTableOffset;
	WORD       DynamicValueRelocTableSection;
	WORD       Reserved2;
	ULONGLONG  GuardRFVerifyStackPointerFunctionPointer; // VA
	DWORD      HotPatchTableOffset;
	DWORD      Reserved3;
	ULONGLONG  EnclaveConfigurationPointer;    // VA
	ULONGLONG  VolatileMetadataPointer;        // VA
	ULONGLONG  GuardEHContinuationTable;       // VA
	ULONGLONG  GuardEHContinuationCount;
	ULONGLONG  GuardXFGCheckFunctionPointer;   // VA
	ULONGLONG  GuardXFGDispatchFunctionPointer; // VA
	ULONGLONG  GuardXFGTableDispatchFunctionPointer; // VA
	ULONGLONG  CastGuardOsDeterminedFailureMode; // VA
	ULONGLONG  GuardMemcpyFunctionPointer;     // VA
	ULONGLONG  UmaFunctionPointers;            // VA
} NTLoadConfigDirectory64, * NTLoadConfigDirectory64Ptr, ** NTLoadConfigDirectory64PtrPtr;

typedef struct _NTImportByName {
	WORD    Hint;
	CHAR   Name[1];
} NTImportByName, * NTImportByNamePtr, ** NTImportByNamePtrPtr;

typedef struct _NTSymbol {
	union {
		BYTE    ShortName[8];
		struct {
			DWORD   Short;     // if 0, use LongName
			DWORD   Long;      // offset into string table
		} Name;
		DWORD   LongName[2];    // PBYTE [2]
	} N;
	DWORD   Value;
	SHORT   SectionNumber;
	WORD    Type;
	BYTE    StorageClass;
	BYTE    NumberOfAuxSymbols;
} NTSymbol, * NTSymbolPtr, ** NTSymbolPtrPtr;

typedef struct _NTRelocation {
	union {
		DWORD   VirtualAddress;
		DWORD   RelocCount;             // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
	} DUMMYUNIONNAME;
	DWORD   SymbolTableIndex;
	WORD    Type;
} NTRelocation,* NTRelocationPtr, ** NTRelocationPtrPtr;

typedef struct _NTLineNumber {
	union {
		DWORD   SymbolTableIndex;               // Symbol table index of function name if Linenumber is 0.
		DWORD   VirtualAddress;                 // Virtual address of line number.
	} Type;
	WORD    Linenumber;                         // Line number.
} NTLineNumber,* NTLineNumberPtr, ** NTLineNumberPtrPtr;

#define PEEXE_FIRST_SECTION( cast, ntheader ) ((cast)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

#include "poppack.h"
