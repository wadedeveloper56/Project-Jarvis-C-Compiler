#include "pch.h"
#include "globals.h"
#include "start.h"
#include "ntio.h"
#include "util.h"

static  char* DefExt[] = {
#undef pick1
#define pick1(enum,text) text,
#include "ldefext.h"
};

char* ChkStrDup(char* str)
{
	size_t      len;
	char* copy;

	len = strlen(str) + 1;
	copy = (char*)malloc(len);
	memcpy(copy, str, len);
	return(copy);
}

char* ChkToString(void* mem, unsigned len)
{
	char* str = (char*)malloc(len + 1);
	memcpy(str, mem, len);
	str[len] = '\0';
	return(str);
}

char* FileName(char* buff, unsigned len, file_defext etype, bool force)
{
	char* namptr;
	char* namstart;
	char* ptr;
	unsigned    cnt;
	unsigned    namelen;

	namptr = buff + len;
	cnt = 0;
	while (cnt != len)
	{
		cnt++;
		--namptr;
		if (IS_PATH_SEP(*namptr))
		{
			break;
		}
	}
	if (IS_PATH_SEP(*namptr))
	{
		namptr++;
	}
	namstart = namptr;
	cnt = len - (namptr - buff);
	if (cnt == 0)
	{
		ptr = (char*)malloc(len + 1);
		memcpy(ptr, buff, len);
		ptr[len] = '\0';
		//LnkMsg(LOC + LINE + FTL + MSG_INV_FILENAME, "s", ptr);
	}
	namelen = cnt;
	namptr = buff + len - 1;
	while (--cnt != 0 && *namptr != '.')
	{
		namptr--;
	}
	if (force || *namptr != '.')
	{
		if (force && etype == E_MAP)
		{         // op map goes in current dir.
			buff = namstart;
			len = namelen;
		}
		if (cnt != 0)
		{
			len = namptr - buff;
		}
		ptr = (char*)malloc(len + strlen(DefExt[etype]) + 1);
		memcpy(ptr, buff, len);
		strcpy(ptr + len, DefExt[etype]);
	}
	else
	{
		ptr = ChkToString(buff, len);
	}
	return(ptr);
}

bool GetToken(sep_type req, tokcontrol ctrl)
{
	return(GetTokenEx(req, ctrl, NULL, NULL));
}

bool GetTokenEx(sep_type req, tokcontrol ctrl, cmdfilelist* resetpoint, bool* pbreset)
{
	char    hmm;
	bool    ret;
	bool    need_sep;

	if (Token.thumb == REJECT)
	{
		Token.thumb = OK;
		if (Token.quoted) return(TRUE);
		Token.next = Token.this1;        /* re-process last token */
	}
	need_sep = TRUE;
	for (;;)
	{                           /* finite state machine */

/*
//  carl.young
//  We had a situation where an input file (in this case a Novell
//  import or export file) does not have the consistent format
//  expected from this FSM code. If the skipToNext flag is set,
//  then we just skip to the next token and return rather than
//  reporting an error.
//  For reference the import files looked like:
//      (PREFIX)
//          symbol1,
//          symbol2,
//          symbolnm1,
//          symboln
//
//  Note the missing comma separator after the prefix token. The
//  prefix token(s) may also appear anywhere in the file.
*/

		if ((Token.skipToNext) && (req == SEP_COMMA))
		{
			Token.skipToNext = 0;
			need_sep = FALSE;
		}

		switch (Token.where)
		{
			case MIDST:
				EatWhite();
				hmm = *Token.next;
				switch (hmm)
				{
					case CTRLZ:
						Token.where = ENDOFFILE;
						break;
					case '\0':
						if (Token.how == BUFFERED
							|| Token.how == ENVIRONMENT
							|| Token.how == SYSTEM)
						{
							Token.where = ENDOFFILE;
							break;
						}                // NOTE the fall through.
					case '\n':
						if (Token.how == BUFFERED
							|| Token.how == ENVIRONMENT
							|| Token.how == SYSTEM)
						{
							Token.next++;               // just skip this.
						}
						else if (Token.how == COMMANDLINE)
						{
							Token.where = ENDOFCMD;
						}
						else
						{
							Token.where = ENDOFLINE;
						}
						Token.line++;
						break;
					case '@':
						if (req != SEP_SPACE)
						{
							Token.next++;
							GetToken(SEP_NO, (tokcontrol)(TOK_INCLUDE_DOT | TOK_IS_FILENAME));
							StartNewFile();
							break;
						}
						Token.next--;   /* make a token out of this */
						ret = MakeToken(ctrl, req);
						Token.quoted = 0;
						return(ret);
					case '#':
						Token.where = ENDOFLINE;            /* begin comment */
						Token.line++;
						break;
					case '^':
						if (req != SEP_SPACE)
						{    /* if not storing system blocks */
							Token.next++;
							BackupParser();
							break;
						}
						Token.next--;   /* make a token out of this */
						ret = MakeToken(ctrl, req);
						Token.quoted = 0;
						return(ret);
					case '%':
						if (req != SEP_SPACE)
						{
							ExpandEnvVariable();
							break;
						}
					default:
						if (need_sep)
						{
							if (need_sep != SEP_QUOTE)
								Token.quoted = 0;
							switch (req)
							{
								case SEP_NO:
									if (hmm == ',' || hmm == '=') return(FALSE);
									break;
								case SEP_COMMA:
									if (hmm != ',') return(FALSE);
									Token.next++;
									break;
								case SEP_EQUALS:
									if (hmm != '=') return(FALSE);
									Token.next++;
									break;
								case SEP_PERIOD:
								case SEP_DOT_EXT:
									if (hmm != '.') return(FALSE);
									Token.next++;
									break;
								case SEP_PAREN:
									if (hmm != '(') return(FALSE);
									Token.next++;
									break;
								case SEP_LCURLY:
									if (hmm != '{') return(FALSE);
									Token.next++;
									break;
								case SEP_QUOTE:
									//if( hmm != '\'' ) return( FALSE );
									if (hmm != Token.quoted) return(FALSE);
									Token.next++;
									//Token.quoted = hmm;
									break;
								case SEP_RCURLY:
									if (hmm != '}') return(FALSE);
									Token.next++;
									return(TRUE);
								case SEP_END:
									return(FALSE);
							}
							need_sep = FALSE;
							EatWhite();
						}
						else
						{                /*  must have good separator here */
									 //if( hmm == '\'' && req != SEP_PAREN && req != SEP_SPACE ) {
							if ((hmm == '\'' || hmm == '"') && req != SEP_PAREN && req != SEP_SPACE)
							{
								req = SEP_QUOTE;   /* token has been quoted */
								Token.next++;      /* don't include the quote */
								Token.quoted = hmm;
							}
							ret = MakeToken(ctrl, req);
							return(ret);
						}
						break;
				}
				break;
			case ENDOFLINE:
				GetNewLine();
				break;
			case ENDOFFILE:
				if (Token.locked) return(FALSE);
				RestoreCmdLine();
				if (Token.thumb == REJECT)
				{
					Token.thumb = OK;
					Token.next = Token.this1;        /* re-process last token */
				}
				Token.quoted = 0;
				if (resetpoint && (CmdFile == resetpoint))
				{
					if (*Token.next == ',')
						break;
					if (pbreset)
						*pbreset = TRUE;            /* Show we have hit a file end-point for a directive */
					return(FALSE);
				}
				break;
			case ENDOFCMD:
				if (CmdFile->next != NULL)
				{
					RestoreParser();
					break;
				}
				Token.quoted = 0;
				ret = (req == SEP_END) ? TRUE : FALSE;
				return(ret);
		}
	}
}

#define IS_WHITESPACE(ptr) (*(ptr) == ' ' || *(ptr) =='\t' || *(ptr) == '\r')

void EatWhite(void)
{
	while (IS_WHITESPACE(Token.next))
	{
		Token.next++;
	}
}

void StartNewFile(void)
{
	char* fname;
	char* envstring;
	char* buff;
	f_handle    file;

	fname = FileName(Token.this1, Token.len, E_COMMAND, FALSE);
	file = QObjOpen(fname);
	if (file == NIL_HANDLE)
	{
		free(fname);
		fname = tostring();
		envstring = GetEnvString(fname);
		if (envstring != NULL)
		{
			buff = ChkStrDup(envstring);
			NewCommandSource(fname, buff, ENVIRONMENT);
		}
		else
		{
			//LnkMsg(LOC + LINE + ERR + MSG_CANT_OPEN_NO_REASON, "s", fname);
			free(fname);
			//Suicide();
		}
		return;
	}
	else
	{
		SetCommandFile(file, fname);
	}
	//DEBUG((DBG_OLD, "processing command file %s", CmdFile->name));
}

bool MakeToken(tokcontrol ctrl, sep_type separator)
/**********************************************************/
{
	bool        quit;
	char        hmm;
	unsigned    len;
	bool        forcematch;
	bool        hitmatch;
	bool        keepspecial;

	Token.this1 = Token.next;
	len = 0;
	quit = FALSE;
	forcematch = (separator == SEP_QUOTE) || (separator == SEP_PAREN)
		|| (separator == SEP_PERCENT);
	keepspecial = (separator == SEP_SPACE) || (separator == SEP_DOT_EXT);
	if (separator == SEP_DOT_EXT)
	{    /* KLUDGE! we want to allow a zero*/
		len--;                  /* length token for parsing wlib files, so */
		Token.next--;           /* artificially back up one here. */
	}
	if (*Token.next == '\\' && separator == SEP_QUOTE && !(ctrl & TOK_IS_FILENAME))
	{
		MapEscapeChar();        /* get escape chars starting in 1st pos. */
	}
	hmm = *Token.next;
	len += MapDoubleByteChar((unsigned char)hmm);
	hitmatch = FALSE;
	for (;;)
	{
		len++;
		hmm = *++Token.next;
		switch (hmm)
		{
			case '\'':
			case '"':
				if (separator == SEP_QUOTE && Token.quoted == hmm)
				{
					++Token.next;      // don't include end quote in next token.
					hitmatch = TRUE;
					quit = TRUE;
				}
				break;
			case ')':
				if (separator == SEP_PAREN)
				{
					++Token.next;    // don't include end paren in next token.
					hitmatch = TRUE;
					quit = TRUE;
				}
				break;
			case '%':
				if (separator == SEP_PERCENT)
				{
					++Token.next;    // don't include end percent in next token.
					hitmatch = TRUE;
					quit = TRUE;
				}
				break;
			case '.':
				if (!(ctrl & TOK_INCLUDE_DOT) && !forcematch)
				{
					quit = TRUE;
				}
				break;
			case '{':
			case '}':
			case '(':
			case ',':
			case '=':
			case '#':
			case '@':
				if (keepspecial)
				{
					break;
				}                   // NOTE the potential fall through
			case '\t':
			case ' ':
				if (!forcematch)
				{
					quit = TRUE;
				}
				break;
			case '\\':
				if (separator == SEP_QUOTE && !(ctrl & TOK_IS_FILENAME))
				{
					MapEscapeChar();
				}
				break;
			case '\0':
			case '\r':
			case '\n':
			case CTRLZ:
				quit = TRUE;
				break;
			default:
				len += MapDoubleByteChar((unsigned char)hmm);
		}
		if (quit)
		{
			break;
		}
	}
	Token.len = len;
	if (forcematch && !hitmatch)
	{
		return(FALSE);     // end quote/paren not found before token end.
	}
	return(TRUE);
}

void BackupParser(void)
{
	if (CmdFile->prev == NULL)
	{
		//LnkMsg(LOC + LINE + WRN + MSG_NO_PREVIOUS_INPUT, NULL);
		return;
	}
	memcpy(&CmdFile->token, &Token, sizeof(tok));   // save current state
	CmdFile = CmdFile->prev;
	memcpy(&Token, &CmdFile->token, sizeof(tok)); // restore old state.
}

void ExpandEnvVariable(void)
{
	char* envname;
	char* env;
	char* buff;
	size_t  envlen;

	Token.next++;
	if (!MakeToken(TOK_INCLUDE_DOT, SEP_PERCENT))
	{
		//LnkMsg(LOC + LINE + FTL + MSG_ENV_NAME_INCORRECT, NULL);
	}
	envname = tostring();
	env = GetEnvString(envname);
	if (env == NULL)
	{
		//LnkMsg(LOC + LINE + WRN + MSG_ENV_NOT_FOUND, "s", envname);
		free(envname);
	}
	else
	{
		envlen = strlen(env);
		if (!IS_WHITESPACE(Token.next))
		{
			MakeToken(TOK_INCLUDE_DOT, SEP_SPACE);
			buff = (char*)malloc(envlen + Token.len + 1);
			memcpy(buff, env, envlen);
			memcpy(buff + envlen, Token.this1, Token.len);
			buff[Token.len + envlen] = '\0';
		}
		else
		{
			buff = ChkToString(env, envlen);
		}
		NewCommandSource(envname, buff, ENVIRONMENT);
	}
}

void OutPutPrompt(char* str)
{
	if (QIsDevice(CmdFile->file))
	{
		//WriteStdOut(str);
	}
}

void GetNewLine(void)
{
	if (Token.how == BUFFERED
		|| Token.how == ENVIRONMENT
		|| Token.how == SYSTEM)
	{
		Token.where = MIDST;
		while (*Token.next != '\n')
		{       //go until next line found;
			if (*Token.next == '\0' || *Token.next == CTRLZ)
			{
				Token.where = ENDOFFILE;
				break;
			}
			Token.next++;
		}
		Token.next++;
	}
	else if (Token.how == NONBUFFERED)
	{
		if (QReadStr(CmdFile->file, Token.buff, MAX_REC, CmdFile->name))
		{
			Token.where = ENDOFFILE;
		}
		else
		{
			Token.where = MIDST;
		}
		Token.next = Token.buff;
	}
	else
	{               // interactive.
		OutPutPrompt(_LinkerPrompt);
		Token.how = INTERACTIVE;
		if (QReadStr(STDIN_HANDLE, Token.buff, MAX_REC, "console"))
		{
			Token.where = ENDOFCMD;
		}
		else
		{
			Token.where = MIDST;
		}
		Token.next = Token.buff;
	}
}

void RestoreCmdLine(void)
{
	cmdfilelist* temp;

	if (CmdFile->prev == NULL)
	{     /* can't free last file */
		Token.where = ENDOFCMD;
		return;
	}
	switch (Token.how)
	{
		case SYSTEM:
			break;
		default:
			free(Token.buff);
			if (CmdFile->file > STDIN_HANDLE)
			{
				QClose(CmdFile->file, CmdFile->name);
			}
			break;
	}
	if (CmdFile->symprefix)
		free(CmdFile->symprefix);
	CmdFile->symprefix = NULL;
	free(CmdFile->name);
	temp = CmdFile->prev;
	temp->next = CmdFile->next;
	if (temp->next != NULL)
	{
		temp->next->prev = temp;
	}
	free(CmdFile);
	CmdFile = temp;
	memcpy(&Token, &CmdFile->token, sizeof(tok)); // restore old state.
}

void RestoreParser(void)
{
	if (CmdFile->next == NULL) return;
	memcpy(&CmdFile->token, &Token, sizeof(tok));  /* save current state */
	CmdFile = CmdFile->next;
	memcpy(&Token, &CmdFile->token, sizeof(tok)); // restore old state.
}

char* tostring(void)
{
	return(ChkToString(Token.this1, Token.len));
}

void MapEscapeChar(void)
{
	int shift = 2;
	char *str = Token.next + 1;
	switch (*str)
	{
		case 'a':
			*Token.next = '\a';
			break;
		case 'b':
			*Token.next = '\b';
			break;
		case 'f':
			*Token.next = '\f';
			break;
		case 'n':
			*Token.next = '\n';
			break;
		case 'r':
			*Token.next = '\r';
			break;
		case 't':
			*Token.next = '\t';
			break;
		case 'v':
			*Token.next = '\v';
			break;
		case 'x':
			shift += ParseNumber(++str, 16);
			break;
		case '0': case '1': case '2': case '3': case '4': case'5': case '6':
		case '7': case '8': case '9':
			shift += ParseNumber(str, 8) - 1;
			break;
		default:
			*Token.next = *str;
			break;
	}
	str = Token.next + shift;
	memmove(Token.next + 1, str, strlen(str) + 1);
}

unsigned MapDoubleByteChar(unsigned char c)
{
	switch (CmdFlags & CF_LANGUAGE_MASK)
	{
		case CF_LANGUAGE_JAPANESE:
			if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC))
			{
				Token.next++;
				return(1);
			}
			break;
		case CF_LANGUAGE_CHINESE:
			if (c > 0xFC) break;
		case CF_LANGUAGE_KOREAN:            // note the fall through
			if (c > 0xFD) break;
			if (c < 0x81) break;
			Token.next++;
			return(1);
	}
	return(0);
}

int ParseNumber(char* str, int radix)
{
	bool        isdig;
	bool        isvalid;
	char        ch;
	int         size;
	unsigned    value;

	size = 0;
	value = 0;
	for (;;)
	{
		ch = tolower(*str);
		isdig = isdigit(ch);
		if (radix == 8)
		{
			isvalid = isdig && !(ch == '8' && ch == '9');
		}
		else
		{
			isvalid = isxdigit(ch);
		}
		if (!isvalid) break;
		value *= radix;
		if (isdig)
		{
			value += ch - '0';
		}
		else
		{
			value += ch - 'a' + 10;
		}
		size++;
		str++;
	}
	*Token.next = value;
	return(size);
}

bool ProcOne(parse_entry* entry, sep_type req, bool suicide)
{
	char* key;
	char* ptr;
	unsigned            plen;
	bool                ret;
	char                keybuff[20];

	ret = GetToken(req, TOK_INCLUDE_DOT);
	if (ret == FALSE)
	{
		return(ret);
	}
	while (entry->keyword != NULL)
	{
		key = entry->keyword;
		ptr = Token.this1;
		plen = Token.len;
		for (;;)
		{
			if (plen == 0 && !isupper(*key))
			{
				if (HintFormat(entry->format))
				{
					ret = (*entry->rtn)();
					DO_OR_EQ(commandflag, CmdFlags, |=, entry->flags);
				}
				else
				{
					strcpy(keybuff, entry->keyword);
					_strupr(keybuff); /* jwlink: was strlwr() */
					//LnkMsg(LOC + LINE + WRN + MSG_FORMAT_BAD_OPTION, "s", keybuff);
					ret = TRUE;
				}
				return(ret);
			}
			if (*key == '\0' || tolower(*ptr) != tolower(*key)) break;
			ptr++;
			key++;
			plen--;
		}
		/* here if this is no match */
		entry++;
	}
	/* here if no match in table */
	if (suicide)
	{
		//Syntax();
	}
	else
	{
		Token.thumb = REJECT;       /*  try again later */
		ret = FALSE;
	}
	return(ret);
}

bool HintFormat(exe_format hint)
{
	return true;
}

bool HaveEquals(tokcontrol ctrl)
{
	if (GetToken(SEP_EQUALS, ctrl) == FALSE)
	{
		Token.this1 = Token.next;
		/* collect the token that caused the problem */
		GetToken(SEP_NO, ctrl);
		return(FALSE);
	}
	return(TRUE);
}

bool GetLong(uint32_t* addr)
{
	uint32_t     value;
	ord_state       ok;

	if (!HaveEquals((tokcontrol)0)) return(FALSE);
	ok = getatol(&value);
	if (ok != ST_IS_ORDINAL)
	{
		return(FALSE);
	}
	else
	{
		*addr = value;
	}
	return(TRUE);
}

ord_state getatol(uint32_t* pnt)
{
	char* p;
	unsigned        len;
	//unsigned long   value;
	uint32_t         value;
	unsigned        radix;
	bool            isvalid;
	bool            isdig;
	bool            gotdigit;
	char            ch;

	len = Token.len;
	if (len == 0)
		return(ST_NOT_ORDINAL);
	p = Token.this1;
	gotdigit = FALSE;
	value = 0ul;
	radix = 10;
	if (len >= 2 && *p == '0')
	{
		--len;
		++p;
		if (tolower(*p) == 'x')
		{
			radix = 16;
			++p;
			--len;
		}
	}
	for (; len != 0; --len)
	{
		ch = tolower(*p++);
		if (ch == 'k')
		{         // constant of the form 64k
			if (len > 1 || !gotdigit)
			{
				return(ST_NOT_ORDINAL);
			}
			else
			{
				value <<= 10;        // value = value * 1024;
			}
		}
		else if (ch == 'm')
		{        // constant of the form 64M
			if (len > 1 || !gotdigit)
			{
				return(ST_NOT_ORDINAL);
			}
			else
			{
				value <<= 20;
			}
		}
		else
		{
			isdig = isdigit(ch);
			if (radix == 10)
			{
				isvalid = isdig;
			}
			else
			{
				isvalid = isxdigit(ch);
			}
			if (!isvalid)
			{
				return(ST_NOT_ORDINAL);
			}
			value *= radix;
			if (isdig)
			{
				value += ch - '0';
			}
			else
			{
				value += ch - 'a' + 10;
			}
			gotdigit = TRUE;
		}
	}
	*pnt = value;
	return(ST_IS_ORDINAL);
}
