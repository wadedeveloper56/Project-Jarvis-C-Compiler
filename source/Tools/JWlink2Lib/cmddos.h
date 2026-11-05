#pragma once 

using namespace std;

void SetDosFmt(void);

bool ProcDos(void);

static void SetOvlClasses(void);

static bool AddClass(void);

bool ProcOverlay(void);

bool ProcDistribute(void);

bool ProcPadSections(void);

bool ProcFixedLib(void);

// this is an arbitrary non-zero value put in the sect->relocs field to
// signify that ProcBegin already made a new section, so ProcSection
// should not.

#define SECT_ALREADY_MADE 1

bool ProcBegin(void);

bool ProcInto(void);

static void NewArea(section* sect);

static void MakeNonArea(void);

bool ProcEnd(void);

void MakeNewSection(void);

bool ProcSection(void);

bool ProcSmall(void);

bool ProcCom(void);

bool ProcDynamic(void);

static bool AddNoVector(void);

bool ProcNoVector(void);

static bool AddVector(void);

bool ProcVector(void);

static bool AddForceVector(void);

bool ProcForceVector(void);

bool ProcAutoSection(void);

bool ProcNoIndirect(void);

#if 1 /* JWLink: KNOWEAS */
bool ProcKnowEAS(void);
#endif
bool ProcStandard(void);

bool ProcArea(void);

void CmdOvlFini(void);

#ifdef _INT_DEBUG
static void PrintOvl(void);
static void PrintAreas(OVL_AREA* ovlarea);
static void PrintSect(section* sect);
#endif
extern int     OvlLevel;
