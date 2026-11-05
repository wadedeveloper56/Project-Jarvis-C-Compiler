#pragma once
#include "structs.h"
symbol* SymOp(sym_flags op, char* symname, unsigned length);
symbol* RefISymbol(char* name);
bool SetCurrentPrefix(const char* pszPrefix, unsigned nLen);
symbol* SymOpNWPfx(sym_flags op, char* name, unsigned length, char* prefix, unsigned prefixLen);