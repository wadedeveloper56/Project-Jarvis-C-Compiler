/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Command parsing tables.
*
****************************************************************************/


#include "pch.h"
#include "linkstd.h"
#include "command.h"
#include "cmdall.h"
#include "cmdos2.h"
#include "cmdqnx.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "cmdphar.h"
#include "cmddos.h"
#include "cmdraw.h"
#include "cmd16m.h"
#include "cmdline.h"


parse_entry     PosDbgMods[] = {
    "LInes",        &ProcLine,          MK_ALL, (commandflag)0,
    "Types",        &ProcType,          MK_ALL, (commandflag)0,
    "LOcals",       &ProcLocal,         MK_ALL, (commandflag)0,
//  "STatic",       &ProcDBIStatic,     MK_ALL, (commandflag)0,
    "All",          &ProcAll,           MK_ALL, (commandflag)0,
#ifdef _NOVELL
    "ONLyexports",  &ProcExportsDBI,    MK_NOVELL, (commandflag)0,
#endif
    NULL
};

 parse_entry     DbgMods[] = {
    "Watcom",       &ProcWatcomDBI,     MK_ALL, (commandflag)0,
    "Dwarf",        &ProcDwarfDBI,      MK_ALL, (commandflag)0,
    "Codeview",     &ProcCodeviewDBI,   MK_ALL, (commandflag)0,
#ifdef _NOVELL
    "Novell",       &ProcNovDBI,        MK_NOVELL, (commandflag)0,
#endif
    NULL
};

parse_entry     SysBeginOptions[] = {
    "Begin",         &ProcSysBegin,     MK_ALL, (commandflag)0,
    NULL
};

parse_entry     SysDeleteOptions[] = {
    "DELete",        &ProcSysDelete,    MK_ALL, (commandflag)0,
    NULL
};

parse_entry     SysEndOptions[] = {
    "End",          &ProcSysEnd,        MK_ALL, (commandflag)0,
    NULL
};

parse_entry     SortOptions[] = {
    "ALPhabetical", &ProcAlphabetical,  MK_ALL, (commandflag)0,
    "GLobal",       &ProcGlobal,        MK_ALL, (commandflag)0,
    NULL
};

parse_entry     Directives[] = {
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
    //"ANONymousexport",&ProcAnonExport,  MK_OS2, CF_AFTER_INC,
    "ANONymousexport",&ProcAnonExport,  (exe_format)(MK_OS2 | MK_PE), (commandflag)CF_AFTER_INC,
#endif
#if defined( _NOVELL ) || defined( _OS2 )
    "IMPort",       &ProcImport,        (exe_format)(MK_NOVELL | MK_ELF | MK_OS2 | MK_PE ), (commandflag)CF_AFTER_INC,
    "EXPort",       &ProcExport,        (exe_format)(MK_NOVELL | MK_ELF | MK_OS2 | MK_PE | MK_WIN_VXD ), (commandflag)CF_AFTER_INC,
#endif
#if defined( _OS2 ) || defined( _QNXLOAD )
    "SEGment",      &ProcSegment,       (exe_format)(MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD ), (commandflag)0,
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


parse_entry     MainOptions[] = {
    "Map",          &ProcMap,           MK_ALL, (commandflag)0,
    "STack",        &ProcStack,         MK_ALL, (commandflag)0,
    "NODefaultlibs",&ProcNoDefLibs,     MK_ALL, (commandflag)CF_NO_DEF_LIBS,
    "Quiet",        &ProcQuiet,         MK_ALL, (commandflag)0,
    "Dosseg",       &ProcDosSeg,        MK_ALL, (commandflag)0,
    "NODOSseg",     &ProcNoDosSeg,      MK_ALL, (commandflag)0, /* v1.9beta15 added */
    "Caseexact",    &ProcCase,          MK_ALL, (commandflag)0,
    "Verbose",      &ProcVerbose,       MK_ALL, (commandflag)0,
    "Undefsok",     &ProcUndefsOK,      MK_ALL, (commandflag)0,
    "NOUndefsok",   &ProcNoUndefsOK,    MK_ALL, (commandflag)0,
    "NAMELen",      &ProcNameLen,       MK_ALL, (commandflag)0,
    "SYMFile",      &ProcSymFile,       MK_ALL, (commandflag)CF_SEPARATE_SYM,
    "OSName",       &ProcOSName,        MK_ALL, (commandflag)0,
    "ELiminate",    &ProcEliminate,     MK_ALL, (commandflag)0,
    "FARcalls",     &ProcFarCalls,      MK_ALL, (commandflag)0,
    "MAXErrors",    &ProcMaxErrors,     MK_ALL, (commandflag)0,
    "NOCASEexact",  &ProcNoCaseExact,   MK_ALL, (commandflag)0,
    "NOCAChe",      &ProcNoCache,       MK_ALL, (commandflag)0,
    "NOEXTension",  &ProcNoExtension,   MK_ALL, (commandflag)0,
    "NOFARcalls",   &ProcNoFarCalls,    MK_ALL, (commandflag)0,
    "CAChe",        &ProcCache,         MK_ALL, (commandflag)0,
    "MANGlednames", &ProcMangledNames,  MK_ALL, (commandflag)0,
    "OBJAlign",     &ProcObjAlign,      MK_ALL, (commandflag)0,
    "RESource",     &ProcOpResource,    MK_ALL, (commandflag)0,
    "STATics",      &ProcStatics,       MK_ALL, (commandflag)0,
    "START",        &ProcStart,         MK_ALL, (commandflag)0,
    "ARTificial",   &ProcArtificial,    MK_ALL, (commandflag)0,
    "SHOwdead",     &ProcShowDead,      MK_ALL, (commandflag)0,
    "VFRemoval",    &ProcVFRemoval,     MK_ALL, (commandflag)0,
    "REDefsok",     &ProcRedefsOK,      MK_ALL, (commandflag)0,
    "NOREDefsok",   &ProcNoRedefs,      MK_ALL, (commandflag)0,
    "CVPack",       &ProcCVPack,        MK_ALL, (commandflag)0,
    "INCremental",  &ProcIncremental,   MK_ALL, (commandflag)0,
    "SMall",        &ProcSmall,         MK_OVERLAYS, (commandflag)0,
    "DIStribute",   &ProcDistribute,    MK_OVERLAYS, (commandflag)0,
    "DYNamic",      &ProcDynamic,       MK_OVERLAYS, (commandflag)0,
    "STANdard",     &ProcStandard,      MK_OVERLAYS, (commandflag)0,
    "NOIndirect",   &ProcNoIndirect,    MK_OVERLAYS, (commandflag)0,
    "ARea",         &ProcArea,          MK_OVERLAYS, (commandflag)0,
    "PADSections",  &ProcPadSections,   MK_OVERLAYS, (commandflag)0,
#if 1  /* JWLink */
    "KNOWEAS",      &ProcKnowEAS,       (exe_format)MK_DOS, (commandflag)0,
    "LARGEaddressaware",&ProcLargeAddressAware, (exe_format)MK_PE, (commandflag)0,
    "NOLARGEaddressaware",&ProcNoLargeAddressAware, (exe_format)MK_PE, (commandflag)0,
#endif
    "PACKCode",     &ProcPackcode,      (exe_format)(MK_OS2_16BIT | MK_DOS | MK_QNX | MK_DOS16M), (commandflag)0,
    "PACKData",     &ProcPackdata,      (exe_format)(MK_OS2_16BIT | MK_DOS | MK_QNX | MK_DOS16M), (commandflag)0,
    "Alignment",    &ProcAlignment,     (exe_format)(MK_OS2_16BIT | MK_OS2_LX | MK_PE | MK_ELF), (commandflag)0,
    "STUB",         &ProcStub,          (exe_format)(MK_OS2 | MK_PE | MK_WIN_VXD | MK_PHAR_LAP | MK_DOS16M), (commandflag)0,
    "NOSTUB",       &ProcNoStub,        (exe_format)(MK_OS2 | MK_PE | MK_WIN_VXD), (commandflag)0,
    "ONEautodata",  &ProcSingle,        (exe_format)MK_OS2, (commandflag)0,
    "MANYautodata", &ProcMultiple,      (exe_format)MK_OS2, (commandflag)0,
    "NOAutodata",   &ProcNone,          (exe_format)MK_OS2_16BIT, (commandflag)0,
    "OLDlibrary",   &ProcOldLibrary,    (exe_format)(MK_OS2 | MK_PE), (commandflag)0,
    "MODName",      &ProcModName,       (exe_format)(MK_OS2 | MK_PE | MK_WIN_VXD), (commandflag)0,
    "NEWFiles",     &ProcNewFiles,      (exe_format)MK_ONLY_OS2_16, (commandflag)0,
    "PROTmode",     &ProcProtMode,      (exe_format)MK_OS2_16BIT, (commandflag)0,
    "DEscription",  &ProcDescription,   (exe_format)(MK_OS2 | MK_PE | MK_WIN_VXD), (commandflag)0,
    "NOSTDCall",    &ProcNoStdCall,     (exe_format)MK_PE, (commandflag)0,
    "FUZzyexport",  &ProcFuzzyExport,   (exe_format)MK_PE, (commandflag)0,
    "RWReloccheck", &ProcRWRelocCheck,  (exe_format)MK_WIN_NE, (commandflag)0,
    "SELFrelative", &ProcSelfRelative,  (exe_format)MK_OS2_LX, (commandflag)0,
    "INTernalrelocs",&ProcInternalRelocs,(exe_format)MK_OS2_LX, (commandflag)0,
    "TOGglerelocsflag",&ProcToggleRelocsFlag,(exe_format)MK_OS2_LX, (commandflag)0,
    "Heapsize",     &ProcHeapSize,      (exe_format)(MK_OS2 | MK_QNX | MK_PE), (commandflag)0,
//  "PACKExp",      &ProcPackExp,       (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "MINData",      &ProcMinData,       (exe_format)MK_PHAR_LAP, (commandflag)0,
    "MAXData",      &ProcMaxData,       (exe_format)MK_PHAR_LAP, (commandflag)0,
    "OFFset",       &ProcOffset,        (exe_format)(MK_PHAR_FLAT|MK_OS2_FLAT|MK_PE|MK_QNX_FLAT|MK_ELF|MK_RAW), (commandflag)0,
    "SCReenname",   &ProcScreenName,    (exe_format)MK_NOVELL, (commandflag)0,
    "CHeck",        &ProcCheck,         (exe_format)MK_NOVELL, (commandflag)0,
    "MULTILoad",    &ProcMultiLoad,     (exe_format)MK_NOVELL, (commandflag)0,
    "AUTOUNload",   &ProcAutoUnload,    (exe_format)MK_NOVELL, (commandflag)0,
    "REentrant",    &ProcReentrant,     (exe_format)MK_NOVELL, (commandflag)0,
    "SYnchronize",  &ProcSynch,         (exe_format)MK_NOVELL, (commandflag)0,
    "CUSTom",       &ProcCustom,        (exe_format)MK_NOVELL, (commandflag)0,
    "EXit",         &ProcExit,          (exe_format)MK_NOVELL, (commandflag)0,
    "THReadname",   &ProcThreadName,    (exe_format)MK_NOVELL, (commandflag)0,
    "PSeudopreemption", &ProcPseudoPreemption,      (exe_format)MK_NOVELL, (commandflag)0,
    "COPYRight",    &ProcCopyright,     (exe_format)MK_NOVELL, (commandflag)0,
    "MESsages",     &ProcMessages,      (exe_format)MK_NOVELL, (commandflag)0,
    "HElp",         &ProcHelp,          (exe_format)MK_NOVELL, (commandflag)0,
    "XDCdata",      &ProcXDCData,       (exe_format)MK_NOVELL, (commandflag)0,
    "SHArelib",     &ProcSharelib,      (exe_format)MK_NOVELL, (commandflag)0,
    "OSDomain",     &ProcOSDomain,      (exe_format)MK_NOVELL, (commandflag)0,
    "NLMFlags",     &ProcNLMFlags,      (exe_format)MK_NOVELL, (commandflag)0,
    "VERSion",      &ProcVersion,       (exe_format)(MK_NOVELL|MK_OS2_FLAT|MK_PE|MK_WIN_NE), (commandflag)0,
    "IMPLib",       &ProcImplib,        (exe_format)(MK_NOVELL|MK_OS2|MK_PE), (commandflag)0,
    "IMPFile",      &ProcImpFile,       (exe_format)(MK_NOVELL|MK_OS2|MK_PE), (commandflag)0,
#ifdef _DOS16M
    "BUFfer",       &ProcBuffer,        (exe_format)MK_DOS16M, (commandflag)0,
    "GDTsize",      &ProcGDTSize,       (exe_format)MK_DOS16M, (commandflag)0,
    "RELocs",       &ProcRelocs,        (exe_format)MK_DOS16M, (commandflag)0,
    "SELstart",     &ProcSelStart,      (exe_format)MK_DOS16M, (commandflag)0,
    "DATASize",     &ProcDataSize,      (exe_format)MK_DOS16M, (commandflag)0,
    "EXTended",     &ProcExtended,      (exe_format)MK_DOS16M, (commandflag)0,
#endif
    "NORelocs",     &ProcNoRelocs,      (exe_format)(MK_QNX | MK_DOS16M | MK_PE | MK_ELF), (commandflag)0,
    "LOnglived",    &ProcLongLived,     (exe_format)MK_QNX, (commandflag)0,
    "PRIVilege",    &ProcQNXPrivilege,  (exe_format)MK_QNX, (commandflag)0,
    "LInearrelocs", &ProcLinearRelocs,  (exe_format)MK_QNX, (commandflag)0,
    "EXTRASections",&ProcExtraSections, (exe_format)MK_ELF, (commandflag)0,
    "EXPORTAll",    &ProcExportAll,     (exe_format)MK_ELF, (commandflag)0,
    "LINKVersion",  &ProcLinkVersion,   (exe_format)MK_PE,  (commandflag)0,
    "OSVersion",    &ProcOsVersion,     (exe_format)MK_PE,  (commandflag)0,
    "CHECKSUM",     &ProcChecksum,      (exe_format)MK_PE,  (commandflag)0,
    "NXCompat",     &ProcNXCompat,      (exe_format)MK_PE,  (commandflag)0,
    "HSHIFT",       &ProcHshift,        (exe_format)(MK_DOS | MK_ALLOW_16),  (commandflag)0,
    "FILLchar",     &ProcFillchar,      (exe_format)MK_ALL,  (commandflag)0,
    "MIXed1632",    &ProcMixed1632,     (exe_format)MK_OS2_FLAT, (commandflag)0,
    NULL
};

/* these directives are the only ones that are harmless to run after the files
 * have been processed in pass 1 */

parse_entry     SysDirectives[] = {
    "Library",      &ProcLibrary,       (exe_format)MK_ALL, (commandflag)0,
    "Name",         &ProcName,          (exe_format)MK_ALL, (commandflag)0,
    "OPtion",       &ProcOptions,       (exe_format)MK_ALL, (commandflag)0,
    "LIBPath",      &ProcLibPath,       (exe_format)MK_ALL, (commandflag)0,
    "FORMat",       &ProcFormat,        (exe_format)MK_ALL, (commandflag)0,
    "DISAble",      &ProcDisable,       (exe_format)MK_ALL, (commandflag)0,
    "SOrt",         &ProcSort,          (exe_format)MK_ALL, (commandflag)0,
    "ORDer",        &ProcOrder,         (exe_format)MK_ALL, (commandflag)0,
    "OUTput",       &ProcOutput,        (exe_format)MK_ALL, (commandflag)0,
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      &ProcRuntime,       (exe_format)(MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), (commandflag)0,
#endif
#if defined( _OS2 ) || defined( _QNXLOAD )
    "SEGment",      &ProcSegment,       (exe_format)(MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD ), (commandflag)0,
#endif
#ifdef _DOS16M
    "MEMory",       &ProcMemory16M,     (exe_format)MK_DOS16M, (commandflag)0,
    "TRansparent",  &ProcTransparent,   (exe_format)MK_DOS16M, (commandflag)0,
#endif
    NULL
};

parse_entry    Models[] = {
    "Dos",          &ProcDos,           (exe_format)MK_DOS, (commandflag)0,
#ifdef _OS2
    "OS2",          &ProcOS2,           (exe_format)MK_ONLY_OS2, (commandflag)0,
    "WINdows",      &ProcWindows,       (exe_format)(MK_WIN_NE|MK_PE|MK_WIN_VXD), (commandflag)0,  /* JWlink */
#endif
#ifdef _PHARLAP
    "PHARlap",      &ProcPharLap,       (exe_format)MK_PHAR_LAP, (commandflag)0,
#endif
#ifdef _NOVELL
    "NOVell",       &ProcNovell,        (exe_format)MK_NOVELL, (commandflag)0,
#endif
#ifdef _QNXLOAD
    "QNX",          &ProcQNX,           (exe_format)MK_QNX, (commandflag)0,
#endif
#ifdef _DOS16M
    "DOS16M",       &Proc16M,           (exe_format)MK_DOS16M, (commandflag)0,
#endif
#ifdef _ELF
    "ELF",          &ProcELF,           (exe_format)MK_ELF, (commandflag)0,
#endif
#ifdef _RAW
    "Raw",          &ProcRaw,           (exe_format)MK_RAW, (commandflag)0,
#endif
    NULL
};

parse_entry    Languages[] = {
    "JApanese",     &ProcJapanese,      (exe_format)MK_ALL, (commandflag)0,
    "CHinese",      &ProcChinese,       (exe_format)MK_ALL, (commandflag)0,
    "KOrean",       &ProcKorean,        (exe_format)MK_ALL, (commandflag)0,
    NULL
};

parse_entry      EndLinkOpt[] = {
    "ENDLink",      &ProcEndLink,       (exe_format)MK_ALL, (commandflag)0,
};

parse_entry  RunOptions[] = {
#ifdef _DOS16M
    "KEYboard",     &ProcKeyboard,      (exe_format)MK_DOS16M, (commandflag)0,
    "OVERload",     &ProcOverload,      (exe_format)MK_DOS16M, (commandflag)0,
    "INIT00",       &ProcInit00,        (exe_format)MK_DOS16M, (commandflag)0,
    "INITFF",       &ProcInitFF,        (exe_format)MK_DOS16M, (commandflag)0,
    "ROTate",       &ProcRotate,        (exe_format)MK_DOS16M, (commandflag)0,
    "AUTO",         &ProcAuto,          (exe_format)MK_DOS16M, (commandflag)0,
    "SELectors",    &ProcSelectors,     (exe_format)MK_DOS16M, (commandflag)0,
    "INT10",        &ProcInt10,         (exe_format)MK_DOS16M, (commandflag)0,
#endif
#ifdef _PHARLAP
    "MINReal",      &ProcMinReal,       (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "MAXReal",      &ProcMaxReal,       (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "REALBreak",    &ProcRealBreak,     (exe_format)MK_PHAR_FLAT, (commandflag)CF_HAVE_REALBREAK,
    "CALLBufs",     &ProcCallBufs,      (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "MINIBuf",      &ProcMiniBuf,       (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "MAXIBuf",      &ProcMaxiBuf,       (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "NISTack",      &ProcNIStack,       (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "ISTKsize",     &ProcIStkSize,      (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "UNPRIVileged", &ProcUnpriv,        (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "PRIVileged",   &ProcPriv,          (exe_format)MK_PHAR_FLAT, (commandflag)0,
/* WARNING: do not document the following directive -- for internal use only */
    "FLAGs",        &ProcFlags,         (exe_format)MK_PHAR_FLAT, (commandflag)0,
#endif
#ifdef _OS2
    "NATive",       &ProcRunNative,     (exe_format)MK_PE, (commandflag)0,
    "WINdows",      &ProcRunWindows,    (exe_format)MK_PE, (commandflag)0,
    "CONsole",      &ProcRunConsole,    (exe_format)MK_PE, (commandflag)0,
    "POSix",        &ProcRunPosix,      (exe_format)MK_PE, (commandflag)0,
    "OS2",          &ProcRunOS2,        (exe_format)MK_PE, (commandflag)0,
    "DOSstyle",     &ProcRunDosstyle,   (exe_format)MK_PE, (commandflag)0,
#endif
#ifdef _ELF
    "ABIver",       &ProcELFRNumber,    (exe_format)MK_ELF, (commandflag)0,
    "SVR4",         &ProcELFRSVR4,      (exe_format)MK_ELF, (commandflag)0,
    "NETbsd",       &ProcELFRNetBSD,    (exe_format)MK_ELF, (commandflag)0,
    "LINux",        &ProcELFRLinux,     (exe_format)MK_ELF, (commandflag)0,
    "FREebsd",      &ProcELFRFBSD,      (exe_format)MK_ELF, (commandflag)0,
    "SOLaris",      &ProcELFRSolrs,     (exe_format)MK_ELF, (commandflag)0,
#endif
    NULL
};

#ifdef _DOS16M
/* parse tables used in cmd16m.c */

extern bool     ProcTryExtended( void );
extern bool     ProcTryLow( void );
extern bool     ProcForceExtended( void );
extern bool     ProcForceLow( void );

parse_entry  Strategies[] = {
    "TRYExtended",  &ProcTryExtended,   (exe_format)MK_DOS16M, (commandflag)0,
    "TRYLow",       &ProcTryLow,        (exe_format)MK_DOS16M, (commandflag)0,
    "FORCEExtended",&ProcForceExtended, (exe_format)MK_DOS16M, (commandflag)0,
    "FORCELow",     &ProcForceLow,      (exe_format)MK_DOS16M, (commandflag)0,
    NULL
};

extern bool     ProcTStack( void );
extern bool     ProcTData( void );

parse_entry  TransTypes[] = {
    "STack",        &ProcTStack,        (exe_format)MK_DOS16M, (commandflag)0,
    "DAta",         &ProcTData,         (exe_format)MK_DOS16M, (commandflag)0,
    NULL
};
#endif

#ifdef _QNXLOAD
/* parse tables used in CMDQNX.C */

parse_entry QNXSegModel[] = {
    "EXECUTEOnly",  &ProcQNXExecuteonly,(exe_format)MK_QNX, (commandflag)0,
    "EXECUTERead",  &ProcQNXExecuteread,(exe_format)MK_QNX, (commandflag)0,
    "READOnly",     &ProcQNXReadOnly,   (exe_format)MK_QNX, (commandflag)0,
    "READWrite",    &ProcQNXReadWrite,  (exe_format)MK_QNX, (commandflag)0,
    NULL
};

parse_entry QNXSegDesc[] = {
    "Class",        &ProcQNXClass,      (exe_format)MK_QNX, (commandflag)0,
    NULL
};

parse_entry QNXFormats[] = {
    "FLat",         &ProcQNXFlat,       (exe_format)MK_QNX_FLAT, (commandflag)0,
    NULL
};
#endif

#ifdef _PHARLAP
/* parse tables used in CMDPHAR.C */

parse_entry  PharModels[] = {
    "EXTended",     &ProcPharFlat,      (exe_format)MK_PHAR_FLAT, (commandflag)0,
    "REX",          &ProcRex,           (exe_format)MK_PHAR_REX, (commandflag)0,
    "SEGmented",    &ProcPharSegmented, (exe_format)MK_PHAR_MULTISEG, (commandflag)0,
    NULL
};
#endif

#ifdef _NOVELL
/* parse tables used in CMDNOV.C */

parse_entry     NovModels[] = {
    "NLM",          &ProcNLM,           (exe_format)MK_NOVELL, (commandflag)0,    /* 0 */
    "LAN",          &ProcLAN,           (exe_format)MK_NOVELL, (commandflag)0,    /* 1 */
    "DSK",          &ProcDSK,           (exe_format)MK_NOVELL, (commandflag)0,    /* 2 */
    "NAM",          &ProcNAM,           (exe_format)MK_NOVELL, (commandflag)0,    /* 3 */
    "0",            &ProcNLM,           (exe_format)MK_NOVELL, (commandflag)0,    /* 0 again */
    "1",            &ProcLAN,           (exe_format)MK_NOVELL, (commandflag)0,    /* etc */
    "2",            &ProcDSK,           (exe_format)MK_NOVELL, (commandflag)0,
    "3",            &ProcNAM,           (exe_format)MK_NOVELL, (commandflag)0,
    "4",            &ProcModuleType4,   (exe_format)MK_NOVELL, (commandflag)0,
    "5",            &ProcModuleType5,   (exe_format)MK_NOVELL, (commandflag)0,
    "6",            &ProcModuleType6,   (exe_format)MK_NOVELL, (commandflag)0,
    "7",            &ProcModuleType7,   (exe_format)MK_NOVELL, (commandflag)0,
    "8",            &ProcModuleType8,   (exe_format)MK_NOVELL, (commandflag)0,
    "9",            &ProcModuleType9,   (exe_format)MK_NOVELL, (commandflag)0,
#if 0
    /* NLM types 10 through 12 are currently reserved */
    "10",           &ProcModuleType10,  (exe_format)MK_NOVELL, (commandflag)0,
    "11",           &ProcModuleType11,  (exe_format)MK_NOVELL, (commandflag)0,
    "12",           &ProcModuleType12,  (exe_format)MK_NOVELL, (commandflag)0,
#endif
    NULL
};

parse_entry      NovDBIOptions[] = {
    "ONLyexports",  &ProcNovDBIExports, (exe_format)MK_NOVELL, (commandflag)0,
    "REFerenced",   &ProcNovDBIReferenced,(exe_format)MK_NOVELL, (commandflag)0,
    NULL
};
#endif

/* parse tables used in CMDDOS.C */

parse_entry     Sections[] = {
    "Section",      &ProcSection,       (exe_format)MK_OVERLAYS, (commandflag)0,
    "AUTOSection",  &ProcAutoSection,   (exe_format)MK_OVERLAYS, (commandflag)0,
    "End",          &ProcEnd,           (exe_format)MK_OVERLAYS, (commandflag)0,
    NULL
};

parse_entry     SectOptions[] = {
    "INto",         &ProcInto,          (exe_format)MK_OVERLAYS, (commandflag)0,
    NULL
};

parse_entry     DosOptions[] = {
    "COM",          &ProcCom,           (exe_format)MK_COM, (commandflag)0,
    NULL
};

#ifdef _OS2
/* parse tables used in CMDOS2.C */

parse_entry  SubFormats[] = {
    "DLl",          &ProcOS2DLL,        (exe_format)(MK_OS2 | MK_PE), (commandflag)0,
    "FLat",         &ProcLX,            (exe_format)MK_OS2_LX, (commandflag)0,
    "LE",           &ProcLE,            (exe_format)MK_OS2_LE, (commandflag)0,
    "LX",           &ProcLX,            (exe_format)MK_OS2_LX, (commandflag)0,
    "NT",           &ProcPE,            (exe_format)MK_PE, (commandflag)0,
    "PE",           &ProcPE,            (exe_format)MK_PE, (commandflag)0,
    "VXD",          &ProcVXD,           (exe_format)MK_WIN_VXD, (commandflag)0,
    NULL
};

parse_entry OS2FormatKeywords[] = {
    "PM",           &ProcPM,            (exe_format)MK_ONLY_OS2, (commandflag)0,
    "PMCompatible", &ProcPMCompatible,  (exe_format)MK_ONLY_OS2, (commandflag)0,
    "FULLscreen",   &ProcPMFullscreen,  (exe_format)MK_ONLY_OS2, (commandflag)0,
    "PHYSdevice",   &ProcPhysDevice,    (exe_format)(MK_OS2_LE | MK_OS2_LX), (commandflag)0,
    "VIRTdevice",   &ProcVirtDevice,    (exe_format)(MK_OS2_LE | MK_OS2_LX), (commandflag)0,
    NULL
};

parse_entry WindowsFormatKeywords[] = {
    "MEMory",       &ProcMemory,        (exe_format)MK_WIN_NE, (commandflag)0,
    "FOnt",         &ProcFont,          (exe_format)MK_WIN_NE, (commandflag)0,
    "DPMI",         &ProcDPMI,          (exe_format)MK_WIN_NE, (commandflag)0,
    NULL
};

parse_entry NTFormatKeywords[] = {
    "TNT",          &ProcTNT,           (exe_format)MK_PE, (commandflag)0,
    "HX",           &ProcHX,            (exe_format)MK_PE, (commandflag)0,
    NULL
};

parse_entry VXDFormatKeywords[] = {
    "DYNamic",      &ProcDynamicDriver, (exe_format)MK_WIN_VXD, (commandflag)0,
    "STATic",       &ProcStaticDriver,  (exe_format)MK_WIN_VXD, (commandflag)0,
    NULL
};

parse_entry Init_Keywords[] = {
    "INITGlobal",   &ProcInitGlobal,    (exe_format)(MK_OS2|MK_PE), (commandflag)0,
    "INITInstance", &ProcInitInstance,  (exe_format)(MK_OS2|MK_PE), (commandflag)0,
    "INITThread",   &ProcInitThread,    (exe_format)MK_PE, (commandflag)0,
    NULL
};

parse_entry Term_Keywords[] = {
    "TERMGlobal",   &ProcTermGlobal,    (exe_format)(MK_OS2_LE | MK_OS2_LX | MK_PE), (commandflag)0,
    "TERMInstance", &ProcTermInstance,  (exe_format)(MK_OS2_LE | MK_OS2_LX | MK_PE), (commandflag)0,
    "TERMThread",   &ProcTermThread,    (exe_format)MK_PE, (commandflag)0,
    NULL
};

parse_entry Exp_Keywords[] = {
    "RESident",     &ProcExpResident,   (exe_format)MK_OS2, (commandflag)0,
    "PRIVATE",      &ProcPrivate,       (exe_format)(MK_OS2|MK_PE), (commandflag)0,
    /* jwlink */
    "NONAME",       &ProcNoName,        (exe_format)(MK_OS2|MK_PE), (commandflag)0,
    NULL
};

parse_entry SegDesc[] = {
    "Class",        &ProcOS2Class,      (exe_format)(MK_OS2|MK_PE|MK_WIN_VXD), (commandflag)0,
    "TYpe",         &ProcSegType,       (exe_format)(MK_OS2|MK_PE|MK_WIN_VXD), (commandflag)0,
    NULL
};

parse_entry SegTypeDesc[] = {
    "CODE",         &ProcSegCode,       (exe_format)(MK_OS2|MK_PE|MK_WIN_VXD), (commandflag)0,
    "DATA",         &ProcSegData,       (exe_format)(MK_OS2|MK_PE|MK_WIN_VXD), (commandflag)0,
    NULL
};

parse_entry SegModel[] = {
    "PReload",      &ProcPreload,       (exe_format)(MK_OS2|MK_WIN_VXD), (commandflag)0,
    "LOadoncall",   &ProcLoadoncall,    (exe_format)(MK_OS2|MK_WIN_VXD), (commandflag)0,
    "Iopl",         &ProcIopl,          (exe_format)(MK_ONLY_OS2|MK_WIN_VXD), (commandflag)0,
    "NOIopl",       &ProcNoIopl,        (exe_format)(MK_ONLY_OS2|MK_WIN_VXD), (commandflag)0,
    "EXECUTEOnly",  &ProcExecuteonly,   (exe_format)MK_OS2, (commandflag)0,
    "EXECUTERead",  &ProcExecuteread,   (exe_format)MK_OS2, (commandflag)0,
    "SHared",       &ProcShared,        (exe_format)(MK_OS2|MK_PE|MK_WIN_VXD), (commandflag)0,
    "NONShared",    &ProcNonShared,     (exe_format)(MK_OS2|MK_PE|MK_WIN_VXD), (commandflag)0,
    "READOnly",     &ProcReadOnly,      (exe_format)(MK_OS2|MK_PE), (commandflag)0,    /* jwlink: added MK_PE */
    "READWrite",    &ProcReadWrite,     (exe_format)MK_OS2, (commandflag)0,
    "WRITAble",     &ProcWritable,      (exe_format)MK_PE, (commandflag)0,     /* jwlink */
    "EXECUTAble",   &ProcExecutable,    (exe_format)MK_PE, (commandflag)0,     /* jwlink */
    "CONforming",   &ProcConforming,    (exe_format)(MK_ONLY_OS2|MK_WIN_VXD), (commandflag)0,
    "NONConforming",&ProcNonConforming, (exe_format)(MK_ONLY_OS2|MK_WIN_VXD), (commandflag)0,
    "MOVeable",     &ProcMovable,       (exe_format)MK_OS2_16BIT, (commandflag)0,
    "FIXed",        &ProcFixed,         (exe_format)MK_WIN_NE, (commandflag)0,
    "DIScardable",  &ProcDiscardable,   (exe_format)(MK_WIN_NE|MK_WIN_VXD), (commandflag)0,
    "NONDiscardable",&ProcNonDiscardable,(exe_format)(MK_WIN_VXD), (commandflag)0,
    "INValid",      &ProcInvalid,       (exe_format)(MK_OS2_LE|MK_OS2_LX), (commandflag)0,
    "RESident",     &ProcPermanent,     (exe_format)(MK_OS2_LE|MK_OS2_LX|MK_WIN_VXD), (commandflag)0,
    "CONTiguous",   &ProcContiguous,    (exe_format)(MK_OS2_LE|MK_OS2_LX), (commandflag)0,
    "DYNamic",      &ProcOS2Dynamic,    (exe_format)(MK_OS2_LE|MK_OS2_LX), (commandflag)0,
    "NONPERManent", &ProcNonPermanent,  (exe_format)(MK_OS2_LE|MK_OS2_LX), (commandflag)0,
    "PERManent",    &ProcPermanent,     (exe_format)(MK_OS2_LE|MK_OS2_LX), (commandflag)0,
    "PAGEable",     &ProcPageable,      (exe_format)MK_PE, (commandflag)0,
    "NONPageable",  &ProcNonPageable,   (exe_format)MK_PE, (commandflag)0,
    NULL
};

parse_entry CommitKeywords[] = {
    "STack",        &ProcCommitStack,   (exe_format)MK_PE, (commandflag)0,
    "Heap",         &ProcCommitHeap,    (exe_format)MK_PE, (commandflag)0,
    NULL
};
#endif

#ifdef _ELF
parse_entry ELFFormatKeywords[] = {
    "DLl",          &ProcELFDLL,        (exe_format)MK_ELF, (commandflag)0,
    NULL
};
#endif

#ifdef _RAW
parse_entry RawOptions[] = {
    "BIN",          &ProcRawBIN,        (exe_format)MK_RAW, (commandflag)0,
    "HEX",          &ProcRawHEX,        (exe_format)MK_RAW, (commandflag)0,
    NULL };
#endif

parse_entry OrderOpts[] = {
    "CLName",       &ProcOrdClass,      (exe_format)MK_ALL, (commandflag)0,
    NULL
};

parse_entry OrderClassOpts[] = {
    "SEGAddr",      &ProcOrdSegAdr,     (exe_format)MK_ALL, (commandflag)0,
    "OFFset",       &ProcOrdOfsAdr,     (exe_format)MK_ALL, (commandflag)0,
    "COpy",         &ProcOrdCopy,       (exe_format)MK_ALL, (commandflag)0,
    "NOEmit",       &ProcOrdNoEmit,     (exe_format)MK_ALL, (commandflag)0,
    "SEGMent",      &ProcOrdSeg,        (exe_format)MK_ALL, (commandflag)0,
    NULL
};

parse_entry OrderSegOpts[] = {
    "SEGAddr",      &ProcOrdSegSegAdr,  (exe_format)MK_ALL, (commandflag)0,
    "OFFset",       &ProcOrdSegOfsAdr,  (exe_format)MK_ALL, (commandflag)0,
    "NOEmit",       &ProcOrdSegNoEmit,  (exe_format)MK_ALL, (commandflag)0,
    NULL
};

parse_entry OutputOpts[] = {
    "RAW",          &ProcOutputRaw,     (exe_format)MK_ALL, (commandflag)0,
    "HEX",          &ProcOutputHex,     (exe_format)MK_ALL, (commandflag)0,
    "OFFset",       &ProcOutputOfs,     (exe_format)MK_ALL, (commandflag)0,
    "HSHIFT",       &ProcOutputHshift,  (exe_format)MK_ALL, (commandflag)0,
    "STartrec",     &ProcOutputStart,   (exe_format)MK_ALL, (commandflag)0,
    NULL
};

