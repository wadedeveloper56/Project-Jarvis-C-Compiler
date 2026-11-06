#pragma once
/*
#define NIL_HANDLE      ((f_handle)-1)
#if defined( __NT__ )
#include <stdio.h>
#define STDIN_HANDLE        (stdin->_handle)
#define STDOUT_HANDLE       (stdout->_handle)
#define STDERR_HANDLE       (stderr->_handle)
#else
#define STDIN_HANDLE    ((f_handle)0)
#define STDOUT_HANDLE   ((f_handle)1)
#define STDERR_HANDLE   ((f_handle)2)
#define STDAUX_HANDLE   ((f_handle)3)
#define STDPRN_HANDLE   ((f_handle)4)
#endif
*/
#define LSEEK_START    0                 
#define LSEEK_CURRENT  1                
#define LSEEK_END      2                

#define MAX_OPEN_FILES 12             
#define MAX_FILE        2 + 63 + 8 + 1 + 3 + 1

#define EXTRA_NAME_DIR  2
#define EXTRA_OBJ_FILE  3
#define EXTRA_LIB_FILE  4
#define EXTRA_RES_FILE  5

extern char     NLSeq[];

