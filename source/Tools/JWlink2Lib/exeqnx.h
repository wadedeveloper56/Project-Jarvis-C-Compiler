#pragma once

#include "pshpack1.h"

#define QNX_VERSION 400

#define QNX_MAX_REC_SIZE 0x7E00       
#define QNX_MAX_FIXUPS   0x7E00            

#define QNX_READ_WRITE  0
#define QNX_READ_ONLY   1
#define QNX_EXEC_READ   2
#define QNX_EXEC_ONLY   3

enum
{
    LMF_HEADER_REC = 0,
    LMF_COMMENT_REC,
    LMF_LOAD_REC,
    LMF_FIXUP_REC,
    LMF_8087_FIXUP_REC,
    LMF_IMAGE_END_REC,
    LMF_RESOURCE_REC,
    LMF_RW_END_REC,
    LMF_LINEAR_FIXUP_REC
};

typedef struct lmf_record
{
    uint8_t      rec_type;
    uint8_t      reserved;          
    uint16_t     data_nbytes;           
    uint16_t     spare;             
} lmf_record;

typedef struct lmf_data
{
    uint16_t     segment;
    uint32_t     offset;
} lmf_data;

#define _TCF_LONG_LIVED                 0x0001
#define _TCF_32BIT                      0x0002
#define _TCF_PRIV_MASK                  0x000c
#define _TCF_FLAT                       0x0010

#define SEG16_CODE_FIXUP                0x0004
#define LINEAR32_CODE_FIXUP             0x80000000
#define LINEAR32_SELF_RELATIVE_FIXUP    0x40000000

typedef struct lmf_header
{
    uint16_t     version;
    uint16_t     cflags;
    uint16_t     cpu;             
    uint16_t     fpu;              
    uint16_t     code_index;         
    uint16_t     stack_index;         
    uint16_t     heap_index;          
    uint16_t     argv_index;           
    uint16_t     spare2[4];         
    uint32_t     code_offset;        
    uint32_t     stack_nbytes;     
    uint32_t     heap_nbytes;         
    uint32_t     image_base;         
    uint32_t     spare3[2];
} lmf_header;

typedef struct lmf_eof
{
    uint8_t  spare[6];
} lmf_eof;

enum
{
    RES_USAGE = 0
};

typedef struct lmf_resource
{
    uint16_t res_type;
    uint16_t spare[3];
} lmf_resource;

typedef struct lmf_rw_end
{
    uint16_t     verify;
    uint32_t     signature;
} lmf_rw_end;

#define VERIFY_OFFSET 36

#include "poppack.h"

