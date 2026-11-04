#pragma once

#include <stdint.h>
#include "structs.h"

// linker specific OS/2 load file stuff.

typedef struct os2_seg_flags
{
    struct os2_seg_flags* next;
    uint16_t             flags;      // as above.
    char* name;
    segflag_type            type;    // true if flags for a class.
    uint16_t             specified;  // used for enforcing mutual exclusion
} os2_seg_flags;

// These used in the specified field.
#define SEG_IOPL_SPECD  0x800
#define SEG_READ_SPECD  0x400
#define SEG_RFLAG (SEG_READ_SPECD | SEG_READ_ONLY)

// These are segment flags used for the PE format and OS/2 v2.x These should
// hopefully not correspond to any existing OS/2 or 16-bit windows flag -
// if a new one appears, make sure these don't conflict!
// actually, SEG_NOPAGE conflicts with SEG_ITERATED, but since SEG_ITERATED
// isn't used, hopefully that should not matter.

#define SEG_NOPAGE              0x8
#define SEG_INVALID             0x8000
#define SEG_RESIDENT_MASK       0x0006
#define SEG_RESIDENT            0x0004
#define SEG_CONTIGUOUS          0x0006
#define SEG_RESIDENT_SHIFT      7

/* jwlink: page bits for MK_PE */
#define SEG_WRITABLE            0x10 /* is SEG_MOVABLE for segmented models */
#define SEG_EXECUTABLE          0x200 /* is SEG_CONFORMING for segmented models */

// NOTE: in these flags, CLEAN_MEMORY and INIT_THREAD_FLAG have the same value,
// since CLEAN_MEMORY is windows 16-bit only, and INIT_THREAD_FLAG is windows pe
// only.  Same goes for PROPOTIONAL_FONT and TERM_THREAD_FLAG.

#define SINGLE_AUTO_DATA    0x0001
#define MULTIPLE_AUTO_DATA  0x0002
#define INIT_INSTANCE_FLAG  0x0004
#define PHONEY_STACK_FLAG   0x0008
#define PM_COMPATIBLE       0x0010
#define PM_APPLICATION      0x0020
#define LONG_FILENAMES      0x0040
#define CLEAN_MEMORY        0x0080 /* MEMORY attribute of FORMAT WINDOWS */
#define INIT_THREAD_FLAG    0x0080 /* (undocumented) INITTHREAD attribute of FORMAT WIN PE */
#define PROPORTIONAL_FONT   0x0100 /* FONT attribute of FORMAT WINDOWS */
#define TERM_THREAD_FLAG    0x0100 /* (undocumented) TERMTHREAD attribute of FORMAT WIN PE */
#define PROTMODE_ONLY       0x0200 /* OPTION PROTMODE */
#define PHYS_DEVICE         0x0400
#define VIRT_DEVICE         0x0800
#define SHARABLE_DGROUP     0x1000
#define PM_NOT_COMPATIBLE   0x4000
#define TERM_INSTANCE_FLAG  0x8000

#define FLAT_GRANULARITY    (64UL * 1024)
#define FLAT_ROUND( x )     ROUND_UP( x, FLAT_GRANULARITY )

#include "exeos2.h"

/* in loadflat.c */

extern void             FiniOS2FlatLoadFile(void);
extern bool             FindOS2ExportSym(symbol*, dll_sym_info**);

/* in loados2.c */

extern unsigned long    ImportProcTable(unsigned long*);
extern unsigned long    ImportModTable(unsigned long*);
extern void             CheckExport(char*, uint16_t,
    int (*)(const char*, const char*));
extern unsigned long    ResNonResNameTable(bool);
extern void             ChkOS2Data(void);
extern void             ChkOS2Exports(void);
extern void             PhoneyStack(void);
extern void             FiniOS2LoadFile(void);
extern void             FreeImpNameTab(void);
extern uint32_t      GetStubSize(void);
extern uint32_t      Write_Stub_File(uint32_t);

