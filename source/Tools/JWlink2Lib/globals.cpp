#include "pch.h"
#include "structs.h"

using namespace std;

extern bool ProcFiles(void);
extern bool ProcModFiles(void);
extern bool ProcLibrary(void);
extern bool ProcName(void);
extern bool ProcOptions(void);
extern bool ProcDebug(void);
extern bool ProcSystem(void);
extern bool ProcLibPath(void);
extern bool ProcLibFile(void);
extern bool ProcPath(void);
extern bool ProcFormat(void);
extern bool ProcModTrace(void);
extern bool ProcSymTrace(void);
extern bool ProcAlias(void);
extern bool ProcReference(void);
extern bool ProcDisable(void);
extern bool ProcSort(void);
extern bool ProcLanguage(void);
extern bool ProcStartLink(void);
extern bool ProcOptLib(void);
extern bool ProcOrder(void);
extern bool ProcOutput(void);
extern bool ProcResource(void);
extern bool ProcCommit(void);
extern bool ProcAnonExport(void);
extern bool ProcImport(void);
extern bool ProcExport(void);
extern bool ProcSegment(void);
extern bool ProcOverlay(void);
extern bool ProcBegin(void);
extern bool ProcFixedLib(void);
extern bool ProcNoVector(void);
extern bool ProcVector(void);
extern bool ProcForceVector(void);
extern bool ProcRuntime(void);
extern bool ProcModule(void);
extern bool ProcMemory16M(void);
extern bool ProcTransparent(void);
extern bool ProcNewSegment(void);

parse_entry Directives[] = 
{
    "File",         &ProcFiles,         MK_ALL, CF_HAVE_FILES,
    "MODFile",      &ProcModFiles,      MK_ALL, (commandflag)0,
    "Library",      &ProcLibrary,       MK_ALL, (commandflag)0,
    "Name",         &ProcName,          MK_ALL, (commandflag)0,
    "OPtion",       &ProcOptions,       MK_ALL, (commandflag)0,
    "Debug",        &ProcDebug,         MK_ALL, (commandflag)0,
    "SYStem",       &ProcSystem,        MK_ALL, (commandflag)0,
    "LIBPath",      &ProcLibPath,       MK_ALL, (commandflag)0,
    "LIBFile",      &ProcLibFile,       MK_ALL, (commandflag)CF_HAVE_FILES,
    "Path",         &ProcPath,          MK_ALL, (commandflag)0,
    "FORMat",       &ProcFormat,        MK_ALL, (commandflag)0,
    "MODTrace",     &ProcModTrace,      MK_ALL, (commandflag)0,
    "SYMTrace",     &ProcSymTrace,      MK_ALL, (commandflag)CF_AFTER_INC,
    "Alias",        &ProcAlias,         MK_ALL, (commandflag)CF_AFTER_INC,
    "REFerence",    &ProcReference,     MK_ALL, (commandflag)CF_AFTER_INC,
    "DISAble",      &ProcDisable,       MK_ALL, (commandflag)0,
    "SOrt",         &ProcSort,          MK_ALL, (commandflag)0,
    "LANGuage",     &ProcLanguage,      MK_ALL, (commandflag)0,
    "STARTLink",    &ProcStartLink,     MK_ALL, (commandflag)0,
    "OPTLIB",       &ProcOptLib,        MK_ALL, (commandflag)0,
    "ORDer",        &ProcOrder,         MK_ALL, (commandflag)0,
    "OUTput",       &ProcOutput,        MK_ALL, (commandflag)0,
#ifdef _OS2
    "RESource",     &ProcResource,      MK_PE, (commandflag)0,
    "COMmit",       &ProcCommit,        MK_PE, (commandflag)0,
    "ANONymousexport",&ProcAnonExport,  (exe_format)(MK_OS2 | MK_PE), (commandflag)CF_AFTER_INC,
#endif
#if defined( _NOVELL ) || defined( _OS2 )
    "IMPort",       &ProcImport,        (exe_format)(MK_NOVELL | MK_ELF | MK_OS2 | MK_PE), (commandflag)CF_AFTER_INC,
    "EXPort",       &ProcExport,        (exe_format)(MK_NOVELL | MK_ELF | MK_OS2 | MK_PE | MK_WIN_VXD), (commandflag)CF_AFTER_INC,
#endif
#if defined( _OS2 ) || defined( _QNXLOAD )
    "SEGment",      &ProcSegment,       (exe_format)(MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD), (commandflag)0,
#endif
#ifdef _EXE
    "OVerlay",      &ProcOverlay,       MK_OVERLAYS, (commandflag)0,
    "Begin",        &ProcBegin,         MK_OVERLAYS, (commandflag)0,
    "FIXedlib",     &ProcFixedLib,      MK_OVERLAYS, (commandflag)0,
    "NOVector",     &ProcNoVector,      MK_OVERLAYS, (commandflag)CF_AFTER_INC,
    "VEctor",       &ProcVector,        MK_OVERLAYS, (commandflag)CF_AFTER_INC,
    "FORCEVEctor",  &ProcForceVector,   MK_OVERLAYS, (commandflag)CF_AFTER_INC,
#endif
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      &ProcRuntime,       (exe_format)(MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), (commandflag)0,
#endif
#ifdef _NOVELL
    "MODUle",       &ProcModule,        (exe_format)(MK_NOVELL | MK_ELF), (commandflag)0,
#endif
#ifdef _DOS16M
    "MEMory",       &ProcMemory16M,     (exe_format)(MK_DOS16M), (commandflag)0,
    "TRansparent",  &ProcTransparent,   (exe_format)(MK_DOS16M), (commandflag)0,
#endif
#if defined( _OS2 ) || defined( _EXE ) || defined ( _QNXLOAD )
    "NEWsegment",   &ProcNewSegment,    (exe_format)(MK_OS2_16BIT | MK_DOS | MK_QNX), (commandflag)0,
#endif
#ifdef _INT_DEBUG
    "Xdbg",         &ProcXDbg,          MK_ALL, (commandflag)0,
    "INTDBG",       &ProcIntDbg,        MK_ALL, (commandflag)0,
#endif
    NULL
};

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
cmdfilelist* CmdFile = NULL;
mapflag        MapFlags;       /* flags dealing with the map file */
char* MapFName;       /* name of map file                     */
char* SymFileName;
group_entry* DBIGroups;
linkflag       LinkFlags;      /* flags to indicate presence of parms  */
