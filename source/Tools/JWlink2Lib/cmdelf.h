#pragma once 


extern bool     ProcELF( void );
extern bool     ProcELFDLL( void );
extern bool     ProcELFImport( void );
extern bool     ProcELFAlignment( void );

extern void     SetELFFmt( void );
extern void     FreeELFFmt( void );
extern void     SetELFImportSymbol( symbol * );

extern bool     ProcExtraSections( void );
extern bool     ProcExportAll( void );

extern bool     ProcELFNoRelocs( void );

extern bool     ProcELFRNumber( void );
extern bool     ProcELFRSVR4( void );
extern bool     ProcELFRNetBSD( void );
extern bool     ProcELFRLinux( void );
extern bool     ProcELFRSolrs( void );
extern bool     ProcELFRFBSD( void );
