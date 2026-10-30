#pragma once
typedef struct gdt_info
{
    uint16_t     gdtlen;
    uint16_t     gdtaddr;
    uint8_t      gdtaddr_hi;
    uint8_t      gdtaccess;
    uint16_t     gdtreserved;
} gdt_info;

typedef struct dos16m_exe_header
{
    uint16_t signature;          /* BW signature to mark valid file  */
    uint16_t last_page_bytes;    /* length of image mod 512          */
    uint16_t pages_in_file;      /* number of 512 byte pages         */
    uint16_t reserved1;
    uint16_t reserved2;
    uint16_t min_alloc;          /* required memory, in KB           */
    uint16_t max_alloc;          /* max KB (private allocation)      */
    uint16_t stack_seg;          /* segment of stack                 */
    uint16_t stack_ptr;          /* initial SP value                 */
    uint16_t first_reloc_sel;    /* huge reloc list selector         */
    uint16_t init_ip;            /* initial IP value                 */
    uint16_t code_seg;           /* segment of code                  */
    uint16_t runtime_gdt_size;   /* runtime GDT size in bytes        */
    uint16_t MAKEPM_version;     /* ver * 100, GLU = (ver+10)*100    */
    /* end of DOS style EXE header */
    uint32_t next_header_pos;    /* file pos of next spliced .EXP    */
    uint32_t cv_info_offset;     /* offset to start of debug info    */
    uint16_t last_sel_used;      /* last selector value used         */
    uint16_t pmem_alloc;         /* private xm amount KB if nonzero  */
    uint16_t alloc_incr;         /* auto ExtReserve amount, in KB    */
    uint8_t  reserved4[6];
    /* the following used to be referenced as gdtimage[0..1] */
    uint16_t options;            /* runtime options                  */
    uint16_t trans_stack_sel;    /* sel of transparent stack         */
    uint16_t exp_flags;          /* see ef_ constants below          */
    uint16_t program_size;       /* size of program in paras         */
    uint16_t gdtimage_size;      /* size of gdt in file (bytes)      */
    uint16_t first_selector;     /* gdt[first_sel] = gdtimage[0], 0 => 0x80 */
    uint8_t  default_mem_strategy;
    uint8_t  reserved5;
    uint16_t transfer_buffer_size;   /* default in bytes, 0 => 8KB   */
    /* the following used to be referenced as gdtimage[2..15] */
    uint8_t  reserved6[48];
    char        EXP_path[64];       /* original .EXP file name  */
    /* gdtimage[16..gdtimage_size] follows immediately, then program image follows */
} dos16m_exe_header;

/* the values for the exp_flags field. */
enum
{
    ef_auto = 0x0001,   /* automatic selectors (=> relocatable) */
    ef_package = 0x0002,   /* module is a package                  */
    ef_nobigfoot = 0x0004,   /* don't place stack low DPMI memory    */
    ef_sharedata = 0x2000,   /* data is global, not per-instance     */
    ef_shareable = 0x4000,   /* component may be shared              */
    ef_dos4g = 0x8000    /* module is or requires DOS/4G         */
};

/* the values for the options field */

enum
{
    OPT_NOVCPI = 0x0002,   /* don't test for VCPI at startup   */
    OPT_KEYBOARD = 0x0004,   /* don't inhibit keyboard polling   */
    OPT_OVERLOAD = 0x0008,   /* allow overloading                */
    OPT_INT10 = 0x0010,
    OPT_INIT00 = 0x0020,   /* init memory allocs to 0x00       */
    OPT_INITFF = 0x0040,   /* init memory allocs to 0xFF       */
    OPT_ROTATE = 0x0080,   /* rotate selector assignment       */
    OPT_AUTO = 0x1000,   /* reloc table is present           */
};

#define TRANSPARENT 0x8000

#define NUM_RESERVED_SELS    16         // number of reserved selectors.

/* memory strategy constants */
#define MPreferExt      0       /* prefer, but not force, extended */
#define MPreferLow      1       /* prefer, but not force, conventional memory */
#define MForceExt       2       /* force extended */
#define MForceLow       3       /* force conventional */
#define MNoStrategy     0       /* no strategy specified */

#define D16M_ACC_CODE   0x9A    /* present, DPL 0, code, read, not accessed */
#define D16M_ACC_DATA   0x92    /* present, DPL 0, data, write, not accessed */

#define D16M_USER_SEL   0x80    /* first user selector (user_gdt << 3) */

#define DOS16M_SIGNATURE    0x5742      /* 'BW' */

#define _EXE16M_H

