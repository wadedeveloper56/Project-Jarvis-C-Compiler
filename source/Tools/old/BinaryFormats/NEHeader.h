#pragma once

#define SEGFLAGS_TYPE_CODE  0
#define SEGFLAGS_TYPE_DATA  1
#define SEGFLAGS_TYPE_MASK  0x0007

#define SEGFLAGS_MOVABLE    0x0010
#define SEGFLAGS_PRELOAD    0x0040
#define SEGFLAGS_HAS_RELOCS 0x0100
#define SEGFLAGS_DISCARD    0xF000
typedef enum _FlagWord {
	NOAUTODATA = 0x0000,
	SINGLEDATA = 0x0001,        
	MULTIPLEDATA = 0x0002,         

	LINKERROR = 0x2000,      
	LIBMODULE = 0x8000,          
} FlagWord;

#define GLOBINIT 1<<2      
#define PMODEONLY 1<<3      
#define INSTRUC86 1<<4     
#define INSTRU286 1<<5     
#define INSTRU386 1<<6     
#define INSTRUx87 1<<7      

typedef enum _apptype {
	none,
	fullscreeen,         
	winpmcompat,       
	winpmuses         
}apptype;

typedef enum _targetos {
	unknown,     
	os2,               
	win,         
	dos4,          
	win386,            
	BOSS              
}targetos;

#define LFN 1                  
#define PMODE 1<<1       
#define PFONT 1<<2      
#define GANGL 1<<3     

#define RESOURCE_FLAGS_MOVEABLE 0x0010
#define RESOURCE_FLAGS_PURE     0x0020
#define RESOURCE_FLAGS_PRELOAD  0x0040

#define ENTRY_FLAGS_EXPORTED   0x01
#define ENTRY_FLAGS_GLOBALDATA 0x02

#define ENTRY_FLAGS_EXPORTED   0x01
#define ENTRY_FLAGS_GLOBALDATA 0x02

#define RELOC_SOURCE_LOW_BYTE 0x00
#define RELOC_SOURCE_SEGMENT 0x02
#define RELOC_SOURCE_FAR_ADDR 0x03 /* (32-bit pointer) */
#define RELOC_SOURCE_OFFSET 0x05 /* (16-bit offset) */

#define RELOC_TARGET_INTERNAL_REF 0x00
#define RELOC_TARGET_IMPORT_ORDINAL 0x01
#define RELOC_TARGET_IMPORT_NAME 0x02
#define RELOC_TARGET_OS_FIXUP 0x03
#define RELOC_TARGET_MASK 0x03

#define RELOC_FLAGS_ADDITIVE 0x04
#define RELOC_FLAGS_MASK 0xFC

#define OS_FIXUP_TYPE_FIARQQ_FJARQQ 0x0001
#define OS_FIXUP_TYPE_FISRQQ_FJSRQQ 0x0002
#define OS_FIXUP_TYPE_FICRQQ_FJCRQQ 0x0003
#define OS_FIXUP_TYPE_FIERQQ        0x0004
#define OS_FIXUP_TYPE_FIDRQQ        0x0005
#define OS_FIXUP_TYPE_FIWRQQ        0x0006

#include "pshpack1.h"
typedef struct _NEHeader BINARYFORMATS_API {
	uint16_t signature;            
	uint8_t MajLinkerVersion;       
	uint8_t MinLinkerVersion;             
	uint16_t EntryTableOffset;          
	uint16_t EntryTableLength;        
	uint32_t FileLoadCRC;              
	uint16_t FlagWord;               
	uint16_t AutoDataSegIndex;       
	uint16_t InitHeapSize;           
	uint16_t InitStackSize;         
	uint32_t EntryPoint;                 
	uint32_t InitStack;                   
	uint16_t SegCount;                
	uint16_t ModRefs;                
	uint16_t NoResNamesTabSiz;         
	uint16_t SegTableOffset;            
	uint16_t ResTableOffset;            
	uint16_t ResidNamTable;              
	uint16_t ModRefTable;                
	uint16_t ImportNameTable;            
	uint32_t OffStartNonResTab;           
	uint16_t MovEntryCount;              
	uint16_t FileAlnSzShftCnt;           
	uint16_t nResTabEntries;           
	uint8_t targOS;               

	uint8_t OS2EXEFlags;           
	uint16_t retThunkOffset;                 
	uint16_t segrefthunksoff;             
	uint16_t mincodeswap;            
	uint8_t expctwinver[2];          
}NEHeader, * NEHeaderPtr, ** NEHeaderPtrPtr;

typedef struct _NESegmentTableEntry BINARYFORMATS_API {
	uint16_t    SectorBase;               
	uint16_t    SegBytes;          
	uint16_t    SegFlags;     
	uint16_t    MinAlloc;         
} NESegmentTableEntry, * NESegmentTableEntryPtr, ** NESegmentTableEntryPtrPtr;

typedef struct _NEResourceTableHeader BINARYFORMATS_API {
	uint16_t    AlignmentShiftCount;
} NEResourceTableHeader, * NEResourceTableHeaderPtr, ** NEResourceTableHeaderPtrPtr;

typedef struct _NEResourceTypeHeader BINARYFORMATS_API {
	uint16_t    TypeID;
	uint16_t    ResourceCount;
	uint32_t    Reserved;
} NEResourceTypeHeader, * NEResourceTypeHeaderPtr, ** NEResourceTypeHeaderPtrPtr;

typedef struct _NEResource BINARYFORMATS_API {
	uint16_t    FileOffset;                  
	uint16_t    ResourceLength;          
	uint16_t    ResourceFlags;    
	uint16_t    ResourceID;
	uint32_t    Reserved;
} NEResource, * NEResourcePtr, ** NEResourcePtrPtr;

typedef struct _NEResidentNameEntry BINARYFORMATS_API {
	uint8_t  NameLength;
	char* Name;   
	uint16_t OrdinalNumber;
} NEResidentNameEntry, * NEResidentNameEntryPtr, ** NEResidentNameEntryPtrPtr;

typedef struct _NEImportedNameTableEntry BINARYFORMATS_API {
	uint8_t  NameLength;
	char* Name;   
} NEImportedNameTableEntry, * NEImportedNameTableEntryPtr, ** NEImportedNameTableEntryPtrPtr;

typedef struct _NEFixedSegmentEntry BINARYFORMATS_API {
	uint8_t  EntryFlags;         
	uint16_t EntryPointOffset;      
} NEFixedSegmentEntry, * NEFixedSegmentEntryPtr, ** NEFixedSegmentEntryPtrPtr;

typedef struct _NEMovableSegmentEntry BINARYFORMATS_API {
	uint8_t  EntryFlags;         
	uint16_t Int3fh;               
	uint8_t  SegmentNumber;         
	uint16_t EntryPointOffset;      
} NEMovableSegmentEntry, * NEMovableSegmentEntryPtr, ** NEMovableSegmentEntryPtrPtr;

typedef struct _NESegRelocationEntry BINARYFORMATS_API {
	uint8_t  Source;              
	uint8_t  FlagsAndTarget;             
	uint16_t SourceChainOffset;
	union {         
		struct {
			uint8_t  SegmentNumber;          
			uint8_t  Zero;            
			uint16_t SegmentIndex;        
		} InternalRef;
		struct {
			uint16_t ModRefTableIndex;      
			uint16_t ProcNameOffset;             
		} ImportName;
		struct {
			uint16_t ModRefTableIndex;      
			uint16_t OrdinalNumber;         
		} ImportOrdinal;
		struct {
			uint16_t OSFixupType;   
			uint16_t Zero;          
		} OSFixup;
	} Value;
} NESegRelocationEntry,* NESegRelocationEntryPtr,** NESegRelocationEntryPtrPtr;

typedef struct _NESegRelocationInfo BINARYFORMATS_API {
	uint16_t RelocEntryCount;
	NESegRelocationEntryPtr Entries; //[RelocEntryCount] in size
} NESegRelocationInfo,* NESegRelocationInfoPtr,** NESegRelocationInfoPtrPtr;
#include "poppack.h"

