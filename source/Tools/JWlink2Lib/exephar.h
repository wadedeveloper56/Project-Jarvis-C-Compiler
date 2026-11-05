#pragma once

#include "pshpack1.h"

typedef struct simple_header
{
    uint16_t     signature;
    uint16_t     mod_size;       // file size mod 512
    uint16_t     file_size;      // file size in blocks
    uint16_t     num_relocs;     // always zero.
    uint16_t     hdr_size;       // size of header in paragraphs
    uint16_t     min_data;       // minimum data in 4K pages.
    uint16_t     max_data;       // maximum data in 4K pages.
    uint32_t     ESP;            // initial ESP value.
    uint16_t     checksum;
    uint32_t     EIP;            // initial EIP value.
    uint16_t     reloc_offset;   // always 0x1E.
    uint16_t     overlay_num;    // always 0.
    uint16_t     always_one;     // always 1.
} simple_header;

typedef struct extended_header
{
    uint16_t     signature;
    uint16_t     format_level;
    uint16_t     header_size;    // in bytes
    uint32_t     file_size;      // in bytes;
    uint16_t     checksum;
    uint32_t     rtp_offset;     // byte offset of run-time param block.
    uint32_t     rtp_size;       // byte size of run-time param block.
    uint32_t     reloc_offset;   // byte offset of relocation table.
    uint32_t     reloc_size;     // byte size of relocation table.
    uint32_t     sit_offset;     // byte offset of segment info table.
    uint32_t     sit_size;       // byte size of segment info table.
    uint16_t     sit_entry_size; // byte size of a seg info table entry
    uint32_t     load_offset;    // byte offset of load image in file.
    uint32_t     load_size;      // byte size of load image.
    uint32_t     sym_offset;     // byte offset of symbol table in file
    uint32_t     sym_size;       // byte size of symbol table.
    uint32_t     gdt_offset;     // byte offset of GDT in load image
    uint32_t     gdt_size;       // byte size of GDT in load image.
    uint32_t     ldt_offset;     // byte offset of LDT in load image.
    uint32_t     ldt_size;       // byte size of LDT in load image.
    uint32_t     idt_offset;     // byte offset of IDT in load image.
    uint32_t     idt_size;       // byte size of idt in load image.
    uint32_t     tss_offset;     // byte offset of tss in load image.
    uint32_t     tss_size;       // byte size of tss in load image.
    uint32_t     min_extra;      // minimum # of extra bytes to allocate.
    uint32_t     max_extra;      // maximum # of extra bytes to allocate.
    uint32_t     offset;         // offset of program image.
    uint32_t     ESP;
    uint16_t     SS;
    uint32_t     EIP;
    uint16_t     CS;
    uint16_t     LDT;
    uint16_t     TSS;
    uint16_t     flags;          // load image flags.
    uint32_t     mem_req;        // byte load image memory requirements
    uint32_t     reserved1;      // must be zero
    uint32_t     stack_size;     // initial stack size
} extended_header;

// this value is for the flags field
#define PHAR_PACK   0x0001

typedef struct rtpblock
{        // run-time parameter block.
    uint16_t     signature;
    uint16_t     minreal;
    uint16_t     maxreal;
    uint16_t     minibuf;
    uint16_t     maxibuf;
    uint16_t     nistack;
    uint16_t     istksize;
    uint32_t     realbreak;
    uint16_t     callbufs;
    uint16_t     extender_flags;     /* for undocumented "runtime flags" */
    uint16_t     unpriv;
} rtpblock;

typedef struct
{
    uint16_t selector;
    uint16_t flags;
    uint32_t base;
    uint32_t extra;
} seg_info_table;

#define SIMPLE_SIGNATURE    0x504D      // 'MP'
#define EXTENDED_SIGNATURE  0x3350      // 'P3'
#define RTP_SIGNATURE       0x5844      // 'DX'
#define REX_SIGNATURE       0x514D      // 'MQ'

#define PHAR_FORMAT_FLAT        1
#define PHAR_FORMAT_SEGMENTED   2

#include "poppack.h"


