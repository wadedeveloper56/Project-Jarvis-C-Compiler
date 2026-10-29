#pragma once

#include "pshpack1.h"

/* OS/2 EXE file header and various tables */
/* ======================================= */

typedef struct os2_exe_header
{
    uint16_t         signature;      /* signature to mark valid EXE file */
    uint16_t         version;        /* version of linker                */
    uint16_t         entry_off;      /* offset from header of entry table*/
    uint16_t         entry_size;     /* size of entry table              */
    uint32_t         chk_sum;        /* checksum of entire file          */
    uint16_t         info;           /* various flags (see below)        */
    uint16_t         adsegnum;       /* automatic data segment number    */
    uint16_t         heap;           /* initial heap allocation - bytes  */
    uint16_t         stack;          /* initial stack allocation - bytes */
    uint16_t         IP;             /* value for IP                     */
    uint16_t         entrynum;       /* segment number of entry segment  */
    uint16_t         SP;             /* value for SP                     */
    uint16_t         stacknum;       /* segment number of stack segment  */
    uint16_t         segments;       /* total number of segments         */
    uint16_t         modrefs;        /* # entries in module ref table    */
    uint16_t         nonres_size;    /* size of nonresident name table   */
    uint16_t         segment_off;    /* header offset of   segment table */
    uint16_t         resource_off;   /* " " "             resource table */
    uint16_t         resident_off;   /* " " "        resident name table */
    uint16_t         module_off;     /* " " "     module reference table */
    uint16_t         import_off;     /* " " "       imported names table */
    uint32_t         nonres_off;     /* offset from start of file of
                                                    nonresident names table */
    uint16_t         movable;        /* number of movable entry points   */
    uint16_t         align;          /* segment alignment shift count    */
    uint16_t         resource;       /* # of entries in resource table   */
    uint8_t          target;         /* target operating system */
    uint8_t          otherflags;     /* more flags */
    uint16_t         gangstart;      /* these 4 added for windows */
    uint16_t         ganglength;
    uint16_t         swaparea;
    uint16_t         expver;
} os2_exe_header;

#define OS2_SIGNATURE_WORD      0x454e  // 'NE'
#define RAT_SIGNATURE_WORD      0x454c  // 'LE'
#define OS2_NE_OFFSET             0x3c
#define OS2_EXE_HEADER_FOLLOWS  0x0040  /* reloc table offset 0x40 */

/******************************************************************************
 *
 *        The info field is used as follows:
 *        (W - Windows only, O - OS2 only)
 *
 *  x x x x      x x x x      x x x x      x x x x
 *  | | |            | |      | | | |      | | | |
 *  | | |            | |      | | | |      | | | +->  single autodata segment
 *  | | |            | |      | | | |      | | +--->  multiple autodata segments
 *  | | |            | |      | | | |      | +----->  if DLL, per-process
 *  | | |            | |      | | | |      |             initialization
 *  | | |            | |      | | | |      +------->  protected mode only
 *  | | |            | |      | | | +-------------->W uses LIM EMS directly
 *  | | |            | |      | | +---------------->O needs 80286
 *  | | |            | |      | |                   W each instance has own
 *  | | |            | |      | |                        EMS bank
 *  | | |            | |      | +------------------>O needs 80386
 *  | | |            | |      |                     W if DLL, global memory
 *  | | |            | |      |                          above EMS line
 *  | | |            | |      +-------------------->  needs math co-processor
 *  | | |            +-+--------------------------->  PM compatibility flags
 *  | | +------------------------------------------>  errors during link
 *  | |                                                  (not executable)
 *  | +-------------------------------------------->W if DLL, private DLL: only
 *  |                                                    one program accesses
 *  +---------------------------------------------->  1=DLL, 0=program file
 *
 *****************************************************************************/

#define OS2_SINGLE_AUTO         1
#define OS2_MULT_AUTO           2
#define OS2_INIT_INSTANCE       0x0004
#define OS2_PROT_MODE_ONLY      0x0008
#define WIN_USES_EMS_DIRECT     0x0010
#define OS2_NEEDS_80286         0x0020
#define WIN_EMS_BANK_INSTANCE   0x0020
#define OS2_NEEDS_80386         0x0040
#define WIN_EMS_GLOBAL_MEM      0x0040
#define OS2_NEEDS_MATH_CO       0x0080
#define OS2_NOT_PM_COMPATIBLE   0x0100
#define OS2_PM_COMPATIBLE       0x0200
#define OS2_PM_APP              0x0300
#define OS2_LINK_ERROR          0x2000
#define OS2_ISCONFORMING        0x4000      /* this doesn't exist anymore.*/
#define WIN_PRIVATE_DLL         0x4000
#define OS2_IS_DLL              0x8000

#define OS2_COMPATIBILITY_MASK  0x0300

 /* these are used in the target field.*/

#define TARGET_OS2      1
#define TARGET_WINDOWS  2
#define TARGET_DOS4     3
#define TARGET_WIN386   4
#define TARGET_DPMI     5 /* jwlink */

/* these are the flags in the otherflags field */

#define OS2_LONG_FILE_NAMES     0x01
#define WIN_PROPORTIONAL_FONT   0x02    /* this can only be set when */
#define WIN_CLEAN_MEMORY        0x04    /* WIN_CLEAN_MEMORY is set. */
#define WIN_GANGLOAD_PRESENT    0x08

typedef struct segment_record
{
    uint16_t         address;        /* segment position within file           */
    uint16_t         size;           /* segment length in bytes (size in file) */
    uint16_t         info;           /* various flags (see below)              */
    uint16_t         min;            /* minimum allocation (size in mem)       */
} segment_record;

/******************************************************************************
 *
 *        The info field is used as follows:
 *
 *  x x x x      x x x x      x x x x      x x x x
 *    | | |      | | | |      | | | |      |     |
 *    | | |      +-| | |      | | | |      |     +-> data segment (not code)
 *    | | |        | | |      | | | |      +-------> iterated data
 *    | | |        | | |      | | | +--------------> movable segment
 *    | | |        | | |      | | +----------------> pure (sharable) segment
 *    | | |        | | |      | +------------------> preload (not loadoncall)
 *    | | |        | | |      +--------------------> read only (code or data)
 *    | | |        | | +---------------------------> seg has relocation info
 *    | | |        | +-----------------------------> seg has debugging info
 *    | | |        +-------------------------------> descriptor privilege level
 *    | | +----------------------------------------> discardable segment
 *    | +------------------------------------------> 32 bit segment
 *    +--------------------------------------------> part of huge segment
 *
 *****************************************************************************/

 /* NOTE: the linker uses some of the unused bits here. If these bits become
  * used, make sure the linker developer knows about it! */

#define SEG_DATA            1
#define SEG_FLAG_1       0x02
#define SEG_FLAG_2       0x04
#define SEG_ITERATED        8
#define SEG_MOVABLE      0x10       /* for PE, it is SEG_WRITABLE */
#define SEG_PURE         0x20       /* i.e. segment is sharable. */
#define SEG_PRELOAD      0x40
#define SEG_READ_ONLY    0x80
#define SEG_RELOC       0x100
#define SEG_CONFORMING  0x200       /* was SEG_DEBUG, is SEG_EXECUTABLE for PE */
#define SEG_LEVEL_1     0x400
#define SEG_LEVEL_2     0x800
#define SEG_LEVEL_3     0xC00
#define SEG_DISCARD    0x1000
#define SEG_32_BIT     0x2000
#define SEG_HUGE       0x4000
#define SEG_RESRC_HIGH 0x8000
#define SEG_SHIFT_PMODE_LVL     10
#define SEG_SHIFT_PRI_LVL       12

#define SEG_LEVEL_MASK  0x0C00

#define MOVABLE_ENTRY_PNT       0xff

typedef struct bundle_prefix
{
    uint8_t          number;         /* number of entries in bundle      */
    uint8_t          type;           /* bundle type                      */
    /* - 00H: null bundle (no records)  */
    /* - FFH: movable segment records   */
    /* - nnH: fixed segment records for */
    /*        segment nn                */
} bundle_prefix;

typedef struct movable_record
{
    uint8_t          info;           /* flags: 1 = exported entry        */
    /*    2 = uses shared data segment  */
    uint16_t         reserved;       /* reserved = 0x3FCD                */
    uint8_t          entrynum;       /* segment # containing entry point */
    uint16_t         entry;          /* offset of entry point            */
} movable_record;

#define ENTRY_EXPORTED  0x01
#define ENTRY_SHARED    0x02
#define IOPL_WORD_SHIFT 2

typedef struct fixed_record
{
    uint8_t          info;           /* flags: 1 = exported entry        */
    /*    2 = uses shared data segment  */
    uint16_t         entry;          /* offset of entry point            */
} fixed_record;

#define OS2_DEF_SEGMENT_SHIFT  9

/* The resource table is an uint16_t resource shift count followed by: */
/* repeated for each type */
typedef struct resource_type_record
{
    uint16_t         type;           /* see below */
    uint16_t         num_resources;  /* of this type */
    uint32_t         reserved;
} resource_type_record;

/* repeated for each instance of this type */
typedef struct resource_record
{
    uint16_t         offset;         /* after resource shift */
    uint16_t         length;         /* after resource shift */
    uint16_t         flags;          /* resource flags */
    uint16_t         name;
    uint32_t         reserved;
} resource_record;

/* resource flags are any combination of SEG_MOVABLE, SEG_PURE, SEG_PRELOAD, */
/* and SEG_DISCARD */

/* If  type  is 0 then this is the end of the table, otherwise the rest of */
/* the type record is present.  type  and  name  use the same format. If the */
/* high bit is set they represent an ordinal type or name, otherwise they */
/* are an offset (without any shift) from the start of the resource table */
/* into a block of strings after the table. Strings in the block are one */
/* byte length followed by that many characters and are not null terminated. */

        /* for relocation fields */
#define REL_OFFSET_ONLY         0x0001
#define REL_SEGMENT_ONLY        0x0002
#define REL_SEGMENT_OFFSET      0x0003
#define REL_INTERNAL_REFERENCE  0x0000
#define REL_IMPORTED_ORDINAL    0x0001
#define REL_IMPORTED_NAME       0x0002
#define REL_ADDITIVE            0x0004

#include "poppack.h"
