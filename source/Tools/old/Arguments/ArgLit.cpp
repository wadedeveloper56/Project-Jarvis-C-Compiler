#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

void argLitResetFn(void* parent_) {
	ArgLitPtr parent = (ArgLitPtr)parent_;
	parent->count = 0;
}

int argLitScanFn(void* parent_, const char* argval) {
	ArgLitPtr parent = (ArgLitPtr)parent_;
	int errorcode = 0;
	if (parent->count < parent->hdr.maxcount)
		parent->count++;
	else
		errorcode = ARG_ERR_MAXCOUNT;
	return errorcode;
}

int argLitCheckFn(void* parent_) {
	ArgLitPtr parent = (ArgLitPtr)parent_;
	int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
	return errorcode;
}

void argLitErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname) {
	ArgLitPtr parent = (ArgLitPtr)parent_;
	const char* shortopts = parent->hdr.shortopts;
	const char* longopts = parent->hdr.longopts;
	const char* datatype = parent->hdr.datatype;

	switch (errorcode) {
		case ARG_ERR_MINCOUNT:
			argDstrCatF(ds, "%s: missing option ", progname);
			argPrintOptionDs(ds, shortopts, longopts, datatype, "\n");
			argDstrCat(ds, "\n");
			break;

		case ARG_ERR_MAXCOUNT:
			argDstrCatF(ds, "%s: extraneous option ", progname);
			argPrintOptionDs(ds, shortopts, longopts, datatype, "\n");
			break;
	}
}

ArgLitPtr argLitN(const char* shortopts, const char* longopts, int mincount, int maxcount, const char* glossary) {
	maxcount = (maxcount < mincount) ? mincount : maxcount;
	ArgLitPtr result = (ArgLitPtr)AllocateMemory(sizeof(ArgLit));
	if (result) {
		result->hdr.flag = 0;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts = longopts;
		result->hdr.datatype = NULL;
		result->hdr.glossary = glossary;
		result->hdr.mincount = mincount;
		result->hdr.maxcount = maxcount;
		result->hdr.parent = result;
		result->hdr.resetfn = argLitResetFn;
		result->hdr.scanfn = argLitScanFn;
		result->hdr.checkfn = argLitCheckFn;
		result->hdr.errorfn = argLitErrorFn;
		result->count = 0;
	}
	return result;
}

ArgLitPtr argLit0(const char* shortopts, const char* longopts, const char* glossary) {
	return argLitN(shortopts, longopts, 0, 1, glossary);
}

ArgLitPtr argLit1(const char* shortopts, const char* longopts, const char* glossary) {
	return argLitN(shortopts, longopts, 1, 1, glossary);
}
