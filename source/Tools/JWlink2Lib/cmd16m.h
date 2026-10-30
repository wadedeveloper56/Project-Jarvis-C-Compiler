#pragma once 

extern bool     ProcMemory16M( void );
extern bool     ProcTryExtended( void );
extern bool     ProcTryLow( void );
extern bool     ProcForceExtended( void );
extern bool     ProcForceLow( void );
extern bool     ProcTransparent( void );
extern bool     ProcTStack( void );
extern bool     ProcTData( void );
extern bool     ProcKeyboard( void );
extern bool     ProcOverload( void );
extern bool     ProcInt10( void );
extern bool     ProcInit00( void );
extern bool     ProcInitFF( void );
extern bool     ProcRotate( void );
extern bool     ProcSelectors( void );
extern bool     ProcAuto( void );
extern bool     ProcBuffer( void );
extern bool     ProcGDTSize( void );
extern bool     ProcRelocs( void );
extern bool     Proc16MNoRelocs( void );
extern bool     ProcSelStart( void );
extern bool     ProcExtended( void );
extern bool     ProcDataSize( void );
extern void     SetD16MFmt( void );
extern void     FreeD16MFmt( void );
extern bool     Proc16M( void );
