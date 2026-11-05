#pragma once

#include <stdint.h>
#include "structs.h"
#include "pshpack1.h"

// this is the portion of the nlm_header which is of a fixed size.
typedef struct fixed_header
{
    char            signature[sizeof(NLM_SIGNATURE) - 1];
    uint32_t     version;
    char            moduleName[14];     // 1st character is a length byte.
    uint32_t     codeImageOffset;
    uint32_t     codeImageSize;
    uint32_t     dataImageOffset;
    uint32_t     dataImageSize;
    uint32_t     uninitializedDataSize;
    uint32_t     customDataOffset;
    uint32_t     customDataSize;
    uint32_t     moduleDependencyOffset;      // temporarily stackSize for the C Lib */
    uint32_t     numberOfModuleDependencies;
    uint32_t     relocationFixupOffset;
    uint32_t     numberOfRelocationFixups;
    uint32_t     externalReferencesOffset;
    uint32_t     numberOfExternalReferences;
    uint32_t     publicsOffset;
    uint32_t     numberOfPublics;
    uint32_t     debugInfoOffset;
    uint32_t     numberOfDebugRecords;
    uint32_t     codeStartOffset;
    uint32_t     exitProcedureOffset;
    uint32_t     checkUnloadProcedureOffset;
    uint32_t     moduleType;
    uint32_t     flags;
    uint8_t      descriptionLength;
} fixed_header;

/* the second "fixed chunk" of the header */

typedef struct
{
    char            versionSignature[VERSION_SIGNATURE_LENGTH];
    uint32_t     majorVersion;
    uint32_t     minorVersion;
    uint32_t     revision;
    uint32_t     year;
    uint32_t     month;      /* starts at 1 */
    uint32_t     day;
} fixed_hdr_2;

typedef struct
{
    char            copyrightSignature[COPYRIGHT_SIGNATURE_LENGTH];
} fixed_hdr_3;


// NOTE: This assumes sizeof( virt_mem ) == sizeof( uint32_t ) !!!!!!!

#define MAX_IMP_INTERNAL  10  // maximum # of import relocs stored internally
                              // before allocating virtual memory.
#define MAX_IMP_VIRT 9       // maximum # of virt blocks before allocating more.
#define IMP_VIRT_ALLOC_SIZE 512     // size of virtual mem block to allocate
#define IMP_NUM_VIRT (IMP_VIRT_ALLOC_SIZE / sizeof( uint32_t ) )

typedef struct nov_import
{
    uint8_t        contents;       // # of relocs || # of virtmem blocks + 10
    uint32_t num_relocs;     // number of relocs allocated.
    virt_mem    addr[1];        // variable # of pointers virtual mem. hunks
} nov_import;

// values for upper bits of the relocation offsets.
#define NOV_IMP_NONRELATIVE  0x80000000
#define NOV_IMP_ISCODE       0x40000000

#define NOV_EXP_ISCODE      0x80000000

#define DO_NOV_EXPORTS          0x80
#define DO_WATCOM_EXPORTS       0x40
#define DO_NOV_REF_ONLY         0x20
#define NOV_EXE_FLAG_MASK       0x1F


extern void     NovDBIAddGlobal(void*);
extern void     NovDBIAddrStart(void);
extern void     NovDBIGenGlobal(symbol*);
extern void     FiniNovellLoadFile(void);
extern void     AddNovImpReloc(symbol*, uint32_t, bool, bool);
extern void     FindExportedSyms(void);
extern void     ResetLoadNov(void);

#include "poppack.h"
