#pragma once 

using namespace std;

extern bool     ProcOverlay( void );
extern bool     ProcBegin( void );
extern bool     ProcFixedLib( void );
extern bool     ProcNoVector( void );
extern bool     ProcVector( void );
extern bool     ProcForceVector( void );
extern bool     ProcSmall( void );
extern bool     ProcDistribute( void );
extern bool     ProcDynamic( void );
extern bool     ProcStandard( void );
extern bool     ProcNoIndirect( void );
extern bool     ProcArea( void );
extern bool     ProcPadSections(void);
#if 1 /* JWlink */
extern bool     ProcKnowEAS(void);
#endif

extern bool     ProcDos( void );
extern bool     ProcCom( void );

extern bool     ProcSection( void );
extern bool     ProcEnd( void );
extern bool     ProcAutoSection( void );
extern bool     ProcInto( void );

extern void     SetDosFmt( void );
extern void     MakeNewSection( void );
extern void     CmdOvlFini( void );

extern ::byte     OvlLevel;
