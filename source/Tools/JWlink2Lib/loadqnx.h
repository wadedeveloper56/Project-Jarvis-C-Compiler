#pragma once

#include <stdint.h>

typedef struct qnx_seg_flags
{
    struct qnx_seg_flags* next;
    uint16_t             flags;        
    char* name;
    bool                    isclass;          
} qnx_seg_flags;

#define QNX_PRIV_SHIFT  0x0002
#define QNX_RES_FILE    0x0080
#define QNX_FLAG_MASK   0x007F

#define QNX_SELECTOR( num ) (((num) << 3) | 4 | FmtData.u.qnx.priv_level)

#define QNX_SEL_NUM(selector) ((selector) >> 3)

#define QNX_DEFAULT_BASE 0x1000

#define QNX_GROUP_ALIGN 0x1000
