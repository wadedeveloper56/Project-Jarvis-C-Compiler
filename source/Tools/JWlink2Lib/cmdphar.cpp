#include "pch.h"
#include "cmdphar.h"
#include "structs.h"
#include "globals.h"
#include "util.h"
#include "command.h"
#include "exephar.h"
#include "cmdos2.h"

bool ProcPharLap(void)
{
    ProcOne(PharModels, SEP_NO, FALSE);
    return(TRUE);
}

void SetPharFmt(void)
{
    Extension = E_PROTECT;
    LinkState &= ~MAKE_RELOCS;
	FmtData.u.phar.params = (rtpblock*)malloc(sizeof(*FmtData.u.phar.params));
    //_ChkAlloc(rtpblock*, FmtData.u.phar.params, sizeof(*FmtData.u.phar.params));
    FmtData.u.phar.mindata = 0;
    FmtData.u.phar.maxdata = 0xFFFFFFFF;
    FmtData.u.phar.breaksym = NULL;
    FmtData.u.phar.stub = NULL;
    FmtData.u.phar.params->minreal = 0;
    FmtData.u.phar.params->maxreal = 0;
    FmtData.u.phar.params->callbufs = 0;
    FmtData.u.phar.params->realbreak = 0;
    FmtData.u.phar.params->minibuf = 1;
    FmtData.u.phar.params->maxibuf = 4;
    FmtData.u.phar.params->nistack = 4;
    FmtData.u.phar.params->istksize = 1;
    FmtData.u.phar.params->extender_flags = 0;
    FmtData.u.phar.params->unpriv = 0;
    ChkBase(4 * 1024);
}

void FreePharFmt(void)
{
    free(FmtData.u.phar.breaksym);
    free(FmtData.u.phar.params);
    free(FmtData.u.phar.stub);
}

bool ProcPharFlat(void)
{
    return(TRUE);
}

bool ProcRex(void)
{
    Extension = E_REX;
    LinkState |= MAKE_RELOCS;         
    return(TRUE);
}

bool ProcPharSegmented(void)
{
    LinkState |= MAKE_RELOCS;         
    return TRUE;
}

#if 0
.exp packing executables implemented yet.

bool ProcPackExp(void)
{
    FmtData.u.phar.pack = TRUE;
    return(TRUE);
}
#endif

bool ProcMinData(void)
{
    return(GetLong(&FmtData.u.phar.mindata));
}

bool ProcMaxData(void)
{
    return(GetLong(&FmtData.u.phar.maxdata));
}

bool ProcUnpriv(void)
{
    FmtData.u.phar.params->unpriv = 1;
    return(TRUE);
}

bool ProcPriv(void)
{
    return(TRUE);
}

bool ProcFlags(void)
{
    bool    ret;
    uint32_t   num;

    ret = GetLong(&num);
    FmtData.u.phar.params->extender_flags = num;
    return(ret);
}

bool ProcMinReal(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        value >>= FmtData.SegShift;           
        if (value > 0xffff)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_TOO_LARGE, "s", "minreal");
        }
        else
        {
            FmtData.u.phar.params->minreal = value;
        }
    }
    return(ret);
}

bool ProcMaxReal(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        value >>= FmtData.SegShift;           
        if (value > 0xffff)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_TOO_LARGE, "s", "maxreal");
        }
        else
        {
            FmtData.u.phar.params->maxreal = value;
        }
    }
    return(ret);
}

bool ProcRealBreak(void)
{
    uint32_t     value;
    ord_state       ok;

    if (!HaveEquals(TOK_INCLUDE_DOT)) return(FALSE);
    ok = getatol(&value);
    if (FmtData.u.phar.breaksym != NULL)
    {
        //LnkMsg(LOC + LINE + WRN + MSG_MULT_REALBREAKS, NULL);
        free(FmtData.u.phar.breaksym);
        FmtData.u.phar.breaksym = NULL;
    }
    if (ok != ST_IS_ORDINAL)
    {        
        FmtData.u.phar.breaksym = tostring();
    }
    else
    {
        FmtData.u.phar.params->realbreak = value;
    }
    return(TRUE);
}

bool ProcCallBufs(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        value >>= 10;           
        if (value > 64)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_TOO_LARGE, "s", "callbufs");
        }
        else
        {
            FmtData.u.phar.params->callbufs = value;
        }
    }
    return(ret);
}

bool ProcMiniBuf(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        value >>= 10;           
        if (value > 64 || value < 1)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_INCORRECT, "s", "minibuf");
        }
        else
        {
            FmtData.u.phar.params->minibuf = value;
        }
    }
    return(ret);
}

bool ProcMaxiBuf(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        value >>= 10;           
        if (value > 64 || value < 1)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_INCORRECT, "s", "maxibuf");
        }
        else
        {
            FmtData.u.phar.params->maxibuf = value;
        }
    }
    return(ret);
}

bool ProcNIStack(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        if (value < 4 || value > 0xFFFF)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_INCORRECT, "s", "nistack");
        }
        else
        {
            FmtData.u.phar.params->nistack = value;
        }
    }
    return(ret);
}

bool ProcIStkSize(void)
{
    uint32_t value;
    bool        ret;

    ret = GetLong(&value);
    if (ret)
    {
        value >>= 10;           
        if (value > 64 || value < 1)
        {
            //LnkMsg(LOC + LINE + WRN + MSG_VALUE_INCORRECT, "s", "istksize");
        }
        else
        {
            FmtData.u.phar.params->istksize = value;
        }
    }
    return(ret);
}
