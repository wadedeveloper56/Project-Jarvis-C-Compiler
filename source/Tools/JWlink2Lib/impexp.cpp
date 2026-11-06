#include "pch.h"
#include "cmdos2.h"
#include "structs.h"
#include "globals.h"
#include "util.h"
#include "objpass1.h"
#include "loadpe.h"
#include "loados2.h"
#include "impexp.h"

void MSImportKeyword(symbol* sym, length_name* modname, length_name* extname, unsigned long ordinal)
{
	dll_sym_info* dll;

	if (!(sym->info & SYM_DEFINED))
	{
		DO_OR_EQ(sym_info, sym->info, |=, SYM_DEFINED | SYM_DCE_REF);
		if (LinkFlags & STRIP_CODE)
		{
			//DEBUG((DBG_OLD, "MSImportKeyword(%s): calling DefStripImpSym", sym->name));
			//FIXME DefStripImpSym(sym);
		}
		SET_SYM_TYPE(sym_info, sym, SYM_IMPORTED);
		//FIXME dll = AllocDLLInfo();
		sym->p.import = dll;
		if (FmtData.type & MK_PE)
		{
			//DEBUG((DBG_OLD, "MSImportKeyword(%s, info=%h): calling GetIATSym", sym->name, sym->info));
			//FIXME dll->iatsym = GetIATSym(sym);
			DO_OR_EQ(sym_info, dll->iatsym->info, |=, SYM_DEFINED | SYM_DCE_REF);
			SET_SYM_TYPE(sym_info, dll->iatsym, SYM_IMPORTED);
			dll->iatsym->p.import = NULL;
		}
		dll->m.modnum = AddNameTable(modname->name, modname->len, TRUE,
			&FmtData.u.os2.mod_ref_list);
		dll->isordinal = ordinal != NOT_IMP_BY_ORDINAL;
		if (!dll->isordinal)
		{
			dll->u.entry = AddNameTable(extname->name, extname->len, FALSE, &FmtData.u.os2.imp_tab_list);
		}
		else
		{
			dll->u.ordinal = ordinal;
		}
	}
}

void AddToExportList(entry_export* exp)
{
}

entry_export* AllocExport(char* name, unsigned len)
{
	return nullptr;
}
