#include "pch.h"
#include "lsymtab.h"
#include "structs.h"
#include "globals.h"
#include "util.h"
#include "objpass1.h"
#include "loadpe.h"
#include "loados2.h"
#include "impexp.h"

symbol* RefISymbol(char* name)
{
	return(SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), name, strlen(name)));
}

//FIXME: These functions are not implemented yet. They are just placeholders for now.
symbol* SymOp(sym_flags op, char* symname, unsigned length)
{
	return nullptr;
}
