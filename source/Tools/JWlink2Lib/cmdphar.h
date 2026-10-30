#pragma once 


extern bool     ProcPackExp( void );
extern bool     ProcMinData( void );
extern bool     ProcMaxData( void );
extern bool     ProcPharLap( void );

extern bool     ProcMinReal( void );
extern bool     ProcMaxReal( void );
extern bool     ProcRealBreak( void );
extern bool     ProcCallBufs( void );
extern bool     ProcMiniBuf( void );
extern bool     ProcMaxiBuf( void );
extern bool     ProcNIStack( void );
extern bool     ProcIStkSize( void );
extern bool     ProcUnpriv( void );
extern bool     ProcPriv( void );
extern bool     ProcFlags( void );

extern bool     ProcPharFlat( void );
extern bool     ProcRex( void );
extern bool     ProcPharSegmented( void );

extern void     SetPharFmt( void );
extern void     FreePharFmt( void );
