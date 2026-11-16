#include "pch.h"
#include "globals.h"
#include "msgtext.h"
#include "cmdline.h"
#include "input.h"   

#if defined(__UNIX__) || defined(__CYGWIN__) || defined(__DJGPP__)

#define WILDCARDS 0
#define CATCHBREAK 0

#else

#define WILDCARDS 1
#ifdef __POCC__
#define CATCHBREAK 0
#else
#define CATCHBREAK 1
#endif

#endif

#if WILDCARDS

 #ifdef __UNIX__
  #include <unistd.h>
 #else
  #include <io.h>
 #endif
#endif

#ifdef TRMEM
void tm_Init( void );
void tm_Fini( void );
#endif

void genfailure( int signo )
{
#if CATCHBREAK
    if (signo != SIGBREAK)
#else
    if (signo != SIGTERM)
#endif
        EmitError( GENERAL_FAILURE );
    close_files();
    exit( EXIT_FAILURE );
}

int AssemblerMain(int argc, char** argv);

int main(int argc, char** argv)
{
	return AssemblerMain(argc, argv);
}

/*
int main( int argc, char **argv )
{
    char    *pEnv;
    int     numArgs = 0;
    int     numFiles = 0;
    int     rc = 0;
#if WILDCARDS
    long    fh;
    const char *pfn;
    int     dirsize;
    struct  _finddata_t finfo;
    char    fname[FILENAME_MAX];
#endif

#if 0             
    int i;
    for ( i = 1; i < argc; i++ ) {
        printf("argv[%u]=>%s<\n", i, argv[i] );
    }
#endif

#ifdef TRMEM
    tm_Init();
#endif

    pEnv = getenv( "JWASM" );
    if ( pEnv == NULL )
        pEnv = "";
    argv[0] = pEnv;

#ifndef DEBUG_OUT
    signal(SIGSEGV, genfailure);
#endif

#if CATCHBREAK
    signal(SIGBREAK, genfailure);
#else
    signal(SIGTERM, genfailure);
#endif

    while ( ParseCmdline( (const char **)&argv[1], &numArgs ) ) {
        numFiles++;
        write_logo();
#if WILDCARDS
        if ((fh = _findfirst( Options.names[ASM], &finfo )) == -1 ) {
            DebugMsg(("main: _findfirst(%s) failed\n", Options.names[ASM] ));
            EmitErr( CANNOT_OPEN_FILE, Options.names[ASM], ErrnoStr() );
            break;
        }
        pfn = GetFNamePart( Options.names[ASM] );
        dirsize = pfn - Options.names[ASM];
        memcpy( fname, Options.names[ASM], dirsize );
        do {
            strcpy( &fname[dirsize], finfo.name );
            DebugMsg(("main: fname=%s\n", fname ));
            rc = AssembleModule( fname );      
        } while ( ( _findnext( fh, &finfo ) != -1 ) );
        _findclose( fh );
#else
        rc = AssembleModule( Options.names[ASM] );
#endif
    };
    CmdlineFini();
    if ( numArgs == 0 ) {
        write_logo();
        printf( "%s", MsgGetEx( MSG_USAGE ) );
    } else if ( numFiles == 0 )
        EmitError( NO_FILENAME_SPECIFIED );

#ifdef TRMEM
    tm_Fini();
#endif

    DebugMsg(("main: exit, return code=%u\n", 1 - rc ));
    return( 1 - rc );      
}
*/