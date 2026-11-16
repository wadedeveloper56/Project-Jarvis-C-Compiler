#include "pch.h"
#include "debug.h"
#include "ExpressionTree.h"
#include "ProgramData.h"
#include "GlobalVars.h"
#include "StructOrUnionSpecifier.h"

using namespace std;
using namespace WadeSpace;

#ifndef _DEBUG
#define debug_printf (void)
#endif

#ifdef _DEBUG
int debug_printf(char* msg)
{
	cout << msg << endl;
	return 0;
}
#endif
