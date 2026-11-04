#pragma once

#include <stdint.h>

/* type of a [relative] virtual address */
typedef uint32_t     pe_va;
typedef unsigned long long pe_va64;

/* PE header table types */
enum
{
    PE_TBL_EXPORT,
    PE_TBL_IMPORT,
    PE_TBL_RESOURCE,
    PE_TBL_EXCEPTION,
    PE_TBL_SECURITY,
    PE_TBL_FIXUP,
    PE_TBL_DEBUG,
    PE_TBL_ARCHITECTURE,
    PE_TBL_GLOB_PTR,
    PE_TBL_THREAD,
    PE_TBL_LOAD_CONF,
    PE_TBL_BOUND_IMPORT,
    PE_TBL_IAT,
    PE_TBL_DELAY_IMP,
    PE_TBL_COMPLUS_RTHDR,
    PE_TBL_NUMBER = 16
};

#define OLD_PE_TBL_NUMBER 9

typedef struct
{
    pe_va           rva;
    uint32_t     size;
} pe_hdr_table_entry;

/* PE header structure */
typedef struct
{
    uint32_t         signature;
    uint16_t         cpu_type;
    uint16_t         num_objects;
    uint32_t         time_stamp;
    uint32_t         sym_table;
    uint32_t         num_syms;
    uint16_t         nt_hdr_size;    /* # of bytes after the flags field */
    uint16_t         flags;
    /* optional header */
    uint16_t         magic;          /* 0x10b=32-bit */
    uint8_t          lnk_major;
    uint8_t          lnk_minor;
    uint32_t         code_size;
    uint32_t         init_data_size;
    uint32_t         uninit_data_size;
    pe_va               entry_rva;
    uint32_t         code_base;
    uint32_t         data_base;
    uint32_t         image_base;
    uint32_t         object_align;
    uint32_t         file_align;
    uint16_t         os_major;
    uint16_t         os_minor;
    uint16_t         user_major;
    uint16_t         user_minor;
    uint16_t         subsys_major;
    uint16_t         subsys_minor;
    uint32_t         rsvd1;
    uint32_t         image_size;
    uint32_t         header_size;  //size of dos hdr, nt hdr, obj table & pad
    uint32_t         file_checksum;
    uint16_t         subsystem;
    uint16_t         dll_flags;
    uint32_t         stack_reserve_size;
    uint32_t         stack_commit_size;
    uint32_t         heap_reserve_size;
    uint32_t         heap_commit_size;
    uint32_t         tls_idx_addr;
    uint32_t         num_tables;
    pe_hdr_table_entry  table[PE_TBL_NUMBER];
} pe_header;

/* PE64 header structure */
typedef struct
{
    uint32_t         signature;
    uint16_t         cpu_type;
    uint16_t         num_objects;
    uint32_t         time_stamp;
    uint32_t         sym_table;
    uint32_t         num_syms;
    uint16_t         nt_hdr_size;    /* # of bytes after the flags field */
    uint16_t         flags;
    /* optional header */
    uint16_t         magic;          /* 0x20b=64-bit */
    uint8_t          lnk_major;
    uint8_t          lnk_minor;
    uint32_t         code_size;
    uint32_t         init_data_size;
    uint32_t         uninit_data_size;
    uint32_t         entry_rva;
    uint32_t         code_base;
    pe_va64             image_base;
    uint32_t         object_align;
    uint32_t         file_align;
    uint16_t         os_major;
    uint16_t         os_minor;
    uint16_t         user_major;
    uint16_t         user_minor;
    uint16_t         subsys_major;
    uint16_t         subsys_minor;
    uint32_t         rsvd1;
    uint32_t         image_size;
    uint32_t         header_size;  //size of dos hdr, nt hdr, obj table & pad
    uint32_t         file_checksum;
    uint16_t         subsystem;
    uint16_t         dll_flags;
    unsigned long long  stack_reserve_size;
    unsigned long long  stack_commit_size;
    unsigned long long  heap_reserve_size;
    unsigned long long  heap_commit_size;
    uint32_t         tls_idx_addr;
    uint32_t         num_tables;
    pe_hdr_table_entry  table[PE_TBL_NUMBER];
} pe_header64;

#define OLD_PE_TBL_SIZE (sizeof(pe_header) - 2 * (16 - 9) * sizeof(pe_va))

#define PE_SIGNATURE 0x4550
#define PL_SIGNATURE 0x4c50
#define PX_SIGNATURE 0x5850

/* CPU type field values */
enum
{
    PE_CPU_UNKNOWN = 0,
    PE_CPU_386 = 0x014c,
    PE_CPU_I860 = 0x014d,
    PE_CPU_MIPS_R3000 = 0x0162,
    PE_CPU_MIPS_R4000 = 0x0166,
    PE_CPU_ALPHA = 0x184,
    PE_CPU_POWERPC = 0x1F0,
    PE_CPU_AMD64 = 0x8664,   /* JWlink */
};

/* FLAG field bit values */
enum
{
    PE_FLG_PROGRAM = 0x0000,
    PE_FLG_RELOCS_STRIPPED = 0x0001,
    PE_FLG_IS_EXECUTABLE = 0x0002,
    PE_FLG_LINNUM_STRIPPED = 0x0004,
    PE_FLG_LOCALS_STRIPPED = 0x0008,
    PE_FLG_MINIMAL_OBJ = 0x0010,
    PE_FLG_UPDATE_OBJ = 0x0020,
    PE_FLG_LARGEADDRESSAWARE = 0x0020,       /* jwlink */
    PE_FLG_16BIT_MACHINE = 0x0040,
    PE_FLG_REVERSE_BYTE_LO = 0x0080,       // bytes are reversed.
    PE_FLG_32BIT_MACHINE = 0x0100,
    PE_FLG_FIXED = 0x0200,
    PE_FLG_FILE_PATCH = 0x0400,
    PE_FLG_FILE_SYSTEM = 0x1000,
    PE_FLG_LIBRARY = 0x2000,
    PE_FLG_REVERSE_BYTE_HI = 0x8000
};


/* Linker major/minor version numbers */
#define PE_LNK_MAJOR    2
#define PE_LNK_MINOR    0x12

#define PE_IMAGE_BASE   (0x400000UL)    /* default image base */
#define PE_OBJECT_ALIGN (64UL*1024)     /* default object alignment */
#define PE_FILE_ALIGN   (0x200U)        /* default file alignment */

/* OS major/minor version numbers */
#define PE_OS_MAJOR     1
#define PE_OS_MINOR     0
/* Subsystem major/minor version numbers */
#define PE_SS_MAJOR     0
#define PE_SS_MINOR     0

/* SUBSYSTEM field values */
enum
{
    PE_SS_UNKNOWN = 0x0000,
    PE_SS_NATIVE = 0x0001,
    PE_SS_WINDOWS_GUI = 0x0002,
    PE_SS_WINDOWS_CHAR = 0x0003,
    PE_SS_OS2_CHAR = 0x0005,
    PE_SS_POSIX_CHAR = 0x0007,
    PE_SS_PL_DOSSTYLE = 0x0042,
};

/* DLL FLAGS field bit values */
enum
{
    PE_DLL_PERPROC_INIT = 0x0001,  /* obsolete */
    PE_DLL_PERPROC_TERM = 0x0002,  /* obsolete */
    PE_DLL_PERTHRD_INIT = 0x0004,  /* obsolete */
    PE_DLL_PERTHRD_TERM = 0x0008,  /* obsolete */
    PE_DLL_NXCOMPAT = 0x0100,  /* jwlink */
    PE_DLL_WDM_DRIVER = 0x2000   /* jwlink */
};

/* PE object table structure */
#define PE_OBJ_NAME_LEN 8
typedef struct
{
    char                name[PE_OBJ_NAME_LEN];
    uint32_t         virtual_size;
    pe_va               rva;
    uint32_t         physical_size;
    uint32_t         physical_offset;
    uint32_t         relocs_rva;
    uint32_t         linnum_rva;
    uint16_t         num_relocs;
    uint16_t         num_linnums;
    uint32_t         flags;
} pe_object;

/* object table flag field bit values */
#define PE_OBJ_DUMMY            0x00000001UL    // reserved
#define PE_OBJ_NOLOAD           0x00000002UL
#define PE_OBJ_GROUPED          0x00000004UL    // for 16-bit offset code
#define PE_OBJ_NOPAD            0x00000008UL    // don't pad sect to next bndry
#define PE_OBJ_TYPE_COPY        0x00000010UL    // reserved
#define PE_OBJ_CODE             0x00000020UL
#define PE_OBJ_INIT_DATA        0x00000040UL
#define PE_OBJ_UNINIT_DATA      0x00000080UL
#define PE_OBJ_OTHER            0x00000100UL    // reserved
#define PE_OBJ_LINK_INFO        0x00000200UL    // contains link information
#define PE_OBJ_OVERLAY          0x00000400UL    // contains an overlay
#define PE_OBJ_REMOVE           0x00000800UL
#define PE_OBJ_COMDAT           0x00001000UL    // comdat section
#define PE_OBJ_ALIGN_1          0x00100000UL
#define PE_OBJ_ALIGN_2          0x00200000UL
#define PE_OBJ_ALIGN_4          0x00300000UL
#define PE_OBJ_ALIGN_8          0x00400000UL
#define PE_OBJ_ALIGN_16         0x00500000UL
#define PE_OBJ_ALIGN_32         0x00600000UL
#define PE_OBJ_ALIGN_64         0x00700000UL
#define PE_OBJ_DISCARDABLE      0x02000000UL
#define PE_OBJ_NOT_CACHED       0x04000000UL
#define PE_OBJ_NOT_PAGABLE      0x08000000UL
#define PE_OBJ_SHARED           0x10000000UL
#define PE_OBJ_EXECUTABLE       0x20000000UL
#define PE_OBJ_READABLE         0x40000000UL
#define PE_OBJ_WRITABLE         0x80000000UL

#define PE_OBJ_ALIGN_MASK       0x00700000UL
#define PE_OBJ_ALIGN_SHIFT      20

/* PE export directory table structure */
typedef struct
{
    uint32_t         flags;
    uint32_t         time_stamp;
    uint16_t         major;
    uint16_t         minor;
    pe_va               name_rva;
    uint32_t         ordinal_base;
    uint32_t         num_eat_entries;
    uint32_t         num_name_ptrs;
    pe_va               address_table_rva;
    pe_va               name_ptr_table_rva;
    pe_va               ordinal_table_rva;
} pe_export_directory;


/* PE import directory table structure */
typedef struct
{
    pe_va               import_lookup_table_rva;        /* was flags */
    uint32_t         time_stamp;
    uint16_t         major;
    uint16_t         minor;
    pe_va               name_rva;
    pe_va               import_address_table_rva;
} pe_import_directory;

/* bit flags for the import address table */
#define PE_IMPORT_BY_NAME       0x00000000UL
#define PE_IMPORT_BY_ORDINAL    0x80000000UL

/* PE import hint-name table structure */
typedef struct
{
    uint16_t         hint;
    uint8_t          name[2]; /* variable size, padded to even boundry */
} pe_hint_name_entry;

#define PE_HINT_DEFAULT         0

/* PE fixup table structure */
typedef uint16_t     pe_fixup_entry;

typedef struct
{
    pe_va               page_rva;
    uint32_t         block_size;
    /*  pe_fixup_entry      fixups[] */     /* variable size */
} pe_fixup_header;

#define PEUP 12


typedef struct
{
    pe_va       virt_addr;
    uint32_t value;
    uint16_t type;
    uint16_t pad;
} old_pe_fixup_entry;

#define OLD_PEUP 0

/* PE fixup types (stashed in 4 high bits of a pe_fixup_entry) */
#define PE_FIX_ABS      (0x0<<PEUP)     /* absolute, skipped */
#define PE_FIX_HIGH     (0x1<<PEUP)     /* add high 16 of delta */
#define PE_FIX_LOW      (0x2<<PEUP)     /* add low 16 of delta */
#define PE_FIX_HIGHLOW  (0x3<<PEUP)     /* add all 32 bits of delta */
#define PE_FIX_HIGHADJ  (0x4<<PEUP)     /* see the doc */
#define PE_FIX_MIPSJMP  (0x5<<PEUP)     /* see the doc */

/* PE fixup types (stashed in 4 high bits of a pe_fixup_entry) */
#define OLD_PE_FIX_ABS          (0x0<<OLD_PEUP) /* absolute, skipped */
#define OLD_PE_FIX_HIGH         (0x1<<OLD_PEUP) /* add high 16 of delta */
#define OLD_PE_FIX_LOW          (0x2<<OLD_PEUP) /* add low 16 of delta */
#define OLD_PE_FIX_HIGHLOW      (0x3<<OLD_PEUP) /* add all 32 bits of delta */
#define OLD_PE_FIX_HIGHADJ      (0x4<<OLD_PEUP) /* see the doc */
#define OLD_PE_FIX_MIPSJMP      (0x5<<OLD_PEUP) /* see the doc */

/* PE debug directory structure */
#define DEBUG_TYPE_UNKNOWN    0
#define DEBUG_TYPE_COFF       1
#define DEBUG_TYPE_CODEVIEW   2
#define DEBUG_TYPE_MISC       4
typedef struct
{
    uint32_t         flags;
    uint32_t         time_stamp;
    uint16_t         major;
    uint16_t         minor;
    uint32_t         debug_type;
    uint32_t         debug_size;
    pe_va               data_rva;
    uint32_t         data_seek;
} debug_directory;

/* PE DEBUG_TYPE_MISC data */
typedef struct
{
    uint32_t data_type;          /* 1 == filename of debug info file */
    uint32_t length;             /* size of this data block */
    uint32_t unicode;            /* LSB is unicode flag, rest is reserved */
    char        data[256 - 16];       /* name + path of debug info file, null terminated */
    uint32_t special_purpose;    /* used to pass file offset to cvpack utility */
} debug_misc_dbgdata;

/* procedure descriptor format for alpha and powerpc */

typedef struct
{
    uint32_t beginaddress;
    uint32_t endaddress;
    uint32_t exceptionhandler;
    uint32_t handlerdata;
    uint32_t prologendaddress;
} procedure_descriptor;

/* PE resource directory structure */
typedef struct
{
    uint32_t         flags;
    uint32_t         time_stamp;
    uint16_t         major;
    uint16_t         minor;
    uint16_t         num_name_entries;
    uint16_t         num_id_entries;
} resource_dir_header;

/*
A resource directory is a resource_dir_header followed immediately by
(num_name_entries + num_id_entries) resource_dir_entry's. The entries are
ordered sorted name entries then sorted id entries.
*/

typedef struct
{
    uint32_t         id_name;        /* see below */
    pe_va               entry_rva;      /* see below */
} resource_dir_entry;
#define PE_RESOURCE_MASK    0x7fffffff
#define PE_RESOURCE_MASK_ON 0x80000000

/*
If id_name & PE_RESOURCE_MASK_ON then id_name & PE_RESOURCE_MASK is a rva
(relative to the start of the resource object) to the name string otherwise
it is an integer id. The name string is an uint16_t length followed by a
unicode string of that length.

If entry_rva & PE_RESOURCE_MASK_ON the entry_rva & PE_RESOURCE_MASK is a rva
(relative to the start of the resource object) to another resource directory
otherwise it is a rva to a resource_entry.
*/

typedef struct
{
    uint32_t     data_rva;       /* relative to Image Base */
    uint32_t     size;
    uint32_t     code_page;
    uint32_t     rsvd;           /* must be 0 */
} resource_entry;


