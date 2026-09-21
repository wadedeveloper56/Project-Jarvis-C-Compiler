#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

extern char* optarg;
extern int optind, opterr, optopt;

void argFreeTable(void** argtable, size_t n) {
	ArgHdrPtrPtr table = (ArgHdrPtrPtr)argtable;
	size_t tabindex = 0;
	for (tabindex = 0; tabindex < n; tabindex++) {
		if (table[tabindex] == NULL)
			continue;
		free(table[tabindex]);
		table[tabindex] = NULL;
	};
}

ArgRemPtr arg_rem(const char* datatype, const char* glossary) {
	ArgRemPtr result = (ArgRemPtr)malloc(sizeof(ArgRem));
	if (result) {
		result->hdr.flag = 0;
		result->hdr.shortopts = NULL;
		result->hdr.longopts = NULL;
		result->hdr.datatype = datatype;
		result->hdr.glossary = glossary;
		result->hdr.mincount = 1;
		result->hdr.maxcount = 1;
		result->hdr.parent = result;
		result->hdr.resetfn = NULL;
		result->hdr.scanfn = NULL;
		result->hdr.checkfn = NULL;
		result->hdr.errorfn = NULL;
	}
	return result;
}

int argNullCheck(void** argtable) {
	ArgHdrPtrPtr table = (ArgHdrPtrPtr)argtable;
	int tabindex;
	if (!table)
		return 1;
	tabindex = 0;
	do {
		if (!table[tabindex])
			return 1;
	} while (!(table[tabindex++]->flag & ARG_TERMINATOR));

	return 0;
}

void arg_reset(void** argtable) {
	ArgHdrPtrPtr table = (ArgHdrPtrPtr)argtable;
	int tabindex = 0;
	do {
		if (table[tabindex]->resetfn)
			table[tabindex]->resetfn(table[tabindex]->parent);
	} while (!(table[tabindex++]->flag & ARG_TERMINATOR));
}

int arg_endindex(ArgHdrPtrPtr table) {
	int tabindex = 0;
	while (!(table[tabindex]->flag & ARG_TERMINATOR))
		tabindex++;
	return tabindex;
}

void arg_register_error(ArgEndPtr end, void* parent, int error, const char* argval) {
	if (end->count < end->hdr.maxcount) {
		end->error[end->count] = error;
		end->parent[end->count] = parent;
		end->argval[end->count] = argval;
		end->count++;
	}
	else {
		end->error[end->hdr.maxcount - 1] = ARG_ELIMIT;
		end->parent[end->hdr.maxcount - 1] = end;
		end->argval[end->hdr.maxcount - 1] = NULL;
	}
}

void arg_parse_check(ArgHdrPtrPtr table, ArgEndPtr endtable) {
	int tabindex = 0;
	do {
		if (table[tabindex]->checkfn) {
			void* parent = table[tabindex]->parent;
			int errorcode = table[tabindex]->checkfn(parent);
			if (errorcode != 0)
				arg_register_error(endtable, parent, errorcode, NULL);
		}
	} while (!(table[tabindex++]->flag & ARG_TERMINATOR));
}

LongOptionsPtr alloc_longoptions(ArgHdrPtrPtr table) {
	LongOptionsPtr result;
	size_t nbytes;
	int noptions = 1;
	size_t longoptlen = 0;
	int tabindex;
	int option_index = 0;
	char* store;

	tabindex = 0;
	do {
		const char* longopts = table[tabindex]->longopts;
		longoptlen += (longopts ? strlen(longopts) : 0) + 1;
		while (longopts) {
			noptions++;
			longopts = strchr(longopts + 1, ',');
		}
	} while (!(table[tabindex++]->flag & ARG_TERMINATOR));
	nbytes = sizeof(LongOptions) + sizeof(Option) * (size_t)noptions + longoptlen;
	result = (LongOptionsPtr)malloc(nbytes);

	result->getoptval = 0;
	result->noptions = noptions;
	result->options = (OptionPtr)(result + 1);
	store = (char*)(result->options + noptions);

	for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		const char* longopts = table[tabindex]->longopts;

		while (longopts && *longopts) {
			char* storestart = store;

			while (*longopts != 0 && *longopts != ',')
				*store++ = *longopts++;
			*store++ = 0;
			if (*longopts == ',')
				longopts++;
			result->options[option_index].name = storestart;
			result->options[option_index].flag = &(result->getoptval);
			result->options[option_index].val = tabindex;
			if (table[tabindex]->flag & ARG_HASOPTVALUE)
				result->options[option_index].has_arg = 2;
			else if (table[tabindex]->flag & ARG_HASVALUE)
				result->options[option_index].has_arg = 1;
			else
				result->options[option_index].has_arg = 0;

			option_index++;
		}
	}
	result->options[option_index].name = 0;
	result->options[option_index].has_arg = 0;
	result->options[option_index].flag = 0;
	result->options[option_index].val = 0;

	return result;
}

char* alloc_shortoptions(ArgHdrPtrPtr table) {
	char* result;
	size_t len = 2;
	int tabindex;
	char* res;

	for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		ArgHdrPtr hdr = table[tabindex];
		len += 3 * (hdr->shortopts ? strlen(hdr->shortopts) : 0);
	}

	result = (char *)malloc(len);

	res = result;

	*res++ = ':';

	for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		ArgHdrPtr hdr = table[tabindex];
		const char* shortopts = hdr->shortopts;
		while (shortopts && *shortopts) {
			*res++ = *shortopts++;
			if (hdr->flag & ARG_HASVALUE)
				*res++ = ':';
			if (hdr->flag & ARG_HASOPTVALUE)
				*res++ = ':';
		}
	}
	*res = 0;

	return result;
}

int find_shortoption(ArgHdrPtrPtr table, char shortopt) {
	int tabindex;
	for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		if (table[tabindex]->shortopts && strchr(table[tabindex]->shortopts, shortopt))
			return tabindex;
	}
	return -1;
}

void arg_parse_untagged(int argc, char** argv, ArgHdrPtrPtr table, ArgEndPtr endtable) {
	int tabindex = 0;
	int errorlast = 0;
	const char* optarglast = NULL;
	void* parentlast = NULL;

	while (!(table[tabindex]->flag & ARG_TERMINATOR)) {
		void* parent;
		int errorcode;

		if (optind >= argc) {
			return;
		}

		if (table[tabindex]->longopts || table[tabindex]->shortopts) {
			tabindex++;
			continue;
		}

		if (!(table[tabindex]->scanfn)) {
			tabindex++;
			continue;
		}

		parent = table[tabindex]->parent;
		errorcode = table[tabindex]->scanfn(parent, argv[optind]);
		if (errorcode == 0) {
			optind++;

			errorlast = 0;
		}
		else {
			tabindex++;

			errorlast = errorcode;
			optarglast = argv[optind];
			parentlast = parent;
		}
	}

	if (errorlast) {
		arg_register_error(endtable, parentlast, errorlast, optarglast);
		optind++;
	}

	while (optind < argc) {
		arg_register_error(endtable, endtable, ARG_ENOMATCH, argv[optind++]);
	}

	return;
}

void arg_parse_tagged(int argc, char** argv, ArgHdrPtrPtr table, ArgEndPtr endtable) {
	LongOptionsPtr longoptions;
	char* shortoptions;
	int copt;

	longoptions = alloc_longoptions(table);
	shortoptions = alloc_shortoptions(table);

	optind = 0;
	opterr = 0;

	while ((copt = getOptLong(argc, argv, shortoptions, longoptions->options, NULL)) != -1) {
		switch (copt) {
			case 0: {
				int tabindex = longoptions->getoptval;
				void* parent = table[tabindex]->parent;
				if (optarg && optarg[0] == 0 && (table[tabindex]->flag & ARG_HASVALUE)) {
					arg_register_error(endtable, endtable, ARG_EMISSARG, argv[optind - 1]);
				}
				if (table[tabindex]->scanfn) {
					int errorcode = table[tabindex]->scanfn(parent, optarg);
					if (errorcode != 0)
						arg_register_error(endtable, parent, errorcode, optarg);
				}
			} break;

			case '?':
				switch (optopt) {
					case 0:
						arg_register_error(endtable, endtable, ARG_ELONGOPT, argv[optind - 1]);
						break;
					default:
						arg_register_error(endtable, endtable, optopt, NULL);
						break;
				}
				break;

			case ':':
				arg_register_error(endtable, endtable, ARG_EMISSARG, argv[optind - 1]);
				break;

			default: {
				int tabindex = find_shortoption(table, (char)copt);
				if (tabindex == -1) {
					arg_register_error(endtable, endtable, copt, NULL);
				}
				else {
					if (table[tabindex]->scanfn) {
						void* parent = table[tabindex]->parent;
						int errorcode = table[tabindex]->scanfn(parent, optarg);
						if (errorcode != 0)
							arg_register_error(endtable, parent, errorcode, optarg);
					}
				}
				break;
			}
		}
	}

	free(shortoptions);
	free(longoptions);
}

int argParse(int argc, char** argv, void** argtable) {
	ArgHdrPtrPtr table = (ArgHdrPtrPtr)argtable;
	ArgEndPtr endtable;
	int endindex;
	char** argvcopy = NULL;
	int i;

	arg_reset(argtable);
	endindex = arg_endindex(table);
	endtable = (ArgEndPtr)table[endindex];
	if (argc == 0) {
		arg_parse_check(table, endtable);
		return endtable->count;
	}
	argvcopy = (char**)malloc(sizeof(char*) * (size_t)(argc + 1));
	for (i = 0; i < argc; i++)
		argvcopy[i] = argv[i];
	argvcopy[argc] = NULL;
	arg_parse_tagged(argc, argvcopy, table, endtable);
	arg_parse_untagged(argc, argvcopy, table, endtable);
	if (endtable->count == 0)
		arg_parse_check(table, endtable);
	free(argvcopy);
	return endtable->count;
}

void arg_print_gnuswitch_ds(ArgDstrPtr ds, ArgHdrPtrPtr table) {
	int tabindex;
	const char* format1 = " -%c";
	const char* format2 = " [-%c";
	const char* suffix = "";

	for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		if (table[tabindex]->mincount < 1)
			continue;

		if (table[tabindex]->shortopts == NULL)
			continue;

		if (table[tabindex]->flag & ARG_HASVALUE)
			continue;

		argDstrCatF(ds, format1, table[tabindex]->shortopts[0]);
		format1 = "%c";
		format2 = "[%c";
	}

	for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		if (table[tabindex]->mincount > 0)
			continue;

		if (table[tabindex]->shortopts == NULL)
			continue;

		if (table[tabindex]->flag & ARG_HASVALUE)
			continue;

		argDstrCatF(ds, format2, table[tabindex]->shortopts[0]);
		format2 = "%c";
		suffix = "]";
	}

	argDstrCatF(ds, "%s", suffix);
}

void arg_cat_option(char* dest, size_t ndest, const char* shortopts, const char* longopts, const char* datatype, int optvalue) {
	if (shortopts) {
		char option[3];

		option[0] = '-';
		option[1] = shortopts[0];
		option[2] = 0;

		argCat(&dest, option, &ndest);
		if (datatype) {
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
	else if (longopts) {
		size_t ncspn;

		argCat(&dest, "--", &ndest);

		ncspn = strcspn(longopts, ",");
#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
		strncat_s(dest, ndest, longopts, (ncspn < ndest) ? ncspn : ndest);
#else
		strncat(dest, longopts, (ncspn < ndest) ? ncspn : ndest);
#endif

		if (datatype) {
			argCat(&dest, "=", &ndest);
			if (optvalue) {
				argCat(&dest, "[", &ndest);
				argCat(&dest, datatype, &ndest);
				argCat(&dest, "]", &ndest);
			}
			else
				argCat(&dest, datatype, &ndest);
		}
	}
	else if (datatype) {
		if (optvalue) {
			argCat(&dest, "[", &ndest);
			argCat(&dest, datatype, &ndest);
			argCat(&dest, "]", &ndest);
		}
		else
			argCat(&dest, datatype, &ndest);
	}
}


void arg_print_syntax_ds(ArgDstrPtr ds, void** argtable, const char* suffix) {
	ArgHdrPtrPtr table = (ArgHdrPtrPtr)argtable;
	int i, tabindex;

	arg_print_gnuswitch_ds(ds, table);

	for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		char syntax[200] = "";
		const char* shortopts, * longopts, * datatype;

		if (table[tabindex]->shortopts && !(table[tabindex]->flag & ARG_HASVALUE))
			continue;

		shortopts = table[tabindex]->shortopts;
		longopts = table[tabindex]->longopts;
		datatype = table[tabindex]->datatype;
		arg_cat_option(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, table[tabindex]->flag & ARG_HASOPTVALUE);

		if (strlen(syntax) > 0) {
			for (i = 0; i < table[tabindex]->mincount; i++) {
				argDstrCat(ds, " ");
				argDstrCat(ds, syntax);
			}

			switch (table[tabindex]->maxcount - table[tabindex]->mincount) {
				case 0:
					break;
				case 1:
					argDstrCat(ds, " [");
					argDstrCat(ds, syntax);
					argDstrCat(ds, "]");
					break;
				case 2:
					argDstrCat(ds, " [");
					argDstrCat(ds, syntax);
					argDstrCat(ds, "]");
					argDstrCat(ds, " [");
					argDstrCat(ds, syntax);
					argDstrCat(ds, "]");
					break;
				default:
					argDstrCat(ds, " [");
					argDstrCat(ds, syntax);
					argDstrCat(ds, "]...");
					break;
			}
		}
	}

	if (suffix) {
		argDstrCat(ds, (char*)suffix);
	}
}

void argPrintSyntax(FILE* fp, void** argtable, const char* suffix) {
	ArgDstrPtr ds = argDstrCreate();
	arg_print_syntax_ds(ds, argtable, suffix);
	fputs(argDstrCStr(ds), fp);
	argDstrDestroy(ds);
}

void arg_print_glossary_ds(ArgDstrPtr ds, void** argtable, const char* format) {
	ArgHdrPtrPtr table = (ArgHdrPtrPtr)argtable;
	int tabindex;

	format = format ? format : "  %-20s %s\n";
	for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
		if (table[tabindex]->glossary) {
			char syntax[200] = "";
			const char* shortopts = table[tabindex]->shortopts;
			const char* longopts = table[tabindex]->longopts;
			const char* datatype = table[tabindex]->datatype;
			const char* glossary = table[tabindex]->glossary;
			argCatOptionV(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, table[tabindex]->flag & ARG_HASOPTVALUE, ", ");
			argDstrCatF(ds, format, syntax, glossary);
		}
	}
}

void argPrintGlossary(FILE* fp, void** argtable, const char* format) {
	ArgDstrPtr ds = argDstrCreate();
	arg_print_glossary_ds(ds, argtable, format);
	fputs(argDstrCStr(ds), fp);
	argDstrDestroy(ds);
}

