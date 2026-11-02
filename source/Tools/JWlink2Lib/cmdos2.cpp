#include "pch.h"
#include "cmdos2.h"
#include "structs.h"
#include "globals.h"
#include "util.h"

bool getimport(void)
{
	length_name         intname;
	length_name         modname;
	length_name         extname;
	uint16_t         ordinal;
	ord_state           state;

	//DEBUG((DBG_OLD, "cmdos2.getimport: enter"));
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
	state = ST_INVALID_ORDINAL;   // assume to extname or ordinal.
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
			//LnkMsg(LOC + LINE + MSG_IMPORT_ORD_INVALID + ERR, NULL);
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

bool ProcOS2Import(void)
{
	return(ProcArgList(&getimport, (tokcontrol)0));
}
