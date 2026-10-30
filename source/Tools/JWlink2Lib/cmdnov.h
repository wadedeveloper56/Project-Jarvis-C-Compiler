#pragma once 


extern bool     ProcNovDBI( void );
extern bool     ProcExportsDBI( void );
extern bool     ProcModule( void );
extern bool     ProcScreenName( void );
extern bool     ProcCheck( void );
extern bool     ProcMultiLoad( void );
extern bool     ProcAutoUnload( void );
extern bool     ProcReentrant( void );
extern bool     ProcSynch( void );
extern bool     ProcCustom( void );
extern bool     ProcExit( void );
extern bool     ProcThreadName( void );
extern bool     ProcPseudoPreemption( void );
extern bool     ProcNLMFlags( void );
extern bool     ProcCopyright( void );
extern bool     ProcMessages( void );
extern bool     ProcHelp( void );
extern bool     ProcXDCData( void );
extern bool     ProcSharelib( void );
extern bool     ProcOSDomain( void );
extern bool     ProcNovell( void );

extern bool     ProcNLM( void );
extern bool     ProcLAN( void );
extern bool     ProcDSK( void );
extern bool     ProcNAM( void );
extern bool     ProcModuleType4( void );
extern bool     ProcModuleType5( void );
extern bool     ProcModuleType6( void );
extern bool     ProcModuleType7( void );
extern bool     ProcModuleType8( void );
extern bool     ProcModuleType9( void );
#if 0
/* 10 through 12 are reserved */
extern bool     ProcModuleType10( void );
extern bool     ProcModuleType11( void );
extern bool     ProcModuleType12( void );
#endif

extern bool     ProcNovDBIExports( void );
extern bool     ProcNovDBIReferenced( void );
extern bool     ProcNovImport( void );
extern bool     ProcNovExport( void );
extern void     SetNovImportSymbol( symbol * );
extern void     SetNovFmt( void );
extern void     FreeNovFmt( void );
extern void     CmdNovFini( void );
