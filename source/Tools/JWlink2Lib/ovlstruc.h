#pragma once

#include "structs.h"
//#include "ovltab.h"

typedef struct vecnode
{
    struct vecnode* next;
    symbol* entry;
} vecnode;

#define DefOvlClass     "CODE"
#define OVL_CL_LEN      ( sizeof( DefOvlClass ) - 1 )
#define OvlMgrClass     "_OVLCODE"
#define OVL_MGR_CL_LEN  ( sizeof( OvlMgrClass ) - 1 )
#define _OvltabName     "__OVLTAB__"
#define OVLTAB_NAMELEN  ( sizeof( _OvltabName ) - 1 )
#define _OvltabEndName  "__OVLTABEND__"
#define _OvlVecStartName "__OVLSTARTVEC__"
#define _OvlVecEndName  "__OVLENDVEC__"
#define _LongOvlldr     "__LOVLLDR__"
#define _ShortOvlldr    "__SOVLLDR__"
#define _LongOvlInitRtn "__LOVLINIT__"
#define _ShortOvlInitRtn "__SOVLINIT__"
#define _DynamicOvlldr   "__NOVLLDR__"
#define _DynamicInitRtn  "__NOVLINIT__"

#define _OvlfnameLength strlen( OvlFName )
