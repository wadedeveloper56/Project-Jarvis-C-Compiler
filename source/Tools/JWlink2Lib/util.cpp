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