#include "pch.h"
#include "cmdelf.h"
#include "structs.h"
#include "globals.h"
#include "util.h"

bool ProcELF(void)
{
	ProcOne(ELFFormatKeywords, SEP_NO, FALSE);
	return TRUE;
}
