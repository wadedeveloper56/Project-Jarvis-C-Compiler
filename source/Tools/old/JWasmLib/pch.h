#pragma once
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <limits.h>
#include <stdarg.h>
#include <setjmp.h>
#if defined(__UNIX__) && ( defined(__GNUC__) || defined(__TINYC__) )
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( _M_IX86 ) && defined(__WATCOMC__)
#include <i86.h>
#endif
#include <malloc.h>

#define DO_AND_EQ(t,a,b,c) a=(t)(a & (c))
#define DO_OR_EQ(t,a,b,c) a=(t)(a | (c))

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4311)
#pragma warning(disable:4146)
#pragma warning(disable:4113)
#pragma warning(disable:5286)
#pragma warning(disable:4312)
#pragma warning(disable:4477)
#pragma warning(disable:5287)
