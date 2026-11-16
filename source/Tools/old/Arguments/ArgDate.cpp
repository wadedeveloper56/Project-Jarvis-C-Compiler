#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

const char* day[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* abday[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char* mon[12] = { "January", "February", "March",     "April",   "May",      "June", "July",    "August",   "September", "October", "November", "December" };
const char* abmon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* am_pm[2] = { "AM", "PM" };

void argDateResetFn(void* parent_) {
	ArgDatePtr parent = (ArgDatePtr)parent_;
	parent->count = 0;
}

int argDateScanFn(void* parent_, const char* argval) {
	ArgDatePtr parent = (ArgDatePtr)parent_;
	int errorcode = 0;

	if (parent->count == parent->hdr.maxcount) {
		errorcode = ARG_ERR_MAXCOUNT;
	}
	else if (!argval) {
		parent->count++;
	}
	else {
		const char* pend;
		struct tm tm = parent->tmval[parent->count];

		pend = argStrPTime(argval, parent->format, &tm);
		if (pend && pend[0] == '\0')
			parent->tmval[parent->count++] = tm;
		else
			errorcode = ARG_ERR_BADDATE;
	}
	return errorcode;
}

int argDateCheckFn(void* parent_) {
	ArgDatePtr parent = (ArgDatePtr)parent_;
	int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
	return errorcode;
}

void argDateErrorFn(void* parent, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname) {
	ArgDatePtr p = (ArgDatePtr)parent;
	const char* shortopts = p->hdr.shortopts;
	const char* longopts = p->hdr.longopts;
	const char* datatype = p->hdr.datatype;

	argval = argval ? argval : "";

	argDstrCatF(ds, "%s: ", progname);
	switch (errorcode) {
		case ARG_ERR_MINCOUNT:
			argDstrCat(ds, "missing option ");
			argPrintOptionDs(ds, shortopts, longopts, datatype, "\n");
			break;

		case ARG_ERR_MAXCOUNT:
			argDstrCat(ds, "excess option ");
			argPrintOptionDs(ds, shortopts, longopts, argval, "\n");
			break;

		case ARG_ERR_BADDATE: {
			struct tm tm;
			char buff[200];

			argDstrCatF(ds, "illegal timestamp format \"%s\"\n", argval);
			memset(&tm, 0, sizeof(tm));
			argStrPTime("1999-12-31 23:59:59", "%F %H:%M:%S", &tm);
			strftime(buff, sizeof(buff), p->format, &tm);
			argDstrCatF(ds, "correct format is \"%s\"\n", buff);
			break;
		}
	}
}

ArgDatePtr argDate0(const char* shortopts, const char* longopts, const char* format, const char* datatype, const char* glossary) {
	return argDateN(shortopts, longopts, format, datatype, 0, 1, glossary);
}

ArgDatePtr argDate1(const char* shortopts, const char* longopts, const char* format, const char* datatype, const char* glossary) {
	return argDateN(shortopts, longopts, format, datatype, 1, 1, glossary);
}

ArgDatePtr argDateN(const char* shortopts, const char* longopts, const char* format, const char* datatype, int mincount, int maxcount, const char* glossary) {
	maxcount = (maxcount < mincount) ? mincount : maxcount;
	if (!format) format = "%x";
	ArgDatePtr result = (ArgDatePtr)AllocateMemory(sizeof(ArgDate));
	if (result) {
		result->hdr.flag = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts = longopts;
		result->hdr.datatype = datatype ? datatype : format;
		result->hdr.glossary = glossary;
		result->hdr.mincount = mincount;
		result->hdr.maxcount = maxcount;
		result->hdr.parent = result;
		result->hdr.resetfn = argDateResetFn;
		result->hdr.scanfn = argDateScanFn;
		result->hdr.checkfn = argDateCheckFn;
		result->hdr.errorfn = argDateErrorFn;
		result->tmval = (struct tm*)AllocateMemory(sizeof(struct tm)*maxcount);
		result->count = 0;
		result->format = format;
	}
	return result;
}

int argStrCaseCmp(const char* s1, const char* s2) {
	const unsigned char* us1 = (const unsigned char*)s1;
	const unsigned char* us2 = (const unsigned char*)s2;
	while (tolower(*us1) == tolower(*us2++))
		if (*us1++ == '\0')
			return 0;

	return tolower(*us1) - tolower(*--us2);
}

int argStrNCaseCmp(const char* s1, const char* s2, size_t n) {
	if (n != 0) {
		const unsigned char* us1 = (const unsigned char*)s1;
		const unsigned char* us2 = (const unsigned char*)s2;
		do {
			if (tolower(*us1) != tolower(*us2++))
				return tolower(*us1) - tolower(*--us2);

			if (*us1++ == '\0')
				break;
		} while (--n != 0);
	}

	return 0;
}

char* argStrPTime(const char* buf, const char* fmt, struct tm* tm) {
	char c;
	const char* bp;
	size_t len = 0;
	int alt_format, i, split_year = 0;

	bp = buf;

	while ((c = *fmt) != '\0') {
		alt_format = 0;

		if (isspace(c)) {
			while (isspace((int)(*bp)))
				bp++;

			fmt++;
			continue;
		}

		if ((c = *fmt++) != '%')
			goto literal;

	again:
		switch (c = *fmt++) {
			case '%':
			literal:
				if (c != *bp++)
					return (0);
				break;

			case 'E':
				LEGAL_ALT(0);
				alt_format |= ALT_E;
				goto again;

			case 'O':
				LEGAL_ALT(0);
				alt_format |= ALT_O;
				goto again;

			case 'c':
				LEGAL_ALT(ALT_E);
				bp = argStrPTime(bp, "%x %X", tm);
				if (!bp)
					return (0);
				break;

			case 'D':
				LEGAL_ALT(0);
				bp = argStrPTime(bp, "%m/%d/%y", tm);
				if (!bp)
					return (0);
				break;

			case 'R':
				LEGAL_ALT(0);
				bp = argStrPTime(bp, "%H:%M", tm);
				if (!bp)
					return (0);
				break;

			case 'r':
				LEGAL_ALT(0);
				bp = argStrPTime(bp, "%I:%M:%S %p", tm);
				if (!bp)
					return (0);
				break;

			case 'T':
				LEGAL_ALT(0);
				bp = argStrPTime(bp, "%H:%M:%S", tm);
				if (!bp)
					return (0);
				break;

			case 'X':
				LEGAL_ALT(ALT_E);
				bp = argStrPTime(bp, "%H:%M:%S", tm);
				if (!bp)
					return (0);
				break;

			case 'x':
				LEGAL_ALT(ALT_E);
				bp = argStrPTime(bp, "%m/%d/%y", tm);
				if (!bp)
					return (0);
				break;

			case 'A':
			case 'a':
				LEGAL_ALT(0);
				for (i = 0; i < 7; i++) {
					len = strlen(day[i]);
					if (argStrNCaseCmp(day[i], bp, len) == 0)
						break;

					len = strlen(abday[i]);
					if (argStrNCaseCmp(abday[i], bp, len) == 0)
						break;
				}

				if (i == 7)
					return (0);

				tm->tm_wday = i;
				bp += len;
				break;

			case 'B':
			case 'b':
			case 'h':
				LEGAL_ALT(0);
				for (i = 0; i < 12; i++) {
					len = strlen(mon[i]);
					if (argStrNCaseCmp(mon[i], bp, len) == 0)
						break;

					len = strlen(abmon[i]);
					if (argStrNCaseCmp(abmon[i], bp, len) == 0)
						break;
				}

				if (i == 12)
					return (0);

				tm->tm_mon = i;
				bp += len;
				break;

			case 'C':
				LEGAL_ALT(ALT_E);
				if (!(convNum(&bp, &i, 0, 99)))
					return (0);

				if (split_year) {
					tm->tm_year = (tm->tm_year % 100) + (i * 100);
				}
				else {
					tm->tm_year = i * 100;
					split_year = 1;
				}
				break;

			case 'd':
			case 'e':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &tm->tm_mday, 1, 31)))
					return (0);
				break;

			case 'k':
				LEGAL_ALT(0);
			case 'H':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &tm->tm_hour, 0, 23)))
					return (0);
				break;

			case 'l':
				LEGAL_ALT(0);
			case 'I':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &tm->tm_hour, 1, 12)))
					return (0);
				if (tm->tm_hour == 12)
					tm->tm_hour = 0;
				break;

			case 'j':
				LEGAL_ALT(0);
				if (!(convNum(&bp, &i, 1, 366)))
					return (0);
				tm->tm_yday = i - 1;
				break;

			case 'M':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &tm->tm_min, 0, 59)))
					return (0);
				break;

			case 'm':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &i, 1, 12)))
					return (0);
				tm->tm_mon = i - 1;
				break;

			case 'p':
				LEGAL_ALT(0);
				if (argStrCaseCmp(am_pm[0], bp) == 0) {
					if (tm->tm_hour > 11)
						return (0);

					bp += strlen(am_pm[0]);
					break;
				}
				else if (argStrCaseCmp(am_pm[1], bp) == 0) {
					if (tm->tm_hour > 11)
						return (0);

					tm->tm_hour += 12;
					bp += strlen(am_pm[1]);
					break;
				}

				return (0);

			case 'S':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &tm->tm_sec, 0, 61)))
					return (0);
				break;

			case 'U':
			case 'W':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &i, 0, 53)))
					return (0);
				break;

			case 'w':
				LEGAL_ALT(ALT_O);
				if (!(convNum(&bp, &tm->tm_wday, 0, 6)))
					return (0);
				break;

			case 'Y':
				LEGAL_ALT(ALT_E);
				if (!(convNum(&bp, &i, 0, 9999)))
					return (0);

				tm->tm_year = i - TM_YEAR_BASE;
				break;

			case 'y':
				LEGAL_ALT(ALT_E | ALT_O);
				if (!(convNum(&bp, &i, 0, 99)))
					return (0);

				if (split_year) {
					tm->tm_year = ((tm->tm_year / 100) * 100) + i;
					break;
				}
				split_year = 1;
				if (i <= 68)
					tm->tm_year = i + 2000 - TM_YEAR_BASE;
				else
					tm->tm_year = i + 1900 - TM_YEAR_BASE;
				break;

			case 'n':
			case 't':
				LEGAL_ALT(0);
				while (isspace((int)(*bp)))
					bp++;
				break;

			default:
				return (0);
		}
	}

	return ((char*)bp);
}

int convNum(const char** buf, int* dest, int llim, int ulim) {
	int result = 0;

	int rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	do {
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}

