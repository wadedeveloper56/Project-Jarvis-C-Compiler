#pragma once 

#include "structs.h"

bool ProcOS2Import(void);

bool ProcOS2Export(void);

bool ProcAnonExport(void);

bool ProcOS2Segment(void);

entry_export* ProcWlibDLLImportEntry(void);

bool GetWlibImports(void);

bool getimport(void);

bool getexport(void);

bool ProcExpResident(void);

bool ProcPrivate(void);

bool ProcNoName(void);

bool ProcOS2Alignment(void);

bool ProcObjAlign(void);

bool ProcModName(void);

bool ProcNewFiles(void);

bool ProcProtMode(void);

bool ProcOldLibrary(void);

bool ProcOS2HeapSize(void);

bool ProcDescription(void);

bool ProcCommitStack(void);

bool ProcCommitHeap(void);

static bool AddCommit(void);

bool ProcCommit(void);

bool ProcRWRelocCheck(void);

bool ProcSelfRelative(void);

bool ProcInternalRelocs(void);

bool ProcToggleRelocsFlag(void);

bool ProcMixed1632(void);

bool ProcPENoRelocs(void);

bool ProcNoStdCall(void);

bool ProcFuzzyExport(void);

bool ProcOS2(void);

bool ProcPE(void);

#if 1 /* JWlink: [NO]LARGEADDRESSAWARE */
bool ProcLargeAddressAware(void);

bool ProcNoLargeAddressAware(void);
#endif

bool ProcVXD(void);

bool ProcWindows(void);

void ChkBase(offset align);;

void SetOS2Fmt(void);

void FreeOS2Fmt(void);

bool ProcLE(void);

bool ProcLX(void);

bool ProcOS2DLL(void);

bool ProcPhysDevice(void);

bool ProcVirtDevice(void);

bool ProcPM(void);

bool ProcPMCompatible(void);

bool ProcPMFullscreen(void);

bool ProcMemory(void);

bool ProcFont(void);

/* jwlink: DPMI */
bool ProcDPMI(void);

bool ProcDynamicDriver(void);

bool ProcStaticDriver(void);

bool ProcInitGlobal(void);

bool ProcInitInstance(void);

bool ProcInitThread(void);

bool ProcTermGlobal(void);

bool ProcTermInstance(void);

bool ProcTermThread(void);

static bool getsegflags(void);

bool ProcOS2Class(void);

bool ProcSegType(void);

bool ProcSegCode(void);

bool ProcSegData(void);

bool ProcPreload(void);

bool ProcLoadoncall(void);

bool ProcIopl(void);

bool ProcNoIopl(void);

bool ProcExecuteonly(void);

bool ProcExecuteread(void);

bool ProcReadOnly(void);

bool ProcReadWrite(void);

bool ProcShared(void);

/* this is for linear models only - currently PE only */
bool ProcWritable(void);
/* this is for linear models only - currently PE only */
bool ProcExecutable(void);;

bool ProcNonShared(void);

bool ProcConforming(void);

bool ProcNonConforming(void);

bool ProcMovable(void);

bool ProcFixed(void);

bool ProcNonPageable(void);

bool ProcPageable(void);

bool ProcDiscardable(void);

bool ProcNonDiscardable(void);

bool ProcOS2Dynamic(void);

bool ProcInvalid(void);;

bool ProcPermanent(void);

bool ProcNonPermanent(void);

bool ProcContiguous(void);

bool ProcSingle(void);

bool ProcMultiple(void);

bool ProcNone(void);

bool ProcRunNative(void);

bool ProcRunWindows(void);

bool ProcRunConsole(void);

bool ProcRunPosix(void);

bool ProcRunOS2(void);

bool ProcRunDosstyle(void);

bool ProcTNT(void);

bool ProcHX(void);

bool ProcNXCompat(void);

static void ParseVersion(void);

static bool AddResource(void);

bool ProcResource(void);

enum
{
	valid_result = 0x01,
	major_valid = 0x02,
	minor_valid = 0x04,
	revision_valid = 0x08
};

typedef struct tagVersBlock
{
	uint32_t major;
	uint32_t minor;
	uint32_t revision;
}VersBlock;

static uint32_t ProcGenericVersion(VersBlock* pVers, uint32_t major_limit, uint32_t minor_limit, uint32_t revision_limit);

bool     ProcLinkVersion(void);

bool     ProcOsVersion(void);

bool     ProcChecksum(void);
