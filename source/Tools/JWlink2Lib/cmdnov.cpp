#include "pch.h"
#include "cmdnov.h"
#include "structs.h"
#include "globals.h"

bool ProcNLM(void)
{
	Extension = E_NLM;
	FmtData.u.nov.moduletype = 0;
	return(TRUE);
}
