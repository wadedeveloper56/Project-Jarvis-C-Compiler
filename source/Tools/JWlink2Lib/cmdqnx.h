#pragma once 


extern bool     ProcLongLived( void );
extern bool     ProcQNXPrivilege( void );
extern bool     ProcLinearRelocs( void );
extern bool     ProcQNX( void );

extern bool     ProcQNXExecuteonly( void );
extern bool     ProcQNXExecuteread( void );
extern bool     ProcQNXReadOnly( void );
extern bool     ProcQNXReadWrite( void );

extern bool     ProcQNXClass( void );

extern bool     ProcQNXFlat( void );

extern bool     ProcQNXSegment( void );
extern bool     ProcQNXNoRelocs( void );
extern bool     ProcQNXHeapSize( void );

extern void     SetQNXFmt( void );
extern void     FreeQNXFmt( void );
extern void     CmdQNXFini( void );
