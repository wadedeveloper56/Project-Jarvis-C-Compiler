#include "pch.h"
#include "cmdnov.h"
#include "structs.h"
#include "globals.h"
#include "command.h"
#include "util.h"
#include "exeelf.h"
#include "cmdall.h"
#include "cmdline.h"
#include "cmddos.h"
#include "ovlstruc.h"
#include "start.h"
#include "lsymtab.h"
#include "exenov.h"
#include "loadnov.h"

static bool             GetNovImport(void);
static bool             GetNovExport(void);
static bool             ProcModuleTypeN(int n);

static bool IsNetWarePrefix(const char* pToken, unsigned nLen)
{
	if (NULL == pToken)
		return(FALSE);
	if ((pToken[0] == '(') && (pToken[nLen - 1] == ')'))
		return(TRUE);
	return(FALSE);
}

#define IS_NUMBER(ptr)     ((*ptr >= '0') && (*ptr <= '9'))
#define IS_WHITESPACE(ptr) (*(ptr) == ' ' || *(ptr) =='\t' || *(ptr) == '\r')

static bool NetWareSplitSymbol(char* tokenThis, unsigned tokenLen, char** name, unsigned* namelen, char** prefix, unsigned* prefixlen)
{
	char* findAt = tokenThis;
	unsigned    nLen = tokenLen;

	if ((NULL == tokenThis) || (0 == tokenLen) || (NULL == name) || (NULL == namelen) || (NULL == prefix) || (NULL == prefixlen))
		return(FALSE);

	*name = *prefix = NULL;
	*namelen = *prefixlen = 0;

	while (nLen)
	{
		if ('@' == *findAt)
			break;
		if ('\0' == *findAt)
		{
			nLen = 0;      
			break;
		}
		findAt++;
		nLen--;
	}

	if (0 == nLen)
	{
		*name = tokenThis;
		*namelen = tokenLen;
		return(TRUE);
	}


	if (IS_NUMBER(&findAt[1]))
	{
		*name = tokenThis;
		*namelen = tokenLen;
		return(TRUE);
	}

	*prefix = tokenThis;
	*prefixlen = findAt - tokenThis;

	*name = &findAt[1];
	*namelen = nLen - 1;

	return(TRUE);
}

static bool DoWeNeedToSkipASeparator(bool CheckDirectives)
{
	char* parse;

	if ((NULL == (parse = Token.next)) || ('\0' == *parse))
		return(FALSE);

	while (('\0' != *parse) && (IS_WHITESPACE(parse)))
		parse++;

	if ('\0' == *parse)
		return(FALSE);

	if (',' == *parse)
		return(FALSE);

	if (('\n' == *parse) || ('\r' == *parse))
		parse++;
	if (('\n' == *parse) || ('\r' == *parse))
		parse++;

	if (('\0' != *parse) && (',' != *parse))
	{
		if (CheckDirectives)
		{
			unsigned    len = 0;
			char* t = parse;

			while (!IS_WHITESPACE(t))
			{
				t++;
				len++;
			}

			if (MatchOne(Directives, SEP_NO, parse, len))
			{
				return(FALSE);
			}
		}
		return(TRUE);
	}
	return(FALSE);
}

bool ProcNovImport(void)
{
	SetCurrentPrefix(NULL, 0);
	return(ProcArgListEx(GetNovImport, TOK_INCLUDE_DOT, CmdFile));
}

bool ProcNovExport(void)
{
	SetCurrentPrefix(NULL, 0);
	return(ProcArgListEx(GetNovExport, TOK_INCLUDE_DOT, CmdFile));
}

#ifndef NDEBUG
extern int printf(const char* fmt, ...);
#endif

static bool GetNovImport(void)
{
	symbol* sym;
	char* name = NULL;
	char* prefix = NULL;
	unsigned    namelen = 0;
	unsigned    prefixlen = 0;

	if (IsNetWarePrefix(Token.this1, Token.len))
	{
		bool result;
		if (FALSE == (result = SetCurrentPrefix(Token.this1, Token.len)))
			return(FALSE);

		Token.skipToNext = DoWeNeedToSkipASeparator(FALSE);

#ifndef NDEBUG
		printf("Set new prefix. Skip = %d\n", Token.skipToNext);
#endif

		return(result);
	}

	if (!NetWareSplitSymbol(Token.this1, Token.len, &name, &namelen, &prefix, &prefixlen))
	{
		return(FALSE);
	}

	sym = SymOpNWPfx(ST_DEFINE_SYM, name, namelen, prefix, prefixlen);
	if (sym == NULL || sym->p.import != NULL)
	{
		return(TRUE);
	}

#ifndef NDEBUG
	printf("imported %s from %s\n", sym->name, sym->prefix ? sym->prefix : "(NONE)");
#endif

	SET_SYM_TYPE(sym_info, sym, SYM_IMPORTED);
	DO_OR_EQ(sym_info, sym->info, |=, SYM_DCE_REF);             
	SetNovImportSymbol(sym);

	Token.skipToNext = DoWeNeedToSkipASeparator(TRUE);

	return(TRUE);
}

void SetNovImportSymbol(symbol* sym)
{
	sym->p.import = DUMMY_IMPORT_PTR;
}

static bool GetNovExport(void)
{
	symbol* sym;
	char* name = NULL;
	char* prefix = NULL;
	unsigned    namelen = 0;
	unsigned    prefixlen = 0;

	if (IsNetWarePrefix(Token.this1, Token.len))
	{
		bool result;

		if (FALSE == (result = SetCurrentPrefix(Token.this1, Token.len)))
			return(FALSE);

		Token.skipToNext = DoWeNeedToSkipASeparator(FALSE);
		return(result);
	}

	if (!NetWareSplitSymbol(Token.this1, Token.len, &name, &namelen, &prefix, &prefixlen))
	{
		return(FALSE);
	}

	sym = SymOpNWPfx((sym_flags)(ST_CREATE | ST_REFERENCE), name, namelen, prefix, prefixlen);

	DO_OR_EQ(sym_info, sym->info, |=, SYM_DCE_REF | SYM_EXPORTED);

	AddNameTable(name, namelen, TRUE, &FmtData.u.nov.exp.export1);

	Token.skipToNext = DoWeNeedToSkipASeparator(TRUE);

	return(TRUE);
}

bool ProcScreenName(void)
{
	if (!GetToken(SEP_NO, TOK_INCLUDE_DOT))
	{
		return(FALSE);
	}
	if (Token.len > MAX_SCREEN_NAME_LENGTH)
	{
	}
	else
	{
		if (FmtData.u.nov.screenname != NULL)
		{
			free(FmtData.u.nov.screenname);      
		}
		FmtData.u.nov.screenname = tostring();
	}
	return(TRUE);
}

bool ProcCheck(void)
{
	if (!GetToken(SEP_EQUALS, TOK_INCLUDE_DOT))
	{
		return(FALSE);
	}
	FmtData.u.nov.checkfn = tostring();
	return(TRUE);
}

bool ProcMultiLoad(void)
{
	FmtData.u.nov.exeflags |= NOV_MULTIPLE;
	return(TRUE);
}

bool ProcAutoUnload(void)
{
	FmtData.u.nov.exeflags |= NOV_AUTOUNLOAD;
	return(TRUE);
}


bool ProcReentrant(void)
{
	FmtData.u.nov.exeflags |= NOV_REENTRANT;
	return(TRUE);
}

bool ProcSynch(void)
{
	FmtData.u.nov.exeflags |= NOV_SYNCHRONIZE;
	return(TRUE);
}

bool ProcPseudoPreemption(void)
{
	FmtData.u.nov.exeflags |= NOV_PSEUDOPREEMPTION;
	return(TRUE);
}

bool ProcNLMFlags(void)
{
	uint32_t value;

	if (!GetLong(&value)) return FALSE;
	FmtData.u.nov.exeflags |= value;
	return(TRUE);
}

bool ProcCustom(void)
{
	if (!GetToken(SEP_EQUALS, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME)))
	{
		return(FALSE);
	}
	FmtData.u.nov.customdata = tostring();            
	return(TRUE);
}

bool ProcMessages(void)
{
	if (!GetToken(SEP_EQUALS, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME)))
	{
		return(FALSE);
	}
	FmtData.u.nov.messages = tostring();              
	return(TRUE);
}

bool ProcHelp(void)
{
	if (!GetToken(SEP_EQUALS, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME)))
	{
		return(FALSE);
	}
	FmtData.u.nov.help = tostring();          
	return(TRUE);
}

bool ProcXDCData(void)
{
	if (!GetToken(SEP_EQUALS, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME)))
	{
		return(FALSE);
	}
	FmtData.u.nov.rpcdata = tostring();       
	return(TRUE);
}

bool ProcSharelib(void)
{
	if (!GetToken(SEP_EQUALS, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME)))
	{
		return(FALSE);
	}
	FmtData.u.nov.sharednlm = FileName(Token.this1, Token.len, E_NLM, FALSE);
	return(TRUE);
}

bool ProcExit(void)
{
	if (!GetToken(SEP_EQUALS, (tokcontrol)(TOK_INCLUDE_DOT)))
	{
		return(FALSE);
	}
	FmtData.u.nov.exitfn = tostring();
	return(TRUE);
}

bool ProcThreadName(void)
{
	if (!GetToken(SEP_NO, (tokcontrol)(TOK_INCLUDE_DOT)))
	{
		return(FALSE);
	}
	else if (Token.len > MAX_THREAD_NAME_LENGTH)
	{
	}
	else
	{
		FmtData.u.nov.threadname = tostring();
	}
	return(TRUE);
}

#define COPYRIGHT_START     "Copyright 1991"
#define DEFAULT_COPYRIGHT COPYRIGHT_START " Novell, Inc. All rights reserved"
#define DEFAULT_COPYRIGHT_LENGTH (sizeof( DEFAULT_COPYRIGHT ) - 1)
#define YEAR_OFFSET (sizeof( COPYRIGHT_START ) - 1)

bool ProcCopyright(void)
{
	struct tm* currtime;
	time_t          thetime;
	unsigned        year;
	char* copy_year;

	if (!GetToken(SEP_EQUALS, TOK_INCLUDE_DOT)
		&& !GetToken(SEP_NO, TOK_INCLUDE_DOT))
	{
		if (FmtData.u.nov.copyright != NULL)
		{
			free(FmtData.u.nov.copyright);      
		}
		FmtData.u.nov.copyright = (char*)malloc(DEFAULT_COPYRIGHT_LENGTH + 1);
		memcpy(FmtData.u.nov.copyright, DEFAULT_COPYRIGHT, DEFAULT_COPYRIGHT_LENGTH + 1);
		copy_year = FmtData.u.nov.copyright + YEAR_OFFSET;
		thetime = time(NULL);
		currtime = localtime(&thetime);
		year = currtime->tm_year + 1900;
		while (year > 0)
		{
			*copy_year = '0' + (year % 10);
			year /= 10;
			copy_year--;
		}
	}
	else
	{
		if (Token.len > MAX_COPYRIGHT_LENGTH)
		{
		}
		else
		{
			if (FmtData.u.nov.copyright != NULL)
			{
				free(FmtData.u.nov.copyright);      
			}
			FmtData.u.nov.copyright = tostring();
		}
	}
	return(TRUE);
}

bool ProcNovell(void)
{
	if (!ProcOne(NovModels, SEP_NO, FALSE))
	{     
		int     nType = 0;

		if ((nType = atoi(Token.this1)) > 0)
		{
			GetToken(SEP_NO, TOK_INCLUDE_DOT);
			ProcModuleTypeN(nType);
		}
		else
			ProcNLM();
	}
	if (!GetToken(SEP_QUOTE, TOK_INCLUDE_DOT))
	{      
		FmtData.u.nov.description = NULL;
		return(TRUE);
	}
	if (Token.len > MAX_DESCRIPTION_LENGTH)
	{
	}
	else
	{
		FmtData.u.nov.description = tostring();
	}
	return(TRUE);
}

void SetNovFmt(void)
{
	Extension = E_NLM;
	if (!(LinkState & FMT_SPECIFIED) && Name != NULL)
	{
		FmtData.u.nov.description = FileName(Name, strlen(Name), E_NLM,
			CmdFlags & CF_UNNAMED);
	}
}

void FreeNovFmt(void)
{
	free(FmtData.u.nov.screenname);
	free(FmtData.u.nov.description);
	free(FmtData.u.nov.checkfn);
	free(FmtData.u.nov.exitfn);
	free(FmtData.u.nov.customdata);
	free(FmtData.u.nov.threadname);
	free(FmtData.u.nov.copyright);
	free(FmtData.u.nov.messages);
	free(FmtData.u.nov.help);
	free(FmtData.u.nov.rpcdata);
	free(FmtData.u.nov.sharednlm);
}

bool ProcNLM(void)
{
	Extension = E_NLM;
	FmtData.u.nov.moduletype = 0;
	return(TRUE);
}

bool ProcLAN(void)
{
	Extension = E_LAN;
	FmtData.u.nov.moduletype = 1;
	return(TRUE);
}

bool ProcDSK(void)
{
	Extension = E_DSK;
	FmtData.u.nov.moduletype = 2;
	return(TRUE);
}

bool ProcNAM(void)
{
	Extension = E_NAM;
	FmtData.u.nov.moduletype = 3;
	return(TRUE);
}

bool     ProcModuleType4(void)
{
	Extension = E_NLM;
	FmtData.u.nov.moduletype = 4;
	return(TRUE);
}

bool     ProcModuleType5(void)
{
	Extension = E_NOV_MSL;
	FmtData.u.nov.moduletype = 5;
	return(TRUE);
}

bool     ProcModuleType6(void)
{
	Extension = E_NLM;
	FmtData.u.nov.moduletype = 6;
	return(TRUE);
}

bool     ProcModuleType7(void)
{
	Extension = E_NLM;
	FmtData.u.nov.moduletype = 7;
	return(TRUE);
}

bool     ProcModuleType8(void)
{
	Extension = E_NOV_HAM;
	FmtData.u.nov.moduletype = 8;
	return(TRUE);
}

bool     ProcModuleType9(void)
{
	Extension = E_NOV_CDM;
	FmtData.u.nov.moduletype = 9;
	return(TRUE);
}

#if 0
bool     ProcModuleType10(void)
{
	Extension = ;
	FmtData.u.nov.moduletype = 10;
	return(TRUE);
}

bool     ProcModuleType11(void)
{
	Extension = ;
	FmtData.u.nov.moduletype = 11;
	return(TRUE);
}

bool     ProcModuleType12(void)
{
	Extension = ;
	FmtData.u.nov.moduletype = 12;
	return(TRUE);
}
#endif

static bool     ProcModuleTypeN(int n)
{
	Extension = E_NLM;
	FmtData.u.nov.moduletype = n;
	return(TRUE);
}


static bool GetNovModule(void)
{
	AddNameTable(Token.this1, Token.len, FALSE, &FmtData.u.nov.exp.module);
	return(TRUE);
}

bool ProcModule(void)
{
	return(ProcArgList(GetNovModule, TOK_INCLUDE_DOT));
}

bool ProcOSDomain(void)
{
	FmtData.u.nov.exeflags |= NOV_OS_DOMAIN;
	return(TRUE);
}

bool ProcNovDBIExports(void)
{
	FmtData.u.nov.flags |= DO_NOV_EXPORTS;
	return(TRUE);
}

bool ProcNovDBIReferenced(void)
{
	FmtData.u.nov.flags |= DO_NOV_REF_ONLY;
	return(TRUE);
}

bool ProcNovDBI(void)
{
	LinkFlags |= NOVELL_DBI_FLAG;
	if (ProcOne(NovDBIOptions, SEP_NO, FALSE))
	{
		while (ProcOne(NovDBIOptions, SEP_COMMA, FALSE));  
	}
	return(TRUE);
}

bool ProcExportsDBI(void)
{
	DBIFlag = (::byte)((int)DBIFlag | DBI_ONLY_EXPORTS);
	FmtData.u.nov.flags |= DO_WATCOM_EXPORTS;
	return(TRUE);
}

void CmdNovFini(void)
{
	if (FmtData.u.nov.description == NULL && Name != NULL)
	{
		FmtData.u.nov.description = FileName(Name, strlen(Name), Extension,
			CmdFlags & CF_UNNAMED);
	}
}
