#include "pch.h"
#include "globals.h"

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

ParseEntry Directives[] = 
{
    "File",         &ProcFiles,         MK_ALL, CF_HAVE_FILES,
    "MODFile",      &ProcModFiles,      MK_ALL, (CommandFlag)0,
    "Library",      &ProcLibrary,       MK_ALL, (CommandFlag)0,
    "Name",         &ProcName,          MK_ALL, (CommandFlag)0,
    "OPtion",       &ProcOptions,       MK_ALL, (CommandFlag)0,
    "Debug",        &ProcDebug,         MK_ALL, (CommandFlag)0,
    "SYStem",       &ProcSystem,        MK_ALL, (CommandFlag)0,
    "LIBPath",      &ProcLibPath,       MK_ALL, (CommandFlag)0,
    "LIBFile",      &ProcLibFile,       MK_ALL, (CommandFlag)CF_HAVE_FILES,
    "Path",         &ProcPath,          MK_ALL, (CommandFlag)0,
    "FORMat",       &ProcFormat,        MK_ALL, (CommandFlag)0,
    "MODTrace",     &ProcModTrace,      MK_ALL, (CommandFlag)0,
    "SYMTrace",     &ProcSymTrace,      MK_ALL, (CommandFlag)CF_AFTER_INC,
    "Alias",        &ProcAlias,         MK_ALL, (CommandFlag)CF_AFTER_INC,
    "REFerence",    &ProcReference,     MK_ALL, (CommandFlag)CF_AFTER_INC,
    "DISAble",      &ProcDisable,       MK_ALL, (CommandFlag)0,
    "SOrt",         &ProcSort,          MK_ALL, (CommandFlag)0,
    "LANGuage",     &ProcLanguage,      MK_ALL, (CommandFlag)0,
    "STARTLink",    &ProcStartLink,     MK_ALL, (CommandFlag)0,
    "OPTLIB",       &ProcOptLib,        MK_ALL, (CommandFlag)0,
    "ORDer",        &ProcOrder,         MK_ALL, (CommandFlag)0,
    "OUTput",       &ProcOutput,        MK_ALL, (CommandFlag)0,
#ifdef _OS2
    "RESource",     &ProcResource,      MK_PE, (CommandFlag)0,
    "COMmit",       &ProcCommit,        MK_PE, (CommandFlag)0,
    "ANONymousexport",&ProcAnonExport,  (ExeFormat)(MK_OS2 | MK_PE), (CommandFlag)CF_AFTER_INC,
#endif
#if defined( _NOVELL ) || defined( _OS2 )
    "IMPort",       &ProcImport,        (ExeFormat)(MK_NOVELL | MK_ELF | MK_OS2 | MK_PE), (CommandFlag)CF_AFTER_INC,
    "EXPort",       &ProcExport,        (ExeFormat)(MK_NOVELL | MK_ELF | MK_OS2 | MK_PE | MK_WIN_VXD), (CommandFlag)CF_AFTER_INC,
#endif
#if defined( _OS2 ) || defined( _QNXLOAD )
    "SEGment",      &ProcSegment,       (ExeFormat)(MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD), (CommandFlag)0,
#endif
#ifdef _EXE
    "OVerlay",      &ProcOverlay,       MK_OVERLAYS, (CommandFlag)0,
    "Begin",        &ProcBegin,         MK_OVERLAYS, (CommandFlag)0,
    "FIXedlib",     &ProcFixedLib,      MK_OVERLAYS, (CommandFlag)0,
    "NOVector",     &ProcNoVector,      MK_OVERLAYS, (CommandFlag)CF_AFTER_INC,
    "VEctor",       &ProcVector,        MK_OVERLAYS, (CommandFlag)CF_AFTER_INC,
    "FORCEVEctor",  &ProcForceVector,   MK_OVERLAYS, (CommandFlag)CF_AFTER_INC,
#endif
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      &ProcRuntime,       (ExeFormat)(MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), (CommandFlag)0,
#endif
#ifdef _NOVELL
    "MODUle",       &ProcModule,        (ExeFormat)(MK_NOVELL | MK_ELF), (CommandFlag)0,
#endif
#ifdef _DOS16M
    "MEMory",       &ProcMemory16M,     (ExeFormat)(MK_DOS16M), (CommandFlag)0,
    "TRansparent",  &ProcTransparent,   (ExeFormat)(MK_DOS16M), (CommandFlag)0,
#endif
#if defined( _OS2 ) || defined( _EXE ) || defined ( _QNXLOAD )
    "NEWsegment",   &ProcNewSegment,    (ExeFormat)(MK_OS2_16BIT | MK_DOS | MK_QNX), (CommandFlag)0,
#endif
#ifdef _INT_DEBUG
    "Xdbg",         &ProcXDbg,          MK_ALL, (CommandFlag)0,
    "INTDBG",       &ProcIntDbg,        MK_ALL, (CommandFlag)0,
#endif
    NULL
};
