#include "pch.h"
#include "command.h"
#include "cmdall.h"
#include "cmdline.h"
#include "cmdraw.h"
#include "structs.h"
#include "globals.h"
#include "util.h"
#include "objpass1.h"
#include "loadpe.h"
#include "loados2.h"
#include "impexp.h"
#include "lsymtab.h"
#include "exepe.h"
#include "ntio.h"

bool ProcRaw( void )
{
    FmtData.base = 0;                             
    LinkState |= MAKE_RELOCS + FMT_DECIDED;       
    ProcOne( RawOptions, SEP_NO, TRUE );
    return( TRUE );
}

bool ProcRawBIN( void )
{
    Extension = E_BIN;
    FmtData.raw_hex_output = 0;
    if( FmtData.osname == NULL )
        FmtData.osname = "RAW Binary Image";
    return( TRUE );
}

bool ProcRawHEX( void )
{
    Extension = E_HEX;
    FmtData.raw_hex_output = 1;
    if( FmtData.osname == NULL )
        FmtData.osname = "RAW Intel Hex Image";
    return( TRUE );
}
