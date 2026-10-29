#include "pch.h"
#include "globals.h"
#include "start.h"

using namespace std;

void SetSegMask(void)
{
	FmtData.SegShift = 16 - FmtData.Hshift;
	FmtData.SegMask = (1 << FmtData.SegShift) - 1;
}

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

int _bgetcmd(char* buffer, int len)
{
	int     total;
	int     i;
	char* word;
	char* p = NULL;
	char** argv = &_argv[1];

	--len; // reserve space for NULL byte

	if (buffer && (len > 0))
	{
		p = buffer;
		*p = '\0';
	}

	/* create approximation of original command line */
	for (word = *argv++, i = 0, total = 0; word; word = *argv++)
	{
		i = strlen(word);
		total += i;

		if (p)
		{
			if (i >= len)
			{
				strncpy(p, word, len);
				p[len] = '\0';
				p = NULL;
				len = 0;
			}
			else
			{
				strcpy(p, word);
				p += i;
				len -= i;
			}
		}
		/* account for at least one space separating arguments */
		if (*argv)
		{
			if (p)
			{
				*p++ = ' ';
				--len;
			}
			++total;
		}
	}
	return(total);
}

char* getcmd(char* buffer)
{
	_bgetcmd(buffer, 1024);
	return(buffer);
}

void GetCmdLine(char* buff)
{
	getcmd(buff);
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

void Help(void)
{
	//LnkMsg(INF + MSG_USAGE, NULL);
}

void DoCmdFile(char* fname)
{
	exe_format  possible;
	f_handle    file;
	size_t      namelen;
	file_defext extension;
	char* namelnk;

	ResetCmdFile();
	if (fname == NULL || *fname == '\0')
	{
		fname = (char*)malloc(10 * 1024);  // arbitrarily large buffer that won't
		GetCmdLine(fname);            // be overflowed
		NewCommandSource(NULL, fname, COMMANDLINE);
	}
	else
	{
		NewCommandSource(NULL, fname, ENVIRONMENT);
	}
	if (IsStdOutConsole())
	{
		DO_OR_EQ(commandflag, CmdFlags, |=, CF_TO_STDOUT);
	}
	while (*fname == ' ')
	{
		fname++;
	}
	//if (QSysHelp(&Token.next))
	//{
	//	Help();
	//}
	if (*fname == '?')
	{
		Token.next = fname + 1;       // skip question mark.
		Help();

	}
	else if (*fname == '-' || *fname == '/')
	{
		if (*(fname + 1) == '?' || *(fname + 1) == 'h')
		{
			Token.next = fname + 2;     // skip /?
			Help();
		}
	}
	if (*fname == '\0')
	{       // go into interactive mode.
		Token.how = INTERACTIVE;
		Token.where = ENDOFLINE;
		//LnkMsg(INF + MSG_PRESS_CTRL_Z, NULL);
	}
	namelnk = GetEnvString(INIT_FILE_ENV);
	file = (namelnk != NULL) ? SearchPath(namelnk) : NIL_HANDLE;
	if (file == NIL_HANDLE)
	{
		namelnk = INIT_FILE_NAME;
		file = SearchPath(namelnk);
	}
	if (file != NIL_HANDLE)
	{
		fname = ChkStrDup(namelnk);
		SetCommandFile(file, fname);
	}
	if (Spawn(DoCmdParse))
	{
		//Ignite();
		//Suicide();
	}
	GetExtraCommands();
	if (!(LinkState & FMT_DECIDED))
	{
		/* restrict set to automatically decided ones */
#define LAST_CHANCE ( MK_OS2_LX             | MK_OS2_NE | MK_WIN_NE | MK_PE | MK_DOS_EXE | MK_WIN_VXD )
		//DEBUG((DBG_BASE, "DoCmdFile: calling HintFormat(LAST_CHANCE)"));
		//HintFormat((exe_format)(LAST_CHANCE));
	}
	else
	{
		/* restrict to a unique type */
		for (possible = (exe_format)1; possible != 0; possible = (exe_format)(possible << 1))
		{
			if (FmtData.type & possible)
			{
				FmtData.type = possible;
				break;
			}
		}
	}
	if (FmtData.type & (MK_NOVELL | MK_DOS) && LinkFlags & INC_LINK_FLAG)
	{
		//LnkMsg(FTL + MSG_FORMAT_BAD_OPTION, "s", "incremental");
	}
#ifdef _NOVELL
	if (FmtData.type & MK_NOVELL)
	{
		//CmdNovFini();
	}
	else
#endif
		if (FmtData.type & MK_OVERLAYS)
		{
			CmdOvlFini();
			AddObjLib("wovl.lib", LIB_PRIORITY_MIN);     // add a reference to wovl.lib
		}
	if (Name == NULL || !(CmdFlags & CF_HAVE_FILES))
	{
		Ignite();
		//LnkMsg(FTL + MSG_NO_FILES_FOUND, NULL);
	}
	namelen = strlen(Name);
	if (MapFlags & MAP_FLAG)
	{
		if (MapFName == NULL)
		{
			MapFName = FileName(Name, namelen, E_MAP, TRUE);
		}
	}
	else
	{
		MapFlags = (mapflag)0;   // if main isn't set, don't set anything.
	}
	if (SymFileName == NULL && (CmdFlags & CF_SEPARATE_SYM ||
		(LinkFlags & OLD_DBI_FLAG && FmtData.type & MK_COM)))
	{
		SymFileName = FileName(Name, namelen, E_SYM, TRUE);
	}
	if (FmtData.make_implib && FmtData.implibname == NULL)
	{
		if (FmtData.make_impfile)
		{
			extension = E_LBC;
		}
		else
		{
			extension = E_LIBRARY;
		}
		FmtData.implibname = FileName(Name, namelen, extension, TRUE);
	}
	//CheckTraces();
	//BurnUtils();
	//PruneSystemList();
	//NumberSections();
	//DBIInit();
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
