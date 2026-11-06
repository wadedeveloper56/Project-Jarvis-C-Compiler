#include "pch.h"
#include "structs.h"
#include "globals.h"
#include "cmdline.h"
#include "exeos2.h"
#include "command.h"
#include "cmdall.h"
#include "cmdos2.h"
#include "cmdqnx.h"
#include "cmd16m.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "cmdphar.h"
#include "cmddos.h"
#include "util.h"
#include "start.h"
#include "ntio.h"
#include "fileio.h"

using namespace std;

#ifdef _INT_DEBUG
unsigned int            Debug = 1;
#endif

static void             Crash(bool);
static void             Help(void);
static void             DoCmdParse(void);
static void             DisplayOptions(void);

static bool             ProcDosHelp(void);
static bool             ProcOS2Help(void);
static bool             ProcPharHelp(void);
static bool             ProcNovellHelp(void);
static bool             Proc16MHelp(void);
static bool             ProcQNXHelp(void);
static bool             ProcELFHelp(void);
static bool             ProcWindowsHelp(void);
static bool             ProcWinVxdHelp(void);
static bool             ProcNTHelp(void);
static bool             ProcRawHelp(void);
static void             WriteHelp(unsigned first_ln, unsigned last_ln, bool prompt);
static void             GetExtraCommands(void);

static  parse_entry   FormatHelp[] = {
	"Dos",          ProcDosHelp,            MK_ALL,     (commandflag)0,
#ifdef _OS2
	"OS2",          ProcOS2Help,            MK_ALL,     (commandflag)0,
	"WINdows",      ProcWindowsHelp,        MK_ALL,     (commandflag)0,
	"VXD",          ProcWinVxdHelp,         MK_ALL,     (commandflag)0,
	"PE",           ProcNTHelp,             MK_ALL,     (commandflag)0,
#endif
#ifdef _PHARLAP
	"PHARlap",      ProcPharHelp,           MK_ALL,     (commandflag)0,
#endif
#ifdef _NOVELL
	"NOVell",       ProcNovellHelp,         MK_ALL,     (commandflag)0,
#endif
#ifdef _DOS16M
	"DOS16M",       Proc16MHelp,            MK_ALL,     (commandflag)0,
#endif
#ifdef _QNXLOAD
	"QNX",          ProcQNXHelp,            MK_ALL,     (commandflag)0,
#endif
#ifdef _ELF
	"ELF",          ProcELFHelp,            MK_ALL,     (commandflag)0,
#endif
#ifdef _RAW
	"Raw",          ProcRawHelp,            MK_ALL,     (commandflag)0,
#endif
	NULL
};

file_defext     Extension;
file_list** CurrFList;
tok             Token;
commandflag     CmdFlags;
char* Name;
sysblock* SysBlocks;
sysblock* LinkCommands;

static sysblock* PrevCommand;

#define INIT_FILE_NAME  "jwlink.lnk"
#define INIT_FILE_ENV   "JWLINK_LNK"

void InitCmdFile(void)
{
	PrevCommand = NULL;
}

void SetSegMask(void)
{
	FmtData.SegShift = 16 - FmtData.Hshift;
	FmtData.SegMask = (1 << FmtData.SegShift) - 1;
}

static void ResetCmdFile(void)
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
	FmtData.Hshift = 12;             
	FmtData.FillChar = 0;       
	SetSegMask();
	CurrSect = Root;
	CurrFList = &Root->files;
	DBIFlag = (::byte)0;               
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
		fname = (char*)malloc(10 * 1024);
		//FIXME _ChkAlloc(char*, fname, (10 * 1024));       
		GetCmdLine(fname);              
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
	if (QSysHelp(&Token.next))
	{
		Help();
	}
	if (*fname == '?')
	{
		Token.next = fname + 1;          
		Help();
#if defined( __UNIX__ )
	}
	else if (*fname == '-')
	{
#else
	}
	else if (*fname == '-' || *fname == '/')
	{
#endif
		if (*(fname + 1) == '?' || *(fname + 1) == 'h')
		{
			Token.next = fname + 2;       
			Help();
		}
	}
	if (*fname == '\0')
	{           
		Token.how = INTERACTIVE;
		Token.where = ENDOFLINE;
		//FIXME LnkMsg(INF + MSG_PRESS_CTRL_Z, NULL);
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
		Ignite();
		//FIXME FIXME Suicide();
	}
	GetExtraCommands();
	if (!(LinkState & FMT_DECIDED))
	{
#if defined( __QNX__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_QNX )
#elif defined( __LINUX__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_ELF )
#elif defined( __NT__ )
#define LAST_CHANCE ( MK_OS2_LX             | MK_OS2_NE | MK_WIN_NE | MK_PE | MK_DOS_EXE | MK_WIN_VXD )
#else
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_DOS_EXE | MK_PHAR_SIMPLE )
#endif
		//FIXME DEBUG((DBG_BASE, "DoCmdFile: calling HintFormat(LAST_CHANCE)"));
		HintFormat((exe_format)(LAST_CHANCE));
	}
	else
	{
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
		//FIXME LnkMsg(FTL + MSG_FORMAT_BAD_OPTION, "s", "incremental");
	}
#ifdef _NOVELL
	if (FmtData.type & MK_NOVELL)
	{
		CmdNovFini();
	}
	else
#endif
		if (FmtData.type & MK_OVERLAYS)
		{
			CmdOvlFini();
			AddObjLib("wovl.lib", LIB_PRIORITY_MIN);          
		}
	if (Name == NULL || !(CmdFlags & CF_HAVE_FILES))
	{
		Ignite();
		//FIXME LnkMsg(FTL + MSG_NO_FILES_FOUND, NULL);
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
		MapFlags = (mapflag)0;          
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
	//FIXME CheckTraces();
	//FIXME BurnUtils();
	PruneSystemList();
	//FIXME NumberSections();
	//FIXME DBIInit();
}

char* GetNextLink(void)
{
	char* cmd;

	cmd = NULL;
	free(PrevCommand);
	if (LinkCommands != NULL)
	{
		PrevCommand = LinkCommands;
		LinkCommands = LinkCommands->next;
		cmd = PrevCommand->commands;
	}
	return(cmd);
}

#define PREFIX_SIZE 8

struct extra_cmd_info
{
	unsigned    type;
	char        prefix[PREFIX_SIZE + 1];
	bool        retry;
};

static struct extra_cmd_info ExtraCmds[] = {
		EXTRA_NAME_DIR, "name    ",     FALSE,
		EXTRA_OBJ_FILE, "file    ",     TRUE,
		EXTRA_LIB_FILE, "lib     ",     TRUE,
		EXTRA_RES_FILE, "opt res=",     FALSE,
		0,              "\0",           FALSE
};

static void GetExtraCommands(void)
{
	struct extra_cmd_info const* cmd;
	char                                buff[_MAX_PATH + PREFIX_SIZE];

	cmd = ExtraCmds;
	while (cmd->prefix[0] != '\0')
	{
		for (;;)
		{
			memcpy(buff, cmd->prefix, PREFIX_SIZE);
			if (true/*!GetAddtlCommand(cmd->type, buff + PREFIX_SIZE)*/) break;
			NewCommandSource(NULL, buff, COMMANDLINE);
			if (Spawn(DoCmdParse)) break;
			if (!cmd->retry) break;
		}
		cmd++;
	}

}

void Syntax(void)
{
	if (Token.this1 == NULL)
	{
		//FIXME LnkMsg(LOC + LINE + FTL + MSG_DIRECTIVE_ERR_BEGINNING, NULL);
	}
	else
	{
		Token.this1[Token.len] = '\0';
		//FIXME LnkMsg(LOC + LINE + FTL + MSG_DIRECTIVE_ERR, "s", Token.this1);
	}
}

static void Crash(bool check_file)
{
	char        buff[81];
	unsigned    len;
	f_handle    fp;

	if (check_file)
	{
		fp = SearchPath("wlink.hlp");
		if (fp != NIL_HANDLE)
		{
			//FIXME	WLPrtBanner();
			for (;; )
			{
				len = QRead(fp, buff, 80, "wlink.hlp");
				if (len == 0) break;
				buff[len] = '\0';
				//FIXME	WriteStdOut(buff);
			}
			QClose(fp, "wlink.hlp");
			Ignite();
			//FIXME FIXME Suicide();
		}
	}
	DisplayOptions();
	Ignite();
	//FIXME FIXME Suicide();
}

static void Help(void)
{
	EatWhite();
	if (*Token.next == '?')
	{
		Crash(FALSE);
	}
	else if (*Token.next == '\0' || !ProcOne(FormatHelp, SEP_NO, FALSE))
	{
		Crash(TRUE);
	}
	else
	{
		Ignite();
		//FIXME FIXME Suicide();
	}
}

static void DoCmdParse(void)
{
	while (GetToken(SEP_END, TOK_INCLUDE_DOT) == FALSE)
	{
		if (ProcOne(Directives, SEP_NO, TRUE) == FALSE)
		{
			Syntax();
		}
		RestoreParser();
	}
}

static void WriteGenHelp(void)
{
	//FIXME WLPrtBanner();
	//FIXME WriteHelp(MSG_GENERAL_HELP_0, MSG_GENERAL_HELP_LAST, CmdFlags & CF_TO_STDOUT);
}

const int htopics[] = {
#if defined( _QNXLOAD ) && defined( __QNX__ )
	MSG_QNX_HELP_0, MSG_QNX_HELP_LAST,
#endif
	/*
#ifdef _EXE
	MSG_DOS_HELP_0, MSG_DOS_HELP_LAST,
#endif
#ifdef _OS2
	MSG_OS2_HELP_0, MSG_OS2_HELP_LAST,
	MSG_WINDOWS_HELP_0, MSG_WINDOWS_HELP_LAST,
	MSG_WIN_VXD_HELP_0, MSG_WIN_VXD_HELP_LAST,
	MSG_NT_HELP_0, MSG_NT_HELP_LAST,
#endif
#ifdef _PHARLAP
	MSG_PHAR_HELP_0, MSG_PHAR_HELP_LAST,
#endif
#ifdef _NOVELL
	MSG_NOVELL_HELP_0, MSG_NOVELL_HELP_LAST,
#endif
#ifdef _DOS16M
	MSG_DOS16_HELP_0, MSG_DOS16_HELP_LAST,
#endif
#if defined( _QNXLOAD ) && !defined( __QNX__ )
	MSG_QNX_HELP_0, MSG_QNX_HELP_LAST,
#endif
#ifdef _ELF
	MSG_ELF_HELP_0, MSG_ELF_HELP_LAST,
#endif
#ifdef _RAW
	MSG_RAW_HELP_0, MSG_RAW_HELP_LAST,
#endif
*/
0,0
};

#define NUMHTOPICS ( sizeof( htopics ) / ( sizeof( htopics[0] * 2 ) ) )

static void PressKey(void);

static void DisplayOptions(void)
{
	int i;

	WriteGenHelp();
	for (i = 0; i < NUMHTOPICS; i += 2)
	{
#ifndef __UNIX__
		if (i && (CmdFlags & CF_TO_STDOUT))
			PressKey();
#endif
		WriteHelp(htopics[i], htopics[i + 1], CmdFlags & CF_TO_STDOUT);
	}
}

#ifdef _EXE
static bool ProcDosHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_DOS_HELP_0, MSG_DOS_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif
#ifdef _OS2
static bool ProcOS2Help(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_OS2_HELP_0, MSG_OS2_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}

static bool ProcWindowsHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_WINDOWS_HELP_0, MSG_WINDOWS_HELP_LAST,		CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}

static bool ProcWinVxdHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_WIN_VXD_HELP_0, MSG_WIN_VXD_HELP_LAST,	CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}

static bool ProcNTHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_NT_HELP_0, MSG_NT_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif
#ifdef _PHARLAP
static bool ProcPharHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_PHAR_HELP_0, MSG_PHAR_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif
#ifdef _NOVELL
static bool ProcNovellHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_NOVELL_HELP_0, MSG_NOVELL_HELP_LAST,CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif
#ifdef _DOS16M
static bool Proc16MHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_DOS16_HELP_0, MSG_DOS16_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif
#ifdef _QNXLOAD
static bool ProcQNXHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_QNX_HELP_0, MSG_QNX_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif

#ifdef _ELF
static bool ProcELFHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_ELF_HELP_0, MSG_ELF_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif

#ifdef _RAW
static bool ProcRawHelp(void)
{
	WriteGenHelp();
	//FIXME WriteHelp(MSG_RAW_HELP_0, MSG_RAW_HELP_LAST, CmdFlags & CF_TO_STDOUT);
	return(TRUE);
}
#endif

static void WriteMsg(char msg_buffer[]);

static void WriteHelp(unsigned first_ln, unsigned last_ln, bool prompt)
{
	char        msg_buffer[RESOURCE_MAX_SIZE];
	int         previous_null = 0;

	for (; first_ln <= last_ln; first_ln++)
	{
		//FIXME Msg_Get((int)first_ln, msg_buffer);
		if (previous_null)
		{
			if (msg_buffer[0] != '\0')
			{
#ifndef __UNIX__
				PressKey();
#endif
				WriteMsg(msg_buffer);
				previous_null = 0;
			}
			else break;
		}
		else if (msg_buffer[0] == '\0')
		{
			if (prompt)
				previous_null = 1;
		}
		else
		{
			WriteMsg(msg_buffer);
		}
	}
}

static void PressKey(void)
{
	char        msg_buffer[RESOURCE_MAX_SIZE];
	char        result;

	//FIXME Msg_Get(MSG_PRESS_KEY, msg_buffer);
	//FIXME WriteStdOut(msg_buffer);
	result = WaitForKey();
	//FIXME WriteNLStdOut();
	if (tolower(result) == 'q')
	{
		Ignite();
		//FIXME	Suicide();
	}
}

static void WriteMsg(char msg_buffer[])
{
	//FIXME	WriteStdOut(msg_buffer);
	//FIXME	WriteNLStdOut();
}

void FreePaths(void)
{
	//FIXME	FreeList(Path);
	Path = NULL;
	if (Name != NULL)
	{
		free(Name);
		Name = NULL;
	}
}

void Burn(void)
{
	FreePaths();
	if (MapFName != NULL)
	{
		free(MapFName);
		MapFName = NULL;
	}
	//FIXME FreeOutFiles();
	//FIXME BurnUtils();
}

void Ignite(void)
{
	BurnSystemList();
	Burn();
}

void SetFormat(void)
{
	char* fname;

	if (CmdFlags & CF_NO_EXTENSION)
	{
		fname = Name;
	}
	else
	{
		unsigned    len = strlen(Name);

		if (FmtData.output_hex)
		{          
			Extension = E_HEX;            
		}
		else if (FmtData.output_raw)
		{
			Extension = E_BIN;
		}
		fname = FileName(Name, len, Extension, CmdFlags & CF_UNNAMED);
		free(Name);
	}
	Root->outfile = NewOutFile(fname);
	Name = NULL;
	//FIXME FillOutFilePtrs();         
	if (MapFlags & MAP_FLAG)
	{
		//FIXME LnkMsg(MAP + MSG_EXE_NAME, "s", Root->outfile->fname);
		//FIXME LnkMsg(MAP + MSG_CREATE_EXE, "f");
	}
#ifdef _QNXLOAD
	if (FmtData.type & MK_QNX)
	{
		CmdQNXFini();
	}
#endif
	//FIXME SetRelocSize();
}

struct select_format
{
	exe_format      bits;
	char* lib_var_name;
	void            (*set_func)(void);
	void            (*free_func)(void);
};

static struct select_format PossibleFmt[] = {
	(exe_format)MK_DOS,         "LIBDOS",       NULL,           NULL,
#ifdef _DOS16M
	(exe_format)MK_DOS16M,      "LIBDOS16M",    SetD16MFmt,     FreeD16MFmt,
#endif
#ifdef _QNXLOAD
	(exe_format)MK_QNX,         "LIBQNX",       SetQNXFmt,      FreeQNXFmt,
#endif
#ifdef _ELF
	(exe_format)MK_ELF,         "LIBELF",       SetELFFmt,      FreeELFFmt,
#endif
#ifdef _OS2
	(exe_format)MK_WIN_NE,      "LIBWIN",       SetOS2Fmt,      FreeOS2Fmt,
	(exe_format)MK_OS2_NE,      "LIBOS2",       SetOS2Fmt,      FreeOS2Fmt,
	(exe_format)MK_OS2_LE,      "LIBOS2FLAT",   SetOS2Fmt,      FreeOS2Fmt,
	(exe_format)MK_OS2_LX,      "LIBOS2FLAT",   SetOS2Fmt,      FreeOS2Fmt,
	(exe_format)MK_PE,          "LIBPE",        SetOS2Fmt,      FreeOS2Fmt,
	(exe_format)MK_WIN_VXD,     "LIBVXD",       SetOS2Fmt,      FreeOS2Fmt,
#endif
#ifdef _PHARLAP
	(exe_format)MK_PHAR_LAP,    "LIBPHAR",      SetPharFmt,     FreePharFmt,
#endif
#ifdef _NOVELL
	(exe_format)MK_NOVELL,      "LIBNOV",       SetNovFmt,      FreeNovFmt,
#endif
};

#define NUMPOSSIBLEFMT ( sizeof( PossibleFmt ) / sizeof ( PossibleFmt[0] ) )

void AddFmtLibPaths(void)
{
	int i;
	exe_format                  possible;

	if (!(LinkState & FMT_DECIDED)) return;
	for (i = 0; i < NUMPOSSIBLEFMT; i++)
	{
		possible = PossibleFmt[i].bits;
		if ((~possible & FmtData.type) == 0)
		{
			AddEnvPaths(PossibleFmt[i].lib_var_name);
			break;
		}
	}
}

static void InitFmt(void (*set)(void))
{
	if (LinkState & FMT_INITIALIZED) return;
	if (set != NULL) set();
	LinkState |= FMT_INITIALIZED;
}

bool HintFormat(exe_format hint)
{
	int i;
	exe_format                  possible;

	//FIXME DEBUG((DBG_BASE, "HintFormat(%h): enter FmtData.type=%h", hint, FmtData.type));
	if (!(hint & FmtData.type))
		return(FALSE);
	DO_AND_EQ(exe_format, FmtData.type, &=, hint);
	if (LinkState & FMT_DECIDED)
		return(TRUE);
	for (i = 0; i < NUMPOSSIBLEFMT; i++)
	{
		possible = PossibleFmt[i].bits;
		if ((~possible & FmtData.type) == 0)
		{
			//FIXME DEBUG((DBG_BASE, "HintFormat(%h): format %h found", hint, possible));
			InitFmt(PossibleFmt[i].set_func);
			LinkState |= FMT_DECIDED;
			if (LinkState & SEARCHING_LIBRARIES)
				AddFmtLibPaths();
			return(TRUE);
		}
	}
#ifdef _OS2
	if ((~(MK_OS2 | MK_PE | MK_WIN_VXD) & FmtData.type) == 0)
	{
		//FIXME DEBUG((DBG_BASE, "HintFormat(%h): calling InitFmt()", hint));
		InitFmt(SetOS2Fmt);
	}
#endif
	return(TRUE);
}

void DecideFormat(void)
{
	exe_format  possible;
	exe_format  allowed;
	char        rc_buff[RESOURCE_MAX_SIZE];

	if (!(LinkState & FMT_DECIDED))
	{
		possible = FmtData.type;
		allowed = (exe_format)(MK_OS2_NE | MK_OS2_LX);
		if (!(LinkState & FMT_SEEN_IMPORT_CMT)) allowed = (exe_format)(~allowed);
		if ((possible & allowed) != 0) DO_AND_EQ(exe_format, possible, &=, allowed);
		HintFormat(possible);
		if (!(LinkState & FMT_DECIDED))
		{
			//FIXME Msg_Get(MSG_FORMAT_NOT_DECIDED, rc_buff);
			//FIXME LnkMsg(FTL + MSG_INTERNAL, "s", rc_buff);
		}
	}
}

void FreeFormatStuff(void)
{
	int i;
	exe_format                  possible;

	if (!(LinkState & FMT_DECIDED)) return;
	for (i = 0; i < NUMPOSSIBLEFMT; i++)
	{
		possible = PossibleFmt[i].bits;
		if ((~possible & FmtData.type) == 0)
		{
			if (PossibleFmt[i].free_func != NULL)
				PossibleFmt[i].free_func();
			break;
		}
	}
}

void AddCommentLib(char* ptr, unsigned len, lib_priority priority)
{
	file_list* result;

	if (CmdFlags & CF_NO_DEF_LIBS)
		return;
	ptr = FileName(ptr, len, E_LIBRARY, FALSE);
	result = AddObjLib(ptr, priority);
	//FIXME CheckLibTrace(result);
	//FIXME DEBUG((DBG_BASE, "library: %s", ptr));
	free(ptr);
}

#ifndef APP

void AddLibPaths(char* name, unsigned len, bool add_to_front)
{
	path_entry* newpath;
	file_list const* libfiles;

	newpath = (path_entry*)malloc(sizeof(path_entry) + len);
	//_ChkAlloc(path_entry*, newpath, sizeof(path_entry) + len);
	memcpy(newpath->name, name, len);
	newpath->name[len] = '\0';
	if (add_to_front)
	{
		newpath->next = LibPath;
		LibPath = newpath;
	}
	else
	{
		//FIXME LinkList(&LibPath, newpath);
	}
	if (LibPath == newpath)
	{
		libfiles = ObjLibFiles;
		while (libfiles != NULL)
		{
			libfiles->file->path_list = LibPath;
			libfiles = libfiles->next_file;
		}
		libfiles = Root->files;
		while (libfiles != NULL && libfiles->file->flags & INSTAT_USE_LIBPATH)
		{
			libfiles->file->path_list = LibPath;
			libfiles = libfiles->next_file;
		}
	}
}

void AddEnvPaths(char* envname)
{
	char* const        val = GetEnvString(envname);
	unsigned            len;

	if (val == NULL)
		return;
	len = strlen(val);
	AddLibPaths(val, len, FALSE);
}

#endif

void ExecSystem(char* name)
{
	sysblock* sys;

	//FIXME DEBUG((DBG_BASE, "ExecSystem(%s) enter", name));
	sys = FindSysBlock(name);
	if (sys != NULL)
	{
		NewCommandSource(NULL, NULL, ENVIRONMENT);    
		Token.where = ENDOFCMD;          
		NewCommandSource(sys->name, sys->commands, SYSTEM);   
		sys->name = NULL;
		while (GetToken(SEP_END, TOK_INCLUDE_DOT) == FALSE)
		{
			if (ProcOne(SysDirectives, SEP_NO, FALSE) == FALSE)
			{
				//FIXME LnkMsg(LOC + LINE + WRN + MSG_ERROR_IN_SYSTEM_BLOCK, NULL);
				RestoreCmdLine();
				break;
			}
		}
		BurnSystemList();
		//FIXME BurnUtils();
	}
	//FIXME DEBUG((DBG_BASE, "ExecSystem(%s) exit", name));
}

static void CleanSystemList(bool check)
{
	sysblock** sys;
	sysblock* next;
	char* name;

	sys = &SysBlocks;
	while (*sys != NULL)
	{
		name = (*sys)->name;
		if (!check
			|| (memcmp("286", name, 4) != 0 && memcmp("386", name, 4) != 0))
		{
			next = (*sys)->next;
			free(name);
			free(*sys);
			*sys = next;
		}
		else
		{
			sys = &(*sys)->next;
		}
	}
}

void PruneSystemList(void)
{
	CleanSystemList(TRUE);
}

void BurnSystemList(void)
{
	CleanSystemList(FALSE);
}

bool ProcImport(void)
{
#if defined( _OS2) || defined( _ELF ) || defined( _NOVELL )
	if (HintFormat((exe_format)(MK_OS2 | MK_PE)))
	{
		return(ProcOS2Import());
	}
	else if (HintFormat(MK_WIN_VXD))
	{
		return(FALSE);
	}
	else if (HintFormat(MK_ELF))
	{
		return(ProcELFImport());
	}
	else
	{
		return(ProcNovImport());
	}
#else
	return(FALSE);
#endif
}

#if defined(_OS2) || defined(_NOVELL)
bool ProcExport(void)
{
#ifdef _OS2
	if (HintFormat((exe_format)(MK_OS2 | MK_PE | MK_WIN_VXD)))
	{
		return(ProcOS2Export());
	}
	else
#endif
#ifdef _NOVELL
	{
		return(ProcNovExport());
	}
#endif
}
#endif

#if defined( _DOS16M ) || defined( _QNXLOAD ) || defined( _OS2 ) || defined( _ELF )
bool ProcNoRelocs(void)
{
#if defined( _QNXLOAD )
	if (HintFormat((exe_format)MK_QNX))
	{
		return(ProcQNXNoRelocs());
	}
#endif
#if defined( _OS2 )
	if (HintFormat((exe_format)MK_PE))
	{
		return(ProcPENoRelocs());
	}
#endif
#ifdef _DOS16M
	if (HintFormat((exe_format)MK_DOS16M))
	{
		return(Proc16MNoRelocs());
	}
#endif
#if defined( _ELF )
	if (HintFormat((exe_format)MK_ELF))
	{
		return(ProcELFNoRelocs());
	}
#endif
	return(TRUE);
}
#endif

#if defined(_OS2) || defined(_QNXLOAD)
bool ProcSegment(void)
{
#ifdef _OS2
	if (HintFormat((exe_format)(MK_OS2 | MK_PE | MK_WIN_VXD)))
	{
		return(ProcOS2Segment());
	}
#endif
#ifdef _QNXLOAD
	if (HintFormat((exe_format)MK_QNX))
	{
		return(ProcQNXSegment());
	}
#endif
	return(TRUE);
}
#endif

bool ProcAlignment(void)
{
#if defined( _OS2 ) || defined( _ELF )
	if (HintFormat((exe_format)(MK_OS2_16BIT | MK_OS2_LX | MK_PE)))
	{
		return(ProcOS2Alignment());
	}
	else if (HintFormat((exe_format)MK_ELF))
	{
		return(ProcELFAlignment());
	}
#endif
	return(TRUE);
}

bool ProcHeapSize(void)
{
#if defined( __QNX__ )
	if (HintFormat(MK_QNX))
	{
		return(ProcQNXHeapSize());
	}
#endif
#ifdef _OS2
	if (HintFormat((exe_format)(MK_OS2 | MK_PE)))
	{
		return(ProcOS2HeapSize());
	}
#endif
#if defined( _QNXLOAD ) && !defined( __QNX__ )
	if (HintFormat((exe_format)MK_QNX))
	{
		return(ProcQNXHeapSize());
	}
#endif
	return(TRUE);
}

#ifndef APP
#if defined(_PHARLAP) || defined(_QNXLOAD) || defined(_OS2) || defined( _RAW)
bool ProcOffset(void)
{
	if (!GetLong(&FmtData.base)) return(FALSE);
	if (!(FmtData.type & (MK_PHAR_LAP | MK_QNX_FLAT | MK_RAW)))
	{
		ChkBase(64 * 1024);
	}
	else if (!(FmtData.type & (MK_OS2_FLAT | MK_PE)))
	{
		ChkBase(4 * 1024);
	}
	return(TRUE);
}
#endif
#endif

#ifdef _INT_DEBUG
bool ProcXDbg(void)
{
	char        value[7];

	if (GetToken(SEP_EQUALS, TOK_INCLUDE_DOT))
	{
		if (Token.len > 6)
		{
			return(FALSE);
		}
		else
		{
			memcpy(value, Token.this1, Token.len);
			value[Token.len] = '\0';
			Debug = strtoul(value, NULL, 0);
			//FIXME DEBUG((DBG_BASE, "debugging info type = %x", Debug));
		}
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

bool ProcIntDbg(void)
{
	LinkState |= INTERNAL_DEBUG;
	return(TRUE);
}
#endif
