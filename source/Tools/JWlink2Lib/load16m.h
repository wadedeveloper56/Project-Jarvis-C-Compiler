#pragma once

#include "structs.h"

enum
{
    TRANS_DATA = 0x0001,   // set if transparent data chosen
    TRANS_STACK = 0x0002,   // set if transparent stack chosen
    FORCE_NO_RELOCS = 0x0004,   // set if no relocs option chosen.
    DATASIZE_SPECD = 0x0008,   // datasize option was chosed.
};

#define TRANS_SPECD (TRANS_DATA | TRANS_STACK)

extern void     Fini16MLoadFile(void);
extern unsigned ToD16MSel(unsigned);
extern segment  Find16MSeg(segment);
extern void     CalcGrpSegs(void);
extern void     MakeDos16PM(void);
