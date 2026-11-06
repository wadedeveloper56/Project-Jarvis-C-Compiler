#include "pch.h"
#include "cmdos2.h"
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
#include "objfree.h"

bool ProcOS2Import(void)
{
	return(ProcArgList(&getimport, (tokcontrol)0));
}

bool ProcOS2Export(void)
{
	bool    retval;

	if (GetToken(SEP_EQUALS, TOK_INCLUDE_DOT))
	{
		retval = GetWlibImports();
	}
	else
	{
		retval = ProcArgList(&getexport, (tokcontrol)0);
	}
	return(retval);
}

bool ProcAnonExport(void)
{
	bool    retval;

	DO_OR_EQ(commandflag, CmdFlags, |=, CF_ANON_EXPORT);
	retval = ProcOS2Export();
	DO_AND_EQ(commandflag, CmdFlags, &=, ~CF_ANON_EXPORT);
	return(retval);
}

bool ProcOS2Segment(void)
{
	return(ProcArgList(&getsegflags, (tokcontrol)TOK_INCLUDE_DOT));
}

entry_export* ProcWlibDLLImportEntry(void)
{
	uint16_t     ordinal;
	entry_export* exp;
	length_name     symname;
	length_name     internal;

	symname.len = Token.len;
	symname.name = (char*)malloc(Token.len + 1);
	memcpy(symname.name, Token.this1, Token.len);
	symname.name[Token.len] = '\0';
	if (!GetToken(SEP_DOT_EXT, (tokcontrol)0))
	{
		return(NULL);
	}
	internal.name = NULL;
	ordinal = 0;
	if (GetToken(SEP_DOT_EXT, (tokcontrol)0))
	{
		if (getatoi(&ordinal) != ST_IS_ORDINAL)
		{
			if (Token.len > 0)
			{
				internal = symname;
				symname.len = Token.len;
				symname.name = (char*)malloc(Token.len + 1);
				memcpy(symname.name, Token.this1, Token.len);
				symname.name[Token.len] = '\0';
			}
			if (GetToken(SEP_DOT_EXT, (tokcontrol)0)
				&& getatoi(&ordinal) != ST_IS_ORDINAL)
			{
				if (GetToken(SEP_DOT_EXT, (tokcontrol)0))
				{
					getatoi(&ordinal);
				}
			}
		}
	}
	exp = AllocExport(symname.name, symname.len);
	exp->isanonymous = (CmdFlags & CF_ANON_EXPORT) != 0;
	if (internal.name != NULL)
	{
		exp->sym = SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), internal.name, internal.len);
	}
	else
	{
		exp->sym = SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), symname.name, symname.len);
	}
	DO_OR_EQ(sym_info, exp->sym->info, |=, SYM_DCE_REF);           
	exp->ordinal = ordinal;
	if (ordinal == 0)
	{
		exp->isresident = TRUE;          
	}
	return(exp);
}

bool GetWlibImports(void)
{
	char* fname;
	f_handle        handle;
	entry_export* exp;

	fname = FileName(Token.this1, Token.len, E_LBC, FALSE);
	handle = QOpenR(fname);
	SetCommandFile(handle, fname);
	Token.locked = TRUE;             
	while (GetToken(SEP_SPACE, (tokcontrol)0))
	{
		if (Token.len <= 2)
			continue;
		if ((Token.this1[0] == '+') && (Token.this1[1] == '+'))
		{
			Token.this1 += 2;
			Token.len -= 2;
			if (Token.this1[0] == '\'')
			{
				Token.thumb = REJECT;
				if (!GetToken(SEP_QUOTE, (tokcontrol)0))
				{
					RestoreCmdLine();         
					return(TRUE);
				}
			}
			exp = ProcWlibDLLImportEntry();
			if (exp == NULL)
			{
				RestoreCmdLine();             
				return(TRUE);
			}
			AddToExportList(exp);
		}
	}
	Token.locked = FALSE;
	return(TRUE);
}

bool getimport(void)
{
	length_name         intname;
	length_name         modname;
	length_name         extname;
	uint16_t         ordinal;
	ord_state           state;

	intname.name = (char*)malloc(Token.len + 1);
	memcpy(intname.name, Token.this1, Token.len);
	intname.name[Token.len] = '\0';
	intname.len = Token.len;
	if (!GetToken(SEP_NO, (tokcontrol)0))
	{
		return(FALSE);
	}
	modname.name = (char*)malloc(Token.len + 1);
	memcpy(modname.name, Token.this1, Token.len);
	modname.name[Token.len] = '\0';
	modname.len = Token.len;
	state = ST_INVALID_ORDINAL;        
	if (GetToken(SEP_PERIOD, TOK_INCLUDE_DOT))
	{
		state = getatoi(&ordinal);
		if (state == ST_NOT_ORDINAL)
		{
			extname.name = (char*)malloc(Token.len + 1);
			memcpy(extname.name, Token.this1, Token.len);
			extname.name[Token.len] = '\0';
			extname.len = Token.len;
		}
		else if (state == ST_INVALID_ORDINAL)
		{
			return(TRUE);
		}
	}
	if (state == ST_IS_ORDINAL)
	{
		HandleImport(&intname, &modname, &intname, ordinal);
	}
	else
	{
		if (state == ST_NOT_ORDINAL)
		{
			HandleImport(&intname, &modname, &extname, NOT_IMP_BY_ORDINAL);
		}
		else
		{
			HandleImport(&intname, &modname, &intname, NOT_IMP_BY_ORDINAL);
		}
	}
	return(TRUE);
}

bool getexport(void)
{
	entry_export* exp;
	uint16_t     value;

	exp = AllocExport(Token.this1, Token.len);
	exp->isanonymous = (CmdFlags & CF_ANON_EXPORT) != 0;
	if (GetToken(SEP_PERIOD, TOK_INCLUDE_DOT))
	{
		if (getatol(&exp->ordinal) != ST_IS_ORDINAL)
		{
			free(exp);
			GetToken(SEP_EQUALS, TOK_INCLUDE_DOT);
			return(TRUE);
		}
	}
	if (GetToken(SEP_EQUALS, TOK_INCLUDE_DOT))
	{
		exp->sym = SymOp((sym_flags)(ST_CREATE | ST_REFERENCE), Token.this1, Token.len);
		if (GetToken(SEP_EQUALS, TOK_INCLUDE_DOT))
		{
			exp->impname = tostring();
		}
	}
	else
	{
		exp->sym = RefISymbol(exp->name);
	}
	DO_OR_EQ(sym_info, exp->sym->info, |=, SYM_DCE_REF);           
	if (exp->ordinal == 0)
	{
		exp->isresident = TRUE;           
	}
	exp->next = FmtData.u.os2.exports;            
	FmtData.u.os2.exports = exp;                 
	while (ProcOne(Exp_Keywords, SEP_NO, FALSE)) {}     
	FmtData.u.os2.exports = exp->next;            
	exp->iopl_words = 0;
	if (!(FmtData.type & (MK_WIN_NE | MK_PE)) && GetToken(SEP_NO, TOK_INCLUDE_DOT))
	{
		if (getatoi(&value) == ST_IS_ORDINAL)
		{
			if (value > 63)
			{
			}
			else
			{
				exp->iopl_words = value;
			}
		}
		else
		{
			Token.thumb = REJECT;       
		}
	}
	AddToExportList(exp);
	return(TRUE);
}

bool ProcExpResident(void)
{
	FmtData.u.os2.exports->isresident = TRUE;
	return(TRUE);
}

bool ProcPrivate(void)
{
	FmtData.u.os2.exports->isprivate = TRUE;
	return(TRUE);
}

bool ProcNoName(void)
{
	FmtData.u.os2.exports->isanonymous = TRUE;
	return(TRUE);
}

bool ProcOS2Alignment(void)
{
	ord_state           ret;
	uint32_t         value;

	if (!HaveEquals((tokcontrol)0)) return(FALSE);
	ret = getatol(&value);
	if (ret != ST_IS_ORDINAL || value == 0)
	{
		return(FALSE);
	}
	FmtData.u.os2.segment_shift = blog_32(value - 1) + 1;      
	return(TRUE);
}

bool ProcObjAlign(void)
{
	ord_state           ret;
	uint32_t         value;

	if (!HaveEquals((tokcontrol)0)) return(FALSE);
	ret = getatol(&value);
	if (ret != ST_IS_ORDINAL || value == 0)
	{
		return(FALSE);
	}                                                   
	if (value < 16 || value >(256 * 1024UL * 1024) || (value & (value - 1)))
	{
		return(FALSE);
	}
	FmtData.objalign = value;
	ChkBase(value);
	return(TRUE);
}

bool ProcModName(void)
{
	if (!HaveEquals(TOK_INCLUDE_DOT)) return(FALSE);
	FmtData.u.os2.res_module_name = totext();
	return(TRUE);
}

bool ProcNewFiles(void)
{
	FmtData.u.os2.flags |= LONG_FILENAMES;
	return(TRUE);
}

bool ProcProtMode(void)
{
	FmtData.u.os2.flags |= PROTMODE_ONLY;
	return(TRUE);
}

bool ProcOldLibrary(void)
{
	if (!HaveEquals((tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME))) return(FALSE);
	FmtData.u.os2.old_lib_name = FileName(Token.this1, Token.len, E_DLL, FALSE);
	return(TRUE);
}

bool ProcOS2HeapSize(void)
{
	ord_state           ret;
	uint32_t         value;

	if (!HaveEquals((tokcontrol)0)) return(FALSE);
	ret = getatol(&value);
	if (ret != ST_IS_ORDINAL || value == 0)
	{
	}
	else
	{
		FmtData.u.os2.heapsize = value;
	}
	return(TRUE);
}

bool ProcDescription(void)
{
	if (!GetToken(SEP_NO, TOK_INCLUDE_DOT))
	{
		return(FALSE);
	}
	FmtData.u.os2.description = tostring();
	return(TRUE);
}

bool ProcCommitStack(void)
{
	return(GetLong(&FmtData.u.pe.stackcommit));
}

bool ProcCommitHeap(void)
{
	return(GetLong(&FmtData.u.pe.heapcommit));
}

static bool AddCommit(void)
{
	Token.thumb = REJECT;
	if (ProcOne(CommitKeywords, SEP_NO, FALSE) == FALSE) return(FALSE);
	return(TRUE);
}

bool ProcCommit(void)
{
	return(ProcArgList(AddCommit, TOK_INCLUDE_DOT));
}

bool ProcRWRelocCheck(void)
{
	FmtData.u.os2.chk_seg_relocs = TRUE;
	return(TRUE);
}

bool ProcSelfRelative(void)
{
	FmtData.u.os2.gen_rel_relocs = TRUE;
	return(TRUE);
}

bool ProcInternalRelocs(void)
{
	FmtData.u.os2.gen_int_relocs = TRUE;
	return(TRUE);
}

bool ProcToggleRelocsFlag(void)
{
	FmtData.u.os2.toggle_relocs = TRUE;
	return(TRUE);
}

bool ProcMixed1632(void)
{
	LinkFlags &= ~FAR_CALLS_FLAG;        
	FmtData.u.os2.mixed1632 = TRUE;
	return(TRUE);
}

bool ProcPENoRelocs(void)
{
	LinkState &= ~MAKE_RELOCS;
	return(TRUE);
}

bool ProcNoStdCall(void)
{
	FmtData.u.pe.no_stdcall = TRUE;
	return(TRUE);
}

bool ProcFuzzyExport(void)
{
	FmtData.u.pe.fuzzy_export = TRUE;
	return(TRUE);
}

bool ProcOS2(void)
{
	Extension = E_LOAD;
	while (ProcOne(SubFormats, SEP_NO, FALSE)) {}    
	if (FmtData.type & MK_WIN_NE)
	{
		if (ProcOne(WindowsFormatKeywords, SEP_NO, FALSE))
		{
			ProcOne(WindowsFormatKeywords, SEP_NO, FALSE);
		}
	}
	else if (FmtData.type & MK_WIN_VXD)
	{
		ProcOne(VXDFormatKeywords, SEP_NO, FALSE);
		FmtData.dll = TRUE;
	}
	else
	{
		ProcOne(OS2FormatKeywords, SEP_NO, FALSE);
		if (FmtData.type & MK_OS2_LX)
		{
			if (FmtData.dll)
			{
				FmtData.u.os2.gen_int_relocs = TRUE;
			}
		}
	}
	if (FmtData.type & MK_ONLY_OS2_16)
	{            
		HintFormat(MK_ONLY_OS2_16);          
		if (FmtData.dll)
		{
			FmtData.u.os2.flags &= ~MULTIPLE_AUTO_DATA;
			FmtData.u.os2.flags |= SINGLE_AUTO_DATA;
		}
	}
	return(TRUE);
}

bool ProcPE(void)
{
	ProcOne(NTFormatKeywords, SEP_NO, FALSE);
	FmtData.u.pe.heapcommit = 4 * 1024;      
#if 1        
	FmtData.u.pe.os2.heapsize = 0x100000;
	StackSize = 0x100000;
#else
	FmtData.u.pe.os2.heapsize = 8 * 1024;     
#endif
	FmtData.u.pe.stackcommit = PE_DEF_STACK_COMMIT;
	FmtData.u.pe.os2.segment_shift = 9;        
	LinkState |= DOSSEG_FLAG;     

	return(TRUE);
}

#if 1    
bool ProcLargeAddressAware(void)
{
	FmtData.u.pe.largeaddressaware = 1;
	return(TRUE);
}

bool ProcNoLargeAddressAware(void)
{
	FmtData.u.pe.nolargeaddressaware = 1;
	return(TRUE);
}
#endif

bool ProcVXD(void)
{
	return(ProcOS2());
}

bool ProcWindows(void)
{
	return(ProcOS2());
}

void ChkBase(offset align)
{
	if (FmtData.objalign != NO_BASE_SPEC && FmtData.objalign > align)
	{
		align = FmtData.objalign;
	}
	if (FmtData.base != NO_BASE_SPEC &&
		(FmtData.base & (align - 1)) != 0)
	{
		FmtData.base = (FmtData.base + align - 1) & ~(align - 1);
	}
}

void SetOS2Fmt(void)
{
	if (LinkState & FMT_INITIALIZED) return;
	LinkState |= FMT_INITIALIZED;
	FmtData.u.os2.flags = MULTIPLE_AUTO_DATA;
	FmtData.u.os2.heapsize = 0;
	FmtData.u.os2.segment_shift = -1;
	FmtData.u.os2.res_module_name = NULL;
	FmtData.u.os2.stub_file_name = NULL;
	FmtData.u.os2.os2_seg_flags = NULL;
	FmtData.u.os2.description = NULL;
	FmtData.u.os2.exports = NULL;
	FmtData.u.os2.old_lib_name = NULL;
#if 0         
	if (FmtData.type & MK_WIN_NE)
	{
		FmtData.def_seg_flags |= SEG_PRELOAD;
	}
#endif
	Extension = E_LOAD;
	ChkBase(64 * 1024);
}

void FreeOS2Fmt(void)
{
	free(FmtData.u.os2.stub_file_name);
	free(FmtData.u.os2.res_module_name);
	free(FmtData.u.os2.old_lib_name);
	free(FmtData.u.os2.description);
	FreeImpNameTab();
	FreeSegFlags((seg_flags*)FmtData.u.os2.os2_seg_flags);
}

bool ProcLE(void)
{
	return(TRUE);
}

bool ProcLX(void)
{
	return(TRUE);
}

bool ProcOS2DLL(void)
{
	FmtData.dll = TRUE;
	Extension = E_DLL;
	if (FmtData.type & MK_WIN_NE)
	{
		FmtData.u.os2.flags &= ~MULTIPLE_AUTO_DATA;
		FmtData.u.os2.flags |= SINGLE_AUTO_DATA;
		FmtData.def_seg_flags |= SEG_PURE | SEG_MOVABLE;
	}
	if (ProcOne(Init_Keywords, SEP_NO, FALSE))
	{
		if (!ProcOne(Term_Keywords, SEP_NO, FALSE))
		{
			if (FmtData.u.os2.flags & INIT_INSTANCE_FLAG)
			{
				FmtData.u.os2.flags |= TERM_INSTANCE_FLAG;
			}
		}
	}
	return(TRUE);
}

bool ProcPhysDevice(void)
{
	FmtData.dll = TRUE;
	Extension = E_DLL;
	FmtData.u.os2.flags |= PHYS_DEVICE;
	return(TRUE);
}

bool ProcVirtDevice(void)
{
	FmtData.dll = TRUE;
	Extension = E_DLL;
	FmtData.u.os2.flags |= VIRT_DEVICE;
	return(TRUE);
}

bool ProcPM(void)
{
	FmtData.u.os2.flags |= PM_APPLICATION;
	return(TRUE);
}

bool ProcPMCompatible(void)
{
	FmtData.u.os2.flags |= PM_COMPATIBLE;
	return(TRUE);
}

bool ProcPMFullscreen(void)
{
	FmtData.u.os2.flags |= PM_NOT_COMPATIBLE;
	return(TRUE);
}

bool ProcMemory(void)
{
	FmtData.u.os2.flags |= CLEAN_MEMORY;
	return(TRUE);
}

bool ProcFont(void)
{
	FmtData.u.os2.flags |= PROPORTIONAL_FONT;
	return(TRUE);
}

bool ProcDPMI(void)
{
	FmtData.u.os2.dpmi16 = TRUE;      
	return(TRUE);
}

bool ProcDynamicDriver(void)
{
	FmtData.u.os2.flags |= VIRT_DEVICE;
	return(TRUE);
}

bool ProcStaticDriver(void)
{
	FmtData.u.os2.flags |= PHYS_DEVICE;
	return(TRUE);
}

bool ProcInitGlobal(void)
{
	FmtData.u.os2.flags &= ~INIT_INSTANCE_FLAG;
	return(TRUE);
}

bool ProcInitInstance(void)
{
	FmtData.u.os2.flags |= INIT_INSTANCE_FLAG;
	return(TRUE);
}

bool ProcInitThread(void)
{
	FmtData.u.os2.flags |= INIT_THREAD_FLAG;
	return(TRUE);
}

bool ProcTermGlobal(void)
{
	FmtData.u.os2.flags &= ~TERM_INSTANCE_FLAG;
	return(TRUE);
}

bool ProcTermInstance(void)
{
	FmtData.u.os2.flags |= TERM_INSTANCE_FLAG;
	return(TRUE);
}

bool ProcTermThread(void)
{
	FmtData.u.os2.flags |= TERM_THREAD_FLAG;
	return(TRUE);
}

static bool getsegflags(void)
{
	os2_seg_flags* entry;

	Token.thumb = REJECT;
	entry = (os2_seg_flags*)malloc(sizeof(os2_seg_flags));
	entry->specified = 0;
	entry->flags = FmtData.def_seg_flags;      
	entry->name = NULL;
	entry->type = SEGFLAG_SEGMENT;
	entry->next = FmtData.u.os2.os2_seg_flags;
	FmtData.u.os2.os2_seg_flags = entry;
	ProcOne(SegDesc, SEP_NO, FALSE);               
	if (entry->type != SEGFLAG_CODE && entry->type != SEGFLAG_DATA)
	{
		if (!GetToken(SEP_NO, TOK_INCLUDE_DOT))
		{
			FmtData.u.os2.os2_seg_flags = entry->next;
			free(entry);
			return(FALSE);
		}
		entry->name = tostring();
	}
	while (ProcOne(SegModel, SEP_NO, FALSE)) {}
	return(TRUE);
}

bool ProcOS2Class(void)
{
	FmtData.u.os2.os2_seg_flags->type = SEGFLAG_CLASS;
	return(TRUE);
}

bool ProcSegType(void)
{
	if (!ProcOne(SegTypeDesc, SEP_NO, FALSE))
	{
	}
	return(TRUE);
}

bool ProcSegCode(void)
{
	FmtData.u.os2.os2_seg_flags->type = SEGFLAG_CODE;
	return(TRUE);
}

bool ProcSegData(void)
{
	FmtData.u.os2.os2_seg_flags->type = SEGFLAG_DATA;
	return(TRUE);
}

bool ProcPreload(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_PRELOAD)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_PRELOAD;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_PRELOAD;
	return(TRUE);
}

bool ProcLoadoncall(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_PRELOAD)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_PRELOAD;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_PRELOAD;
	return(TRUE);
}

bool ProcIopl(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_IOPL_SPECD)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_LEVEL_3;
	FmtData.u.os2.os2_seg_flags->flags |= SEG_LEVEL_2;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_IOPL_SPECD;
	return(TRUE);
}

bool ProcNoIopl(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_IOPL_SPECD)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_LEVEL_3;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_IOPL_SPECD;
	return(TRUE);
}

bool ProcExecuteonly(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_READ_ONLY;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_ONLY;
	return(TRUE);
}

bool ProcExecuteread(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_READ_ONLY;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_ONLY;
	return(TRUE);
}

bool ProcReadOnly(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_READ_ONLY;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_SPECD;
	return(TRUE);
}

bool ProcReadWrite(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_READ_ONLY;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_SPECD;
	return(TRUE);
}

bool ProcShared(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_PURE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_PURE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_PURE;
	return(TRUE);
}

bool ProcWritable(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_WRITABLE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_WRITABLE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_WRITABLE;
	return(TRUE);
}
bool ProcExecutable(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_EXECUTABLE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_EXECUTABLE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_EXECUTABLE;
	return(TRUE);
}

bool ProcNonShared(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_PURE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_PURE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_PURE;
	return(TRUE);
}

bool ProcConforming(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_CONFORMING)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_CONFORMING;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_CONFORMING;
	return(TRUE);
}

bool ProcNonConforming(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_CONFORMING)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_CONFORMING;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_CONFORMING;
	return(TRUE);
}

bool ProcMovable(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_MOVABLE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_MOVABLE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_MOVABLE;
	return(TRUE);
}

bool ProcFixed(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_MOVABLE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_MOVABLE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_MOVABLE;
	return(TRUE);
}

bool ProcNonPageable(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_NOPAGE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_NOPAGE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_NOPAGE;
	return(TRUE);
}

bool ProcPageable(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_NOPAGE)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_NOPAGE;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_NOPAGE;
	return(TRUE);
}

bool ProcDiscardable(void)
{
	FmtData.u.os2.os2_seg_flags->flags |= SEG_DISCARD;
	return(TRUE);
}

bool ProcNonDiscardable(void)
{
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_DISCARD;
	return(TRUE);
}

bool ProcOS2Dynamic(void)
{
	return(TRUE);
}

bool ProcInvalid(void)
{
	FmtData.u.os2.os2_seg_flags->flags |= SEG_INVALID;
	return(TRUE);
}

bool ProcPermanent(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_RESIDENT)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags |= SEG_RESIDENT;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_RESIDENT;
	return(TRUE);
}

bool ProcNonPermanent(void)
{
	if (FmtData.u.os2.os2_seg_flags->specified & SEG_RESIDENT)
	{
	}
	FmtData.u.os2.os2_seg_flags->flags &= ~SEG_RESIDENT;
	FmtData.u.os2.os2_seg_flags->specified |= SEG_RESIDENT;
	return(TRUE);
}

bool ProcContiguous(void)
{
	FmtData.u.os2.os2_seg_flags->flags |= SEG_CONTIGUOUS;
	return(TRUE);
}

bool ProcSingle(void)
{
	if (CmdFlags & CF_AUTO_SEG_FLAG)
	{
	}
	FmtData.u.os2.flags |= SINGLE_AUTO_DATA;
	FmtData.u.os2.flags &= ~MULTIPLE_AUTO_DATA;
	return(TRUE);
}

bool ProcMultiple(void)
{
	if (CmdFlags & CF_AUTO_SEG_FLAG)
	{
	}
	FmtData.u.os2.flags &= ~SINGLE_AUTO_DATA;
	FmtData.u.os2.flags |= MULTIPLE_AUTO_DATA;
	return(TRUE);
}

bool ProcNone(void)
{
	if (CmdFlags & CF_AUTO_SEG_FLAG)
	{
	}
	FmtData.u.os2.flags &= ~(SINGLE_AUTO_DATA | MULTIPLE_AUTO_DATA);
	return(TRUE);
}

bool ProcRunNative(void)
{
	FmtData.u.pe.subsystem = PE_SS_NATIVE;
	ParseVersion();
	return(TRUE);
}

bool ProcRunWindows(void)
{
	FmtData.u.pe.subsystem = PE_SS_WINDOWS_GUI;
	ParseVersion();
	return(TRUE);
}

bool ProcRunConsole(void)
{
	FmtData.u.pe.subsystem = PE_SS_WINDOWS_CHAR;
	ParseVersion();
	return(TRUE);
}

bool ProcRunPosix(void)
{
	FmtData.u.pe.subsystem = PE_SS_POSIX_CHAR;
	ParseVersion();
	return(TRUE);
}

bool ProcRunOS2(void)
{
	FmtData.u.pe.subsystem = PE_SS_OS2_CHAR;
	ParseVersion();
	return(TRUE);
}


bool ProcRunDosstyle(void)
{
	FmtData.u.pe.subsystem = PE_SS_PL_DOSSTYLE;
	FmtData.u.pe.tnt = TRUE;
	ParseVersion();
	return(TRUE);
}

bool ProcTNT(void)
{
	FmtData.u.pe.tnt = TRUE;
	return(TRUE);
}

bool ProcHX(void)
{
	FmtData.u.pe.hx = TRUE;
	return(TRUE);
}

bool ProcNXCompat(void)
{
	FmtData.u.pe.nxcompat = TRUE;
	return(TRUE);
}

static void ParseVersion(void)
{
	ord_state   retval;

	if (!GetToken(SEP_EQUALS, (tokcontrol)0)) return;
	FmtData.u.pe.submajor = 0;
	FmtData.u.pe.subminor = 0;
	retval = getatoi(&FmtData.u.pe.submajor);
	if (retval != ST_IS_ORDINAL)
	{
		return;
	}
	FmtData.u.pe.sub_specd = TRUE;
	if (!GetToken(SEP_PERIOD, (tokcontrol)0))
	{        
		return;                             
	}
	retval = getatoi(&FmtData.u.pe.subminor);
	if (retval != ST_IS_ORDINAL)
	{
	}
}

static bool AddResource(void)
{
	DoAddResource(tostring());
	return(TRUE);
}

bool ProcResource(void)
{
	return(ProcArgList(&AddResource, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME)));
}

static uint32_t ProcGenericVersion(VersBlock* pVers, uint32_t major_limit, uint32_t minor_limit, uint32_t revision_limit)
{
	uint32_t state = 0;
	ord_state   retval;
	uint32_t value;

	if (NULL == pVers)
	{
		return(state);
	}
	if (!GetToken(SEP_EQUALS, (tokcontrol)0))
	{
		return(state);
	}

	retval = getatol(&value);
	if (retval != ST_IS_ORDINAL)
	{
		return(state);
	}
	else if ((major_limit) && (value > major_limit))
	{
		return(state);
	}
	pVers->major = value;
	pVers->minor = 0;
	pVers->revision = 0;
	state |= (valid_result | major_valid);

	if (!GetToken(SEP_PERIOD, (tokcontrol)0))
	{        
		return(state);                         
	}
	retval = getatol(&value);
	if (retval != ST_IS_ORDINAL)
	{
		return(state);
	}
	else if ((minor_limit) && (value > minor_limit))
	{
		return(state);
	}

	pVers->minor = value;
	state |= minor_valid;

	if (!GetToken(SEP_PERIOD, (tokcontrol)0))
	{        
		return(state);                     
	}

	retval = getatol(&value);
	if (retval == ST_NOT_ORDINAL && Token.len == 1)
	{
		value = tolower(*Token.this1) - 'a' + 1;
	}
	else if (retval == ST_NOT_ORDINAL)
	{
		return(state);
	}

	if ((revision_limit) && (value > revision_limit))
	{
		return(state);
	}
	pVers->revision = value;
	state |= revision_valid;
	return(state);
}

bool     ProcLinkVersion(void)
{
	uint32_t result;
	VersBlock   vb;

	result = ProcGenericVersion(&vb, 255, 255, 0);
	if (!(result & valid_result))
	{
		return(FALSE);        
	}

	FmtData.u.pe.lnk_specd = 1;
	FmtData.u.pe.linkmajor = (result & major_valid) ? vb.major : 0;
	FmtData.u.pe.linkminor = (result & minor_valid) ? vb.minor : 0;

	return(TRUE);
}

bool     ProcOsVersion(void)
{
	uint32_t result;
	VersBlock   vb;

	result = ProcGenericVersion(&vb, 0, 99, 0);          
	if (!(result & valid_result))
	{
		return(FALSE);        
	}

	FmtData.u.pe.osv_specd = 1;
	FmtData.u.pe.osmajor = (result & major_valid) ? vb.major : 0;
	FmtData.u.pe.osminor = (result & minor_valid) ? vb.minor : 0;

	return(TRUE);
}

bool     ProcChecksum(void)
{
	FmtData.u.pe.checksumfile = 1;
	return(TRUE);
}

void FreeImpNameTab(void)
{
	FmtData.u.os2.mod_ref_list = NULL;      
	FmtData.u.os2.imp_tab_list = NULL;
}
