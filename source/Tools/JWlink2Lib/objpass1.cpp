#include "pch.h"
#include "globals.h"
#include "start.h"
#include "ntio.h"
#include "util.h"
#include "ring.h"
#include "lsymtab.h"
#include "impexp.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "objstrip.h"

void HandleImport(length_name* intname, length_name* modname, length_name* extname, unsigned long ordinal)
{
	symbol* sym;

	sym = SymOp(ST_CREATE, intname->name, intname->len);
	if (!(sym->info & SYM_DEFINED))
	{
		if (CurrMod != NULL)
		{
			Ring2Append(&CurrMod->publist, sym);
			sym->mod = CurrMod;
		}
		if (FmtData.type & (MK_OS2 | MK_PE | MK_WIN_VXD))
		{
			MSImportKeyword(sym, modname, extname, ordinal);
		}
		else
		{
			SET_SYM_TYPE(sym_info, sym, SYM_IMPORTED);
			DO_OR_EQ(sym_info, sym->info, |=, SYM_DEFINED | SYM_DCE_REF);
			if (FmtData.type & MK_NOVELL)
			{
				SetNovImportSymbol(sym);
			}
			else
			{
				SetELFImportSymbol(sym);
			}
		}
	}
}