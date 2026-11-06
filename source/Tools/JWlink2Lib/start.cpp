#include "pch.h"
#include "globals.h"
#include "start.h"
#include "ntio.h"
#include "util.h"

#define INIT_FILE_NAME  "jwlink.lnk"
#define INIT_FILE_ENV   "JWLINK_LNK"

using namespace std;

void ResetCmdFile(void)
{
	SysBlocks = NULL;
	Extension = E_LOAD;
	Name = NULL;
	CmdFlags = CF_UNNAMED;
	Path = NULL;
	memset(&FmtData, 0, sizeof(FmtData));
	FmtData.base = NO_BASE_SPEC;
	FmtData.objalign = NO_BASE_SPEC;
	FmtData.type = MK_ALL;
	FmtData.def_seg_flags = SEG_LEVEL_3;
	FmtData.output_raw = FALSE;
	FmtData.output_hex = FALSE;
	FmtData.Hshift = 12;   // May want different value for some 32 bit segmented modes
	FmtData.FillChar = 0;  // Default fillchar for segment alignment
	SetSegMask();
	CurrSect = Root;
	CurrFList = &Root->files;
	DBIFlag = (::byte)0;        /*  default is only global information */
}

void NewCommandSource(char* name, char* buff, method how)
{
	cmdfilelist* newfile;

	newfile = (cmdfilelist*)malloc(sizeof(cmdfilelist));
	newfile->file = STDIN_HANDLE;
	if (CmdFile != NULL)
	{
		memcpy(&CmdFile->token, &Token, sizeof(tok));
		newfile->next = CmdFile->next;
		if (newfile->next != NULL)
		{
			newfile->next->prev = newfile;
		}
	}
	else
	{
		newfile->next = NULL;
	}
	newfile->prev = CmdFile;
	if (newfile->prev != NULL)
	{
		newfile->prev->next = newfile;
	}
	CmdFile = newfile;
	CmdFile->name = name;
	CmdFile->token.buff = buff;     /* make sure token is freed */
	CmdFile->token.how = how;       /* but only if it needs to be */
	Token.buff = buff;
	Token.next = Token.buff;
	Token.where = MIDST;
	Token.line = 1;
	Token.how = how;
	Token.thumb = OK;
	Token.locked = FALSE;
	Token.quoted = 0;
}

void DoCmdParse(void)
{
	while (GetToken(SEP_END, TOK_INCLUDE_DOT) == FALSE)
	{
	if (ProcOne(Directives, SEP_NO, TRUE) == FALSE)
		{
//			Syntax();
		}
		RestoreParser();
	}
}

void GetExtraCommands(void)
{

}

void DoLink(char* cmdline)
{
	DoCmdFile(cmdline);
}

section* NewSection(void)
{
	section* sect;

	OvlNum++;
	sect = (section*)malloc(sizeof(section));
	sect->next_sect = NULL;
	sect->classlist = NULL;
	sect->orderlist = NULL;
	sect->areas = NULL;
	sect->files = NULL;
	//sect->modFilesHashed = CreateHTable(256, StringiHashFunc, stricmp_wrapper, ChkLAlloc, LFree);
	sect->mods = NULL;
	sect->reloclist = NULL;
	sect->sect_addr.off = 0;
	sect->sect_addr.seg = UNDEFINED;
	sect->ovl_num = 0;
	sect->parent = NULL;
	sect->relocs = 0;
	sect->size = 0;
	sect->outfile = NULL;
	sect->u.dist_mods = NULL;
	sect->dbg_info = NULL;
	return(sect);
}

void ResetSubSystems(void)
{
	OvlNum = 0;
	Root = NewSection();
}

void LinkMeBaby(void)
{
	ResetSubSystems();
	DoLink(ArgSave);
}

int Spawn(void (*fn)(void))
{
	void* save_env;
	jmp_buf env;
	int     status;

	save_env = SpawnStack;
	SpawnStack = env;
	status = setjmp(env);
	if (status == 0)
	{
		(*fn)();
	}
	SpawnStack = save_env;  /* unwind */
	return(status);
}

void LinkMainLine(char* cmds)
{
	ArgSave = cmds;
	Spawn(&LinkMeBaby);
}

void InitSubSystems(void)
{}

void FiniSubSystems(void)
{}

int LinkerMain(int argc, char* argv[]) {
	_argv = argv;
	_argc = argc;
	InitSubSystems();
	LinkMainLine(NULL);
	FiniSubSystems();
	return((LinkState & LINK_ERROR) ? 1 : 0);
}
