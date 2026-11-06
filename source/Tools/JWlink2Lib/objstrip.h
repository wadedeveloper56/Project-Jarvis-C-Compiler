#pragma once

#include "structs.h"

extern void     RefSeg(segdata*);
extern void     DataRef(symbol*);
extern void     AddEdge(segdata*, symbol*);
extern void     AddSymEdge(symbol*, symbol*);
extern void     AddSymSegEdge(symbol* srcsym, segdata* tarsgdata);
extern void     DefStripSym(symbol*, segdata*);
extern void     DefStripImpSym(symbol* sym);
extern void     CleanStripInfo(symbol*);
extern void     RefGroupSegs(group_entry*);
extern void     ResetObjStrip(void);
