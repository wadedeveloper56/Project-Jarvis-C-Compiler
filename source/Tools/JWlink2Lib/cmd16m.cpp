#include "pch.h"
#include "structs.h"
#include "util.h"
#include "globals.h"
#include "cmd16m.h"
#include "exe16m.h"
#include "load16m.h"

#ifdef _DOS16M
bool ProcMemory16M( void )
{
    return( ProcOne( Strategies, SEP_NO, FALSE ) );
}

bool ProcTryExtended( void )
{
    FmtData.u.d16m.strategy = MPreferExt;
    return( TRUE );
}

bool ProcTryLow( void )
{
    FmtData.u.d16m.strategy = MPreferLow;
    return( TRUE );
}

bool ProcForceExtended( void )
{
    FmtData.u.d16m.strategy = MForceExt;
    return( TRUE );
}

bool ProcForceLow( void )
{
    FmtData.u.d16m.strategy = MForceLow;
    return( TRUE );
}

bool ProcTransparent( void )
{
    if( FmtData.u.d16m.flags & TRANS_SPECD ) {
        //LnkMsg( LOC+LINE+WRN+MSG_OPTION_MULTIPLY_DEFD, "s", "transparent" );
        return( TRUE );
    } else {
        return( ProcOne( TransTypes, SEP_NO, FALSE ) );
    }
}

bool ProcTStack( void )
{
    FmtData.u.d16m.flags |= TRANS_STACK;
    return( TRUE );
}

bool ProcTData( void )
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        //LnkMsg( LOC+LINE+WRN+MSG_TRANS_RELOCS_NEEDED, NULL );
    }
    FmtData.u.d16m.flags |= TRANS_DATA;
    LinkState |= MAKE_RELOCS;
    return( TRUE );
}

bool ProcKeyboard( void )
{
    FmtData.u.d16m.options |= OPT_KEYBOARD;
    return( TRUE );
}

bool ProcOverload( void )
{
    FmtData.u.d16m.options |= OPT_OVERLOAD;
    return( TRUE );
}

bool ProcInt10( void )
{
    FmtData.u.d16m.options |= OPT_INT10;
    return( TRUE );
}

bool ProcInit00( void )
{
    FmtData.u.d16m.options |= OPT_INIT00;
    return( TRUE );
}

bool ProcInitFF( void )
{
    FmtData.u.d16m.options |= OPT_INITFF;
    return( TRUE );
}

bool ProcRotate( void )
{
    FmtData.u.d16m.options |= OPT_ROTATE;
    return( TRUE );
}

bool ProcSelectors( void )
{
    FmtData.u.d16m.options |= OPT_AUTO;
    return( TRUE );
}

bool ProcAuto( void )
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        //LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    }
    FmtData.u.d16m.options |= OPT_AUTO;
    LinkState |= MAKE_RELOCS;
    return( TRUE );
}

bool ProcBuffer( void )
{
    uint32_t value;

    if( !GetLong( &value ) )
        return( FALSE );
    if( value < 1024 || value > 32768 ) {
        //LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "buffer" );
    } else {
        FmtData.u.d16m.buffer = value;
    }
    return( TRUE );
}

bool ProcGDTSize( void )
{
    uint32_t value;

    if( !GetLong( &value ) )
        return( FALSE );
    if( (value % 8) != 0 ) {
        //LnkMsg( LOC+LINE+WRN+MSG_NOT_MULTIPLE_OF_8, "s", "gdtsize" );
        value &= -8;
    }
    if( value > 65536 ) {
        //LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "gdtsize" );
    } else {
        FmtData.u.d16m.gdtsize = --value;
    }
    return( TRUE );
}

bool ProcRelocs( void )
{
    if( FmtData.u.d16m.flags & FORCE_NO_RELOCS ) {
        //LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    }
    LinkState |= MAKE_RELOCS;
    return( TRUE );
}

bool Proc16MNoRelocs( void )
{
    if( LinkState & MAKE_RELOCS ) {
        //LnkMsg( LOC+LINE+WRN+MSG_BOTH_RELOC_OPTIONS, NULL );
    } else {
        FmtData.u.d16m.flags |= FORCE_NO_RELOCS;
    }
    return( TRUE );
}

bool ProcSelStart( void )
{
    uint32_t value;

    if( !GetLong( &value ) )
        return( FALSE );
    if( (value % 8) != 0 ) {
        //LnkMsg( LOC+LINE+WRN+MSG_NOT_MULTIPLE_OF_8, "s", "selstart" );
        value &= -8;
    }
    if( value > 65536 || value < D16M_USER_SEL ) {
        //LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "selstart" );
    } else {
        FmtData.u.d16m.selstart = value;
    }
    return( TRUE );
}

bool ProcExtended( void )
{
    uint32_t value;

    if( !GetLong( &value ) )
        return( FALSE );
    value >>= 10;      // value should be in K.
    if( value > 65535 ) {
        //LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "extended" );
    } else {
        FmtData.u.d16m.extended = value;
    }
    return( TRUE );
}

bool ProcDataSize( void )
{
    uint32_t value;

    if( !GetLong( &value ) )
        return( FALSE );
    if( value > 65536 ) {
        //LnkMsg( LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "datasize" );
    } else {
        FmtData.u.d16m.datasize = (value + 15) >> 4;
        FmtData.u.d16m.flags |= DATASIZE_SPECD;
    }
    return( TRUE );
}

void SetD16MFmt( void )
{
    LinkState &= ~MAKE_RELOCS;              // assume none being produced.
    Extension = E_PROTECT;
    FmtData.u.d16m.options = 0;
    FmtData.u.d16m.flags = 0;
    FmtData.u.d16m.strategy = MNoStrategy;
    FmtData.u.d16m.buffer = 0;
    FmtData.u.d16m.gdtsize = 0xFFFF;        // 64 K - 1.
    FmtData.u.d16m.selstart = D16M_USER_SEL;
    FmtData.u.d16m.extended = 0x7FFF;
    FmtData.u.d16m.datasize = 0x1000;
    FmtData.u.d16m.stub = NULL;
}

extern void FreeD16MFmt( void )
{
    free( FmtData.u.d16m.stub );
}

bool Proc16M( void )
{
    return( TRUE );
}
#endif
