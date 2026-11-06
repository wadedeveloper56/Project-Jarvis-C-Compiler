#include "pch.h"
#include "util.h"
#include "globals.h"

//bool ProcFiles(void){ return false;}
//bool ProcModFiles(void){ return false;}
//bool ProcLibrary(void){ return false;}
//bool ProcName(void){ return false;}
//bool ProcOptions(void){ return false;}
//bool ProcDebug(void){ return false;}
//bool ProcSystem(void){ return false;}
//bool ProcLibPath(void){ return false;}
//bool ProcLibFile(void){ return false;}
//bool ProcPath(void){ return false;}
//bool ProcFormat(void) { return false; }
//bool ProcModTrace(void){ return false;}
//bool ProcSymTrace(void){ return false;}
//bool ProcAlias(void){ return false;}
//bool ProcReference(void){ return false;}
//bool ProcDisable(void){ return false;}
//bool ProcSort(void){ return false;}
//bool ProcLanguage(void){ return false;}
//bool ProcStartLink(void){ return false;}
//bool ProcOptLib(void){ return false;}
//bool ProcOrder(void){ return false;}
//bool ProcOutput(void){ return false;}
//bool ProcResource(void){ return false;}
//bool ProcCommit(void){ return false;}
//bool ProcAnonExport(void){ return false;}
//bool ProcImport(void){ return false;}
//bool ProcExport(void){ return false;}
//bool ProcSegment(void){ return false;}
//bool ProcOverlay(void){ return false;}
//bool ProcBegin(void){ return false;}
//bool ProcFixedLib(void){ return false;}
//bool ProcNoVector(void){ return false;}
//bool ProcVector(void){ return false;}
//bool ProcForceVector(void){ return false;}
//bool ProcRuntime(void){ return false;}
//bool ProcModule(void){ return false;}
//bool ProcNewSegment(void){ return false;}

//bool ProcDistribute(void) { return false; }
//bool ProcOsVersion(void) { return false; }
//bool ProcStart(void) { return false; }
//bool ProcEnd(void) { return false; }
//bool ProcNoDosSeg(void) { return false; }
//bool ProcQNXExecuteread(void) { return false; }
//bool ProcImplib(void) { return false; }
//bool ProcQNXReadWrite(void) { return false; }
//bool ProcSelfRelative(void) { return false; }
//bool ProcOrdCopy(void) { return false; }
//bool ProcELFRNumber(void) { return false; }
//bool ProcModuleType4(void) { return false; }
//bool ProcELFRNetBSD(void) { return false; }
bool ProcRaw(void) { return false; }
//bool ProcExportsDBI(void) { return false; }
//bool ProcPharFlat(void) { return false; }
//bool ProcFuzzyExport(void) { return false; }
//bool ProcInit00(void) { return false; }
//bool ProcInitFF(void) { return false; }
//bool ProcNovell(void) { return false; }
//bool ProcHeapSize(void) { return false; }
//bool ProcNoUndefsOK(void) { return false; }
//bool ProcFixed(void) { return false; }
//bool ProcDiscardable(void) { return false; }
//bool ProcPrivate(void) { return false; }
//bool ProcHshift(void) { return false; }
//bool ProcNonPageable(void) { return false; }
//bool ProcAutoSection(void) { return false; }
//bool ProcReadOnly(void) { return false; }
//bool ProcPMFullscreen(void) { return false; }
//bool ProcEliminate(void) { return false; }
//bool ProcOrdSegSegAdr(void) { return false; }
//bool ProcObjAlign(void) { return false; }
//bool ProcNoName(void) { return false; }
//bool ProcMemory16M(void) { return false; }
//bool ProcCommitStack(void) { return false; }
//bool ProcQNX(void) { return false; }
//bool ProcMovable(void) { return false; }
//bool ProcInt10(void) { return false; }
//bool ProcLongLived(void) { return false; }
//bool ProcFarCalls(void) { return false; }
//bool ProcCheck(void) { return false; }
//bool ProcNonConforming(void) { return false; }
//bool ProcSynch(void) { return false; }
//bool ProcTStack(void) { return false; }
//bool ProcNIStack(void) { return false; }
//bool ProcBuffer(void) { return false; }
//bool ProcOrdNoEmit(void) { return false; }
//bool ProcOpResource(void) { return false; }
//bool ProcMiniBuf(void) { return false; }
//bool ProcELFDLL(void) { return false; }
//bool ProcFillchar(void) { return false; }
//bool ProcNovDBI(void) { return false; }
//bool ProcInvalid(void) { return false; }
//bool ProcModuleType7(void) { return false; }
//bool ProcQuiet(void) { return false; }
//bool ProcShowDead(void) { return false; }
//bool ProcRelocs(void) { return false; }
//bool ProcHX(void) { return false; }
//bool ProcVXD(void) { return false; }
//bool ProcLinearRelocs(void) { return false; }
//bool ProcPM(void) { return false; }
//bool ProcRWRelocCheck(void) { return false; }
//bool ProcOS2(void) { return false; }
//bool ProcSegCode(void) { return false; }
//bool ProcExpResident(void) { return false; }
//bool ProcUnpriv(void) { return false; }
//bool ProcSegType(void) { return false; }
//bool ProcModuleType8(void) { return false; }
//bool ProcCustom(void) { return false; }
//bool ProcSelStart(void) { return false; }
//bool ProcLoadoncall(void) { return false; }
//bool ProcDSK(void) { return false; }
//bool ProcRotate(void) { return false; }
//bool ProcXDCData(void) { return false; }
//bool ProcOutputOfs(void) { return false; }
//bool ProcIStkSize(void) { return false; }
//bool ProcExtended(void) { return false; }
//bool ProcDwarfDBI(void) { return false; }
//bool ProcImpFile(void) { return false; }
//bool ProcRex(void) { return false; }
//bool ProcPermanent(void) { return false; }
//bool ProcVersion(void) { return false; }
//bool ProcAlignment(void) { return false; }
//bool ProcCodeviewDBI(void) { return false; }
//bool ProcWritable(void) { return false; }
//bool ProcFlags(void) { return false; }
//bool ProcNovDBIExports(void) { return false; }
//bool ProcMultiple(void) { return false; }
//bool ProcQNXExecuteonly(void) { return false; }
//bool ProcMap(void) { return false; }
//bool ProcNonShared(void) { return false; }
//bool ProcOrdSegNoEmit(void) { return false; }
//bool ProcPackdata(void) { return false; }
//bool ProcOffset(void) { return false; }
bool ProcRawHEX(void) { return false; }
//bool ProcReentrant(void) { return false; }
//bool ProcNoRelocs(void) { return false; }
//bool ProcInitThread(void) { return false; }
//bool ProcType(void) { return false; }
//bool ProcNoStub(void) { return false; }
//bool ProcEndLink(void) { return false; }
//bool ProcExecuteonly(void) { return false; }
//bool ProcLX(void) { return false; }
//bool ProcExit(void) { return false; }
//bool ProcRedefsOK(void) { return false; }
//bool ProcQNXClass(void) { return false; }
//bool ProcNoFarCalls(void) { return false; }
//bool ProcPseudoPreemption(void) { return false; }
//bool ProcNoDefLibs(void) { return false; }
//bool ProcLocal(void) { return false; }
//bool ProcStandard(void) { return false; }
//bool ProcStatics(void) { return false; }
//bool ProcVirtDevice(void) { return false; }
//bool ProcModuleType9(void) { return false; }
//bool ProcMultiLoad(void) { return false; }
//bool ProcTermGlobal(void) { return false; }
//bool ProcOrdOfsAdr(void) { return false; }
//bool ProcProtMode(void) { return false; }
//bool ProcChinese(void) { return false; }
//bool ProcCVPack(void) { return false; }
//bool ProcForceExtended(void) { return false; }
//bool ProcOutputRaw(void) { return false; }
//bool ProcRunConsole(void) { return false; }
//bool ProcRunDosstyle(void) { return false; }
//bool ProcWindows(void) { return false; }
//bool ProcMemory(void) { return false; }
//bool ProcAll(void) { return false; }
//bool ProcCallBufs(void) { return false; }
//bool ProcExecutable(void) { return false; }
//bool ProcNewFiles(void) { return false; }
//bool ProcSmall(void) { return false; }
//bool ProcTryExtended(void) { return false; }
//bool ProcOverload(void) { return false; }
//bool ProcMinData(void) { return false; }
//bool ProcSegData(void) { return false; }
//bool ProcNonPermanent(void) { return false; }
//bool ProcKeyboard(void) { return false; }
//bool ProcPE(void) { return false; }
//bool ProcNoIndirect(void) { return false; }
//bool ProcNoCaseExact(void) { return false; }
//bool ProcSelectors(void) { return false; }
//bool ProcOutputStart(void) { return false; }
//bool ProcQNXReadOnly(void) { return false; }
//bool ProcInternalRelocs(void) { return false; }
//bool ProcModuleType6(void) { return false; }
//bool ProcVFRemoval(void) { return false; }
//bool ProcELFRLinux(void) { return false; }
//bool ProcRealBreak(void) { return false; }
//bool ProcGDTSize(void) { return false; }
//bool ProcInto(void) { return false; }
//bool ProcOrdClass(void) { return false; }
//bool ProcNoCache(void) { return false; }
//bool ProcStaticDriver(void) { return false; }
//bool ProcPharSegmented(void) { return false; }
//bool ProcNLMFlags(void) { return false; }
//bool ProcOSName(void) { return false; }
//bool ProcLE(void) { return false; }
//bool ProcCopyright(void) { return false; }
//bool ProcStub(void) { return false; }
//bool ProcArea(void) { return false; }
//bool ProcOSDomain(void) { return false; }
//bool ProcNoExtension(void) { return false; }
//bool ProcTData(void) { return false; }
//bool ProcTermThread(void) { return false; }
//bool ProcNameLen(void) { return false; }
//bool ProcConforming(void) { return false; }
//bool ProcOrdSegAdr(void) { return false; }
//bool ProcJapanese(void) { return false; }
//bool ProcPhysDevice(void) { return false; }
//bool ProcShared(void) { return false; }
//bool ProcSingle(void) { return false; }
//bool ProcSysDelete(void) { return false; }
//bool ProcPackcode(void) { return false; }
//bool ProcNoRedefs(void) { return false; }
//bool ProcTNT(void) { return false; }
//bool ProcContiguous(void) { return false; }
//bool ProcIopl(void) { return false; }
//bool ProcOutputHex(void) { return false; }
//bool ProcDPMI(void) { return false; }
//bool ProcInitInstance(void) { return false; }
//bool ProcHelp(void) { return false; }
//bool ProcLinkVersion(void) { return false; }
//bool ProcPreload(void) { return false; }
//bool ProcTryLow(void) { return false; }
//bool ProcMessages(void) { return false; }
//bool ProcLAN(void) { return false; }
//bool ProcELFRFBSD(void) { return false; }
//bool ProcQNXPrivilege(void) { return false; }
//bool ProcMangledNames(void) { return false; }
//bool ProcGlobal(void) { return false; }
//bool ProcModuleType5(void) { return false; }
//bool ProcNoIopl(void) { return false; }
//bool ProcCommitHeap(void) { return false; }
//bool ProcCache(void) { return false; }
//bool ProcQNXFlat(void) { return false; }
//bool ProcInitGlobal(void) { return false; }
//bool ProcNAM(void) { return false; }
//bool ProcCase(void) { return false; }
//bool ProcOrdSeg(void) { return false; }
//bool ProcThreadName(void) { return false; }
//bool ProcStack(void) { return false; }
//bool ProcNonDiscardable(void) { return false; }
//bool ProcDynamic(void) { return false; }
//bool ProcUndefsOK(void) { return false; }
//bool ProcOutputHshift(void) { return false; }
//bool ProcNone(void) { return false; }
//bool ProcScreenName(void) { return false; }
//bool ProcPMCompatible(void) { return false; }
//bool ProcNoLargeAddressAware(void) { return false; }
//bool ProcKnowEAS(void) { return false; }
//bool ProcLargeAddressAware(void) { return false; }
//bool ProcSysEnd(void) { return false; }
//bool ProcSymFile(void) { return false; }
//bool ProcVerbose(void) { return false; }
//bool ProcIncremental(void) { return false; }
bool ProcRawBIN(void) { return false; }
//bool ProcReadWrite(void) { return false; }
//bool ProcAuto(void) { return false; }
//bool ProcForceLow(void) { return false; }
//bool ProcNXCompat(void) { return false; }
//bool ProcELFRSVR4(void) { return false; }
//bool ProcDescription(void) { return false; }
//bool ProcLine(void) { return false; }
//bool ProcNovDBIReferenced(void) { return false; }
//bool ProcOS2DLL(void) { return false; }
//bool ProcDosSeg(void) { return false; }
//bool ProcAlphabetical(void) { return false; }
//bool ProcExtraSections(void) { return false; }
//bool ProcModName(void) { return false; }
//bool ProcMixed1632(void) { return false; }
//bool ProcPageable(void) { return false; }
//bool ProcTermInstance(void) { return false; }
//bool ProcMaxiBuf(void) { return false; }
//bool ProcArtificial(void) { return false; }
//bool ProcTransparent(void) { return false; }
//bool ProcPriv(void) { return false; }
//bool ProcOldLibrary(void) { return false; }
//bool ProcExportAll(void) { return false; }
//bool ProcMaxReal(void) { return false; }
//bool ProcKorean(void) { return false; }
//bool ProcAutoUnload(void) { return false; }
//bool ProcRunOS2(void) { return false; }
//bool ProcPadSections(void) { return false; }
//bool ProcExecuteread(void) { return false; }
//bool ProcWatcomDBI(void) { return false; }
//bool ProcOS2Dynamic(void) { return false; }
//bool ProcDataSize(void) { return false; }
//bool ProcDynamicDriver(void) { return false; }
//bool ProcOrdSegOfsAdr(void) { return false; }
//bool ProcFont(void) { return false; }
//bool ProcNoStdCall(void) { return false; }
//bool ProcToggleRelocsFlag(void) { return false; }
//bool ProcMaxErrors(void) { return false; }
//bool ProcSection(void) { return false; }
//bool ProcSysBegin(void) { return false; }
//bool ProcOS2Class(void) { return false; }
//bool ProcRunPosix(void) { return false; }
//bool ProcCom(void) { return false; }
//bool ProcMinReal(void) { return false; }
//bool ProcChecksum(void) { return false; }
//bool ProcRunWindows(void) { return false; }
//bool ProcELFRSolrs(void) { return false; }
//bool ProcRunNative(void) { return false; }
//bool ProcSharelib(void) { return false; }

//bool ProcNone(void) { return false; }
//bool ProcCallBufs(void) { return false; }
