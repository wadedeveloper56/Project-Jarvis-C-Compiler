#include "pch.h"
#include "structs.h"

using namespace std;

int _argc;
char** _argv;
void* SpawnStack;
char* ArgSave;
file_defext Extension;
file_list** CurrFList;
tok Token;
commandflag CmdFlags;
char* Name;
sysblock* SysBlocks;
sysblock* LinkCommands;
path_entry* Path;           /* path for object files                */
struct fmt_data FmtData;       /* format specific data */
section* Root;           /* pointer to root information          */
section* CurrSect;       /* current section for file processing  */
::byte DBIFlag;
int OvlNum;         /* current overlay number               */
stateflag      LinkState;      /* flags to indicate linker state */

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

void GetCmdLine(char* buff)
{}

void NewCommandSource(char* name, char* buff, method how)
{}

void DoCmdFile(char* fname)
{
    ResetCmdFile();
    if (fname == NULL || *fname == '\0')
    {
        fname = (char*)malloc(10 * 1024);
        GetCmdLine(fname);
        NewCommandSource(NULL, fname, COMMANDLINE);
    }
    else
    {
        NewCommandSource(NULL, fname, ENVIRONMENT);
    }
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
