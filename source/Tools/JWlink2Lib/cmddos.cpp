#include "pch.h"
#include "cmdnov.h"
#include "structs.h"
#include "globals.h"
#include "util.h"

using namespace std;

::byte            OvlLevel;

bool ProcDos(void)
{
	OvlLevel = (::byte)0;
	ProcOne(DosOptions, SEP_NO, FALSE);
	return(TRUE);
}
