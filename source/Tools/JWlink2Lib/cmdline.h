#pragma once 


extern bool             ProcImport( void );
extern bool             ProcExport( void );
extern bool             ProcSegment( void );
extern bool             ProcAlignment( void );
extern bool             ProcHeapSize( void );
extern bool             ProcOffset( void );
extern bool             ProcNoRelocs( void );

extern bool             ProcXDbg( void );
extern bool             ProcIntDbg( void );

extern void             InitCmdFile( void );
extern void             SetSegMask(void);
extern char             *GetNextLink( void );
extern void             DoCmdFile( char * );
extern void             Syntax( void );
extern void             FreePaths( void );
extern void             Burn( void );
extern void             Ignite( void );
extern void             SetFormat( void );
extern void             AddFmtLibPaths( void );
extern bool             HintFormat( exe_format );
extern void             DecideFormat( void );
extern void             FreeFormatStuff( void );
extern void             AddCommentLib( char *, unsigned, lib_priority );
extern void             ExecSystem( char * );
extern void             PruneSystemList( void );
extern void             BurnSystemList( void );
extern void             AddLibPaths( char *, unsigned, bool );
extern void             AddEnvPaths( char * );
extern file_list        *AddObjLib( char *, lib_priority );
