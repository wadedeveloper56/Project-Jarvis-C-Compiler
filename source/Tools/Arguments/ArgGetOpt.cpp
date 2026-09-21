#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

#define GNU_COMPATIBLE		       
int	opterr = 1;
int	optind = 1;
int	optopt = '?';
int	optreset;
char* optarg;
#define PRINT_ERROR	((opterr) && (*options != ':'))
#define FLAG_PERMUTE	0x01	        
#define FLAG_ALLARGS	0x02	        
#define FLAG_LONGONLY	0x04	    
#define	BADCH		(int)'?'
#define	BADARG		((*options == ':') ? (int)':' : (int)'?')
#define	INORDER 	(int)1
#define	EMSG		""
#ifdef GNU_COMPATIBLE
#define NO_PREFIX	(-1)
#define D_PREFIX	0
#define DD_PREFIX	1
#define W_PREFIX	2
#endif
#define MAX_OPTERRMSG_SIZE 128

char* place = (char *)EMSG;
int nonopt_start = -1;
int nonopt_end = -1;
const char recargchar[] = "option requires an argument -- %c";
const char illoptchar[] = "illegal option -- %c";
int dash_prefix = NO_PREFIX;
const char gnuoptchar[] = "invalid option -- %c";
const char recargstring[] = "option `%s%s' requires an argument";
const char ambig[] = "option `%s%.*s' is ambiguous";
const char noarg[] = "option `%s%.*s' doesn't allow an argument";
const char illoptstring[] = "unrecognized option `%s%s'";
extern char opterrmsg[MAX_OPTERRMSG_SIZE];
char opterrmsg[MAX_OPTERRMSG_SIZE];


void warnx(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	memset(opterrmsg, 0, sizeof(opterrmsg));
	if (fmt != NULL)
	{
		_vsnprintf(opterrmsg, sizeof(opterrmsg) - 1, fmt, ap);
	}
	va_end(ap);
	fprintf(stderr, "%s\n", opterrmsg);
}

int gcd(int a, int b) {
	int c;

	c = a % b;
	while (c != 0) {
		a = b;
		b = c;
		c = a % b;
	}

	return (b);
}

void permuteArgs(int panonopt_start, int panonopt_end, int opt_end, char* const* nargv)
{
	int cstart, cyclelen, i, j, ncycle, nnonopts, nopts, pos;
	char* swap;

	nnonopts = panonopt_end - panonopt_start;
	nopts = opt_end - panonopt_end;
	ncycle = gcd(nnonopts, nopts);
	cyclelen = (opt_end - panonopt_start) / ncycle;

	for (i = 0; i < ncycle; i++) {
		cstart = panonopt_end + i;
		pos = cstart;
		for (j = 0; j < cyclelen; j++) {
			if (pos >= panonopt_end)
				pos -= nnonopts;
			else
				pos += nopts;
			swap = nargv[pos];
			((char**)nargv)[pos] = nargv[cstart];
			((char**)nargv)[cstart] = swap;
		}
	}
}

int parseLongOptions(char* const* nargv, const char* options, const OptionPtr long_options, int* idx, int short_too, int flags) {
	char* current_argv, * has_equal;
	char* current_dash;
	size_t current_argv_len;
	int i, match, exact_match, second_partial_match;

	current_argv = place;
	switch (dash_prefix) {
		case D_PREFIX:
			current_dash = (char *)"-";
			break;
		case DD_PREFIX:
			current_dash = (char*)"--";
			break;
		case W_PREFIX:
			current_dash = (char*)"-W ";
			break;
		default:
			current_dash = (char*)"";
			break;
	}
	match = -1;
	exact_match = 0;
	second_partial_match = 0;

	optind++;

	if ((has_equal = strchr(current_argv, '=')) != NULL) {
		current_argv_len = (size_t)(has_equal - current_argv);
		has_equal++;
	}
	else
		current_argv_len = strlen(current_argv);

	for (i = 0; long_options[i].name; i++) {
		if (strncmp(current_argv, long_options[i].name,
			current_argv_len))
			continue;

		if (strlen(long_options[i].name) == current_argv_len) {
			match = i;
			exact_match = 1;
			break;
		}
		if (short_too && current_argv_len == 1)
			continue;

		if (match == -1)
			match = i;
		else if ((flags & FLAG_LONGONLY) ||
			long_options[i].has_arg !=
			long_options[match].has_arg ||
			long_options[i].flag != long_options[match].flag ||
			long_options[i].val != long_options[match].val)
			second_partial_match = 1;
	}
	if (!exact_match && second_partial_match) {
		if (PRINT_ERROR)
			warnx(ambig,
#ifdef GNU_COMPATIBLE
				current_dash,
#endif
				(int)current_argv_len,
				current_argv);
		optopt = 0;
		return (BADCH);
	}
	if (match != -1) {
		if (long_options[match].has_arg == no_argument
			&& has_equal) {
			if (PRINT_ERROR)
				warnx(noarg,
#ifdef GNU_COMPATIBLE
					current_dash,
#endif
					(int)current_argv_len,
					current_argv);
			if (long_options[match].flag == NULL)
				optopt = long_options[match].val;
			else
				optopt = 0;
#ifdef GNU_COMPATIBLE
			return (BADCH);
#else
			return (BADARG);
#endif
		}
		if (long_options[match].has_arg == required_argument ||
			long_options[match].has_arg == optional_argument) {
			if (has_equal)
				optarg = has_equal;
			else if (long_options[match].has_arg ==
				required_argument) {
				optarg = nargv[optind++];
			}
		}
		if ((long_options[match].has_arg == required_argument)
			&& (optarg == NULL)) {
			if (PRINT_ERROR)
				warnx(recargstring,
#ifdef GNU_COMPATIBLE
					current_dash,
#endif
					current_argv);
			if (long_options[match].flag == NULL)
				optopt = long_options[match].val;
			else
				optopt = 0;
			--optind;
			return (BADARG);
		}
	}
	else {
		if (short_too) {
			--optind;
			return (-1);
		}
		if (PRINT_ERROR)
			warnx(illoptstring,
#ifdef GNU_COMPATIBLE
				current_dash,
#endif
				current_argv);
		optopt = 0;
		return (BADCH);
	}
	if (idx)
		*idx = match;
	if (long_options[match].flag) {
		*long_options[match].flag = long_options[match].val;
		return (0);
	}
	else
		return (long_options[match].val);
}

int getOptInternal(int nargc, char* const* nargv, const char* options, const OptionPtr long_options, int* idx, int flags) {
	char* oli;
	int optchar, short_too;
	int posixly_correct = -1;

	if (options == NULL)
		return (-1);

	if (optind == 0)
		optind = optreset = 1;

	if (posixly_correct == -1 || optreset) {
		size_t requiredSize;
		getenv_s(&requiredSize, NULL, 0, "POSIXLY_CORRECT");
		posixly_correct = requiredSize != 0;
	}

	if (*options == '-')
		flags |= FLAG_ALLARGS;
	else if (posixly_correct || *options == '+')
		flags &= ~FLAG_PERMUTE;
	if (*options == '+' || *options == '-')
		options++;

	optarg = NULL;
	if (optreset)
		nonopt_start = nonopt_end = -1;
start:
	if (optreset || !*place) {
		optreset = 0;
		if (optind >= nargc) {
			place = (char*)EMSG;
			if (nonopt_end != -1) {
				permuteArgs(nonopt_start, nonopt_end,optind, nargv);
				optind -= nonopt_end - nonopt_start;
			}
			else if (nonopt_start != -1) {
				optind = nonopt_start;
			}
			nonopt_start = nonopt_end = -1;
			return (-1);
		}
		if (*(place = nargv[optind]) != '-' ||
#ifdef GNU_COMPATIBLE
			place[1] == '\0') {
#else
			(place[1] == '\0' && strchr(options, '-') == NULL)) {
#endif
			place = (char*)EMSG;
			if (flags & FLAG_ALLARGS) {
				optarg = nargv[optind++];
				return (INORDER);
			}
			if (!(flags & FLAG_PERMUTE)) {
				return (-1);
			}
			if (nonopt_start == -1)
				nonopt_start = optind;
			else if (nonopt_end != -1) {
				permuteArgs(nonopt_start, nonopt_end,
					optind, nargv);
				nonopt_start = optind -
					(nonopt_end - nonopt_start);
				nonopt_end = -1;
			}
			optind++;
			goto start;
		}
		if (nonopt_start != -1 && nonopt_end == -1)
			nonopt_end = optind;

		if (place[1] != '\0' && *++place == '-' && place[1] == '\0') {
			optind++;
			place = (char*)EMSG;
			if (nonopt_end != -1) {
				permuteArgs(nonopt_start, nonopt_end,
					optind, nargv);
				optind -= nonopt_end - nonopt_start;
			}
			nonopt_start = nonopt_end = -1;
			return (-1);
		}
	}

	if (long_options != NULL && place != nargv[optind] &&
		(*place == '-' || (flags & FLAG_LONGONLY))) {
		short_too = 0;
#ifdef GNU_COMPATIBLE
		dash_prefix = D_PREFIX;
#endif
		if (*place == '-') {
			place++;
			if (*place == '\0')
				return (BADARG);
#ifdef GNU_COMPATIBLE
			dash_prefix = DD_PREFIX;
#endif
		}
		else if (*place != ':' && strchr(options, *place) != NULL)
			short_too = 1;

		optchar = parseLongOptions(nargv, options, long_options,
			idx, short_too, flags);
		if (optchar != -1) {
			place = (char*)EMSG;
			return (optchar);
		}
	}

	if ((optchar = (int)*place++) == (int)':' ||
		(optchar == (int)'-' && *place != '\0') ||
		(oli = strchr((char*)options, optchar)) == NULL) {
		if (optchar == (int)'-' && *place == '\0')
			return (-1);
		if (!*place)
			++optind;
#ifdef GNU_COMPATIBLE
		if (PRINT_ERROR)
			warnx(posixly_correct ? illoptchar : gnuoptchar,
				optchar);
#else
		if (PRINT_ERROR)
			warnx(illoptchar, optchar);
#endif
		optopt = optchar;
		return (BADCH);
	}
	if (long_options != NULL && optchar == 'W' && oli[1] == ';') {
		if (*place)
			;
		else if (++optind >= nargc) {
			place = (char*)EMSG;
			if (PRINT_ERROR)
				warnx(recargchar, optchar);
			optopt = optchar;
			return (BADARG);
		}
		else
			place = nargv[optind];
#ifdef GNU_COMPATIBLE
		dash_prefix = W_PREFIX;
#endif
		optchar = parseLongOptions(nargv, options, long_options,
			idx, 0, flags);
		place = (char*)EMSG;
		return (optchar);
	}
	if (*++oli != ':') {
		if (!*place)
			++optind;
	}
	else {
		optarg = NULL;
		if (*place)
			optarg = place;
		else if (oli[1] != ':') {
			if (++optind >= nargc) {
				place = (char*)EMSG;
				if (PRINT_ERROR)
					warnx(recargchar, optchar);
				optopt = optchar;
				return (BADARG);
			}
			else
				optarg = nargv[optind];
		}
		place = (char*)EMSG;
		++optind;
	}
	return (optchar);
}

int getOpt(int nargc, char* const* nargv, const char* options) {
	return (getOptInternal(nargc, nargv, options, NULL, NULL, 0));
}

int getOptLong(int nargc, char* const* nargv, const char* options, const OptionPtr long_options, int* idx) {
	return (getOptInternal(nargc, nargv, options, long_options, idx, FLAG_PERMUTE));
}

int getOptLongOnly(int nargc, char* const* nargv, const char* options, const OptionPtr long_options, int* idx) {
	return (getOptInternal(nargc, nargv, options, long_options, idx, FLAG_PERMUTE | FLAG_LONGONLY));
}

void argCat(char** pdest, const char* src, size_t* pndest) {
	char* dest = *pdest;
	char* end = dest + *pndest;

	while (dest < end - 1 && *dest != 0)
		dest++;

	while (dest < end - 1 && *src != 0)
		*dest++ = *src++;

	*dest = 0;

	*pndest = (size_t)(end - dest);
	*pdest = dest;
}

void argCatOptionV(char* dest, size_t ndest, const char* shortopts, const char* longopts, const char* datatype, int optvalue, const char* separator) {
	separator = separator ? separator : "";

	if (shortopts) {
		const char* c = shortopts;
		while (*c) {
			char shortopt[3];

			shortopt[0] = '-';
			shortopt[1] = *c;
			shortopt[2] = 0;

			argCat(&dest, shortopt, &ndest);
			if (*++c)
				argCat(&dest, separator, &ndest);
		}
	}

	if (shortopts && longopts)
		argCat(&dest, separator, &ndest);

	if (longopts) {
		const char* c = longopts;
		while (*c) {
			size_t ncspn;

			argCat(&dest, "--", &ndest);

			ncspn = strcspn(c, ",");
#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
			strncat_s(dest, ndest, c, (ncspn < ndest) ? ncspn : ndest);
#else
			strncat(dest, c, (ncspn < ndest) ? ncspn : ndest);
#endif
			c += ncspn;

			if (*c == ',') {
				argCat(&dest, separator, &ndest);
				c++;
			}
		}
	}

	if (datatype) {
		if (longopts)
			argCat(&dest, "=", &ndest);
		else if (shortopts)
			argCat(&dest, " ", &ndest);

		if (optvalue) {
			argCat(&dest, "[", &ndest);
			argCat(&dest, datatype, &ndest);
			argCat(&dest, "]", &ndest);
		}
		else
			argCat(&dest, datatype, &ndest);
	}
}

void argPrintOptionDs(ArgDstrPtr ds, const char* shortopts, const char* longopts, const char* datatype, const char* suffix) {
	char syntax[200] = "";
	suffix = suffix ? suffix : "";
	argCatOptionV(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, 0, "|");
	argDstrCat(ds, syntax);
	argDstrCat(ds, (char*)suffix);
}