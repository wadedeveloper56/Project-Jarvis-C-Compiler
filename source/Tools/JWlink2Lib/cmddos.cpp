#include "pch.h"
#include "structs.h"
#include "globals.h"
#include "util.h"
#include "command.h"
#include "cmdall.h"
#include "cmdline.h"
#include "cmddos.h"
#include "ovlstruc.h"
#include "start.h"
#include "lsymtab.h"

using namespace std;
int            OvlLevel;

#ifdef _INT_DEBUG
static void             PrintOvl(void);
static void             PrintAreas(OVL_AREA* ovlarea);
static void             PrintSect(section* sect);
#endif
static bool             AddClass(void);
static void             NewArea(section* sect);

void SetDosFmt(void)
{
	Extension = E_LOAD;
}

bool ProcDos(void)
{
	OvlLevel = 0;
	ProcOne(DosOptions, SEP_NO, FALSE);
	return(TRUE);
}

static void SetOvlClasses(void)
{
	list_of_names* ovlmgr;

	if (OvlClasses != NULL)
	{
		ovlmgr = (list_of_names*)malloc(sizeof(list_of_names) + OVL_MGR_CL_LEN);
		//_PermAlloc(list_of_names*, ovlmgr, sizeof(list_of_names) + OVL_MGR_CL_LEN);
		memcpy(ovlmgr->name, OvlMgrClass, OVL_MGR_CL_LEN + 1);
		ovlmgr->next_name = OvlClasses;
		OvlClasses = ovlmgr;
	}
}

static bool AddClass(void)
{
	list_of_names* ovlclass;

	ovlclass = (list_of_names*)malloc(sizeof(list_of_names) + Token.len);
	//_PermAlloc(list_of_names*, ovlclass, sizeof(list_of_names) + Token.len);
	memcpy(ovlclass->name, Token.this1, Token.len);
	ovlclass->name[Token.len] = '\0';
	ovlclass->next_name = OvlClasses;
	OvlClasses = ovlclass;
	return(TRUE);
}

bool ProcOverlay(void)
{
	return(ProcArgList(&AddClass, TOK_INCLUDE_DOT));
}

bool ProcDistribute(void)
{
	FmtData.u.dos.distribute = TRUE;
	return(TRUE);
}

bool ProcPadSections(void)
{
	FmtData.u.dos.pad_sections = TRUE;
	return(TRUE);
}

bool ProcFixedLib(void)
{
	bool    ret;

	DO_OR_EQ(commandflag, CmdFlags, |=, CF_SET_SECTION);
	ret = ProcLibrary();
	DO_AND_EQ(commandflag, CmdFlags, &= ~, CF_SET_SECTION);
	return(ret);
}

// this is an arbitrary non-zero value put in the sect->relocs field to
// signify that ProcBegin already made a new section, so ProcSection
// should not.
#define SECT_ALREADY_MADE 1

bool ProcBegin(void)
{
	section* oldsect = NULL;
	file_list** oldflist = NULL;
	section* sect = NULL;

	LinkState |= FMT_SPECIFIED;      // she must want DOS mode.
	if ((OvlLevel > 0) && FmtData.u.dos.dynamic)
	{
		DO_AND_EQ(commandflag, CmdFlags, &= ~, CF_AUTOSECTION);        // merge old area with this.
	}
	else
	{
		oldsect = CurrSect;
		oldflist = CurrFList;
		sect = NewSection();
		if (LinkFlags & ANY_DBI_FLAG)
		{
			DBISectInit(sect);
		}
		NewArea(sect);
		sect->relocs = SECT_ALREADY_MADE;
		CurrSect = sect;
		CurrFList = &sect->files;
	}
	OvlLevel++;
	while (ProcOne(Sections, SEP_NO, FALSE) != FALSE) {}  // NULL LOOP
	if ((OvlLevel == 0) || !FmtData.u.dos.dynamic)
	{
		CurrFList = oldflist;
		CurrSect = oldsect;
	}
	return(TRUE);
}

bool ProcInto(void)
{
	bool ret;

	ret = GetToken(SEP_NO, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME));
	if (ret)
	{
		CurrSect->outfile = NewOutFile(FileName(Token.this1, Token.len, E_OVL, FALSE));
	}
	else
	{
		//LnkMsg(LOC + LINE + WRN + MSG_DIRECTIVE_ERR, "s", "into");
	}
	return(ret);
}

static void NewArea(section* sect)
{
	ovl_area* ovl;
	ovl_area** owner;

	ovl = (ovl_area*)malloc(sizeof(ovl_area));
	//_PermAlloc(ovl_area*, ovl, sizeof(ovl_area));
	ovl->next_area = NULL;
	ovl->sections = sect;
	sect->parent = CurrSect;
	owner = &CurrSect->areas;
	while (*owner != NULL)
	{
		owner = &(*owner)->next_area;
	}
	*owner = ovl;
}

static void MakeNonArea(void)
{
	NonSect = NewSection();   // No debug info in nonsect.
	NewArea(NonSect);
}

bool ProcEnd(void)
{
	if (OvlLevel > 0)
	{  // OvlLevel should always be > 0, but just in case..
		OvlLevel--;
	}
	if (CurrSect->relocs != 0)
	{   // this only happens if the user specifies
		CurrSect->relocs = 0;       // an overlay area with no files in it.
	}
	return(FALSE);    /*  cause loop to be exited in ProcBegin */
}

void MakeNewSection(void)
{
	section* sect;

	if (CurrSect->relocs != SECT_ALREADY_MADE)
	{
		sect = NewSection();
		if (LinkFlags & ANY_DBI_FLAG)
		{
			DBISectInit(sect);
		}
		if (CmdFlags & CF_AUTOSECTION)
		{
			sect->outfile = CurrSect->outfile;
		}
		sect->parent = CurrSect->parent;
		CurrSect->next_sect = sect;
		CurrSect = sect;
		CurrFList = &sect->files;
	}
	else
	{
		CurrSect->relocs = 0;
	}
}

bool ProcSection(void)
{
	if (OvlLevel == 0)
	{
		//LnkMsg(LOC + LINE + WRN + MSG_NO_SECTION_IN_ROOT, NULL);
	}
	else
	{
		MakeNewSection();
		ProcOne(SectOptions, SEP_NO, FALSE);      // check for INTO
		while (ProcOne(Directives, SEP_NO, FALSE) != FALSE)
		{
			RestoreParser();
		}
	}
	return(TRUE);
}

bool ProcSmall(void)
{
	FmtData.u.dos.ovl_short = TRUE;
	return(TRUE);
}

bool ProcCom(void)
{
	Extension = E_COM;
	return(TRUE);
}

bool ProcDynamic(void)
{
	FmtData.u.dos.dynamic = TRUE;
	return(TRUE);
}

static bool AddNoVector(void)
{
	symbol* sym;

	sym = SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), Token.this1, Token.len);
	sym->u.d.ovlstate |= (OVL_FORCE | OVL_NO_VECTOR);
	return(TRUE);
}

bool ProcNoVector(void)
{
	return(ProcArgList(AddNoVector, TOK_INCLUDE_DOT));
}

static bool AddVector(void)
{
	Vectorize(SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), Token.this1, Token.len));
	return(TRUE);
}

bool ProcVector(void)
{
	return(ProcArgList(AddVector, TOK_INCLUDE_DOT));
}

static bool AddForceVector(void)
{
	symbol* sym;

	sym = SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), Token.this1, Token.len);
	Vectorize(sym);
	sym->u.d.ovlstate |= OVL_ALWAYS;
	return(TRUE);
}

bool ProcForceVector(void)
{
	return(ProcArgList(AddForceVector, TOK_INCLUDE_DOT));
}

bool ProcAutoSection(void)
{
	if (OvlLevel == 0)
	{
		//LnkMsg(LOC + LINE + WRN + MSG_NO_SECTION_IN_ROOT, NULL);
	}
	else
	{
		MakeNewSection();
		ProcOne(SectOptions, SEP_NO, FALSE);      // check for INTO
		DO_OR_EQ(commandflag, CmdFlags, |=, CF_AUTOSECTION | CF_SECTION_THERE);
		while (ProcOne(Directives, SEP_NO, FALSE) != FALSE) {}
		DO_AND_EQ(commandflag, CmdFlags, &= ~, CF_AUTOSECTION);
	}
	return(TRUE);
}

bool ProcNoIndirect(void)
{
	FmtData.u.dos.noindirect = TRUE;
	return(TRUE);
}

#if 1 /* JWLink: KNOWEAS */
bool ProcKnowEAS(void)
{
	FmtData.u.dos.knoweas = TRUE;
	return(TRUE);
}
#endif

bool ProcStandard(void)
{
	FmtData.u.dos.dynamic = FALSE;
	return(TRUE);
}

bool ProcArea(void)
{
	//unsigned long   value;
	uint32_t           value;
	bool            ret;

	ret = GetLong(&value);
	if (ret)
	{
		AreaSize = (value + FmtData.SegMask) >> FmtData.SegShift;
	}
	return(ret);
}

void CmdOvlFini(void)
{
	if (OvlLevel != 0)
	{
		//Ignite();
		//LnkMsg(LOC + LINE + FTL + MSG_EXPECTING_END, NULL);
	}
	if (FmtData.u.dos.dynamic &&
		((Root->areas == NULL) || (Root->areas->next_area != NULL)))
	{
		//Ignite();
		//LnkMsg(LOC + LINE + FTL + MSG_INCORRECT_NUM_AREAS, NULL);
	}
	SetOvlClasses();
	MakeNonArea();
#ifdef _INT_DEBUG
	PrintOvl();
#endif
}

#ifdef _INT_DEBUG
static void PrintOvl(void)
{
	OvlLevel = 0;
	PrintAreas(Root->areas);
}

static void PrintAreas(OVL_AREA* ovlarea)
{
	for (; ovlarea != NULL; ovlarea = ovlarea->next_area)
	{
		//DEBUG((DBG_OLD, ""));
		//DEBUG((DBG_OLD, ""));
		//DEBUG((DBG_OLD, "Begin OverLay Area"));
		PrintSect(ovlarea->sections);
		//DEBUG((DBG_OLD, ""));
		//DEBUG((DBG_OLD, "End OverLay Area"));
	}
}


static void PrintSect(section* sect)
{
	file_list* list;

	OvlLevel++;
	for (; sect != NULL; sect = sect->next_sect)
	{
		//DEBUG((DBG_OLD, ""));
		//DEBUG((DBG_OLD, "OverLay #%d   Level %d", sect->ovl_num, OvlLevel));
		//DEBUG((DBG_OLD, "Files:"));
		if (sect->files == NULL)
		{
			//DEBUG((DBG_OLD, "\"Non-section\""));
		}
		for (list = sect->files; list != NULL; list = list->next_file)
		{
			//DEBUG((DBG_OLD, "%s", list->file->name));
		}
		PrintAreas(sect->areas);
	}
	OvlLevel--;
}
#endif
