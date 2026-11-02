#include "pch.h"
#include "cmdphar.h"
#include "structs.h"
#include "globals.h"
#include "util.h"

bool ProcPharLap(void)
{
	ProcOne(PharModels, SEP_NO, FALSE);
	return(TRUE);
}

bool ProcMaxData(void)
{
	return(GetLong(&FmtData.u.phar.maxdata));
}

