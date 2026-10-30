#include "pch.h"
#include "util.h"
#include "globals.h"

bool ProcFiles(void){ return false;}
bool ProcModFiles(void){ return false;}
bool ProcLibrary(void){ return false;}
bool ProcName(void){ return false;}
bool ProcOptions(void){ return false;}
bool ProcDebug(void){ return false;}
bool ProcSystem(void){ return false;}
bool ProcLibPath(void){ return false;}
bool ProcLibFile(void){ return false;}
bool ProcPath(void){ return false;}
bool ProcFormat(void)
{
	if (LinkState & FMT_SPECIFIED)
	{
		//LnkMsg(LOC + LINE + FTL + MSG_MULTIPLE_MODES_FOUND, NULL);
	}
	LinkState |= FMT_SPECIFIED;
	return(ProcOne(Models, SEP_NO, TRUE));
}
bool ProcModTrace(void){ return false;}
bool ProcSymTrace(void){ return false;}
bool ProcAlias(void){ return false;}
bool ProcReference(void){ return false;}
bool ProcDisable(void){ return false;}
bool ProcSort(void){ return false;}
bool ProcLanguage(void){ return false;}
bool ProcStartLink(void){ return false;}
bool ProcOptLib(void){ return false;}
bool ProcOrder(void){ return false;}
bool ProcOutput(void){ return false;}
bool ProcResource(void){ return false;}
bool ProcCommit(void){ return false;}
bool ProcAnonExport(void){ return false;}
bool ProcImport(void){ return false;}
bool ProcExport(void){ return false;}
bool ProcSegment(void){ return false;}
bool ProcOverlay(void){ return false;}
bool ProcBegin(void){ return false;}
bool ProcFixedLib(void){ return false;}
bool ProcNoVector(void){ return false;}
bool ProcVector(void){ return false;}
bool ProcForceVector(void){ return false;}
bool ProcRuntime(void){ return false;}
bool ProcModule(void){ return false;}
bool ProcNewSegment(void){ return false;}
bool ProcDos(void) { return false; }