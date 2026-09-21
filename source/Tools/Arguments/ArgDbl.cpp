#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

void argDblResetFn(void* parent_) {
	ArgDblPtr parent = (ArgDblPtr)parent_;
	if (parent) parent->count = 0;
}

int argDblScanFn(void* parent_, const char* argval) {
	ArgDblPtr parent = (ArgDblPtr)parent_;
	int errorcode = 0;

	if (parent->count == parent->hdr.maxcount) {
		errorcode = ARG_ERR_MAXCOUNT;
	}
	else if (!argval) {
		parent->count++;
	}
	else {
		double val;
		char* end;

		val = strtod(argval, &end);

		if (*end == 0)
			parent->dval[parent->count++] = val;
		else
			errorcode = ARG_ERR_BADDOUBLE;
	}

	return errorcode;
}

int argDblCheckFn(void* parent_) {
	ArgDblPtr parent = (ArgDblPtr)parent_;
	int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
	return errorcode;
}

void argDblErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname) {
	ArgDblPtr parent = (ArgDblPtr)parent_;
	const char* shortopts = parent->hdr.shortopts;
	const char* longopts = parent->hdr.longopts;
	const char* datatype = parent->hdr.datatype;

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

	case ARG_ERR_BADDOUBLE:
		argDstrCatF(ds, "invalid argument \"%s\" to option ", argval);
		argPrintOptionDs(ds, shortopts, longopts, datatype, "\n");
		break;
	}
}

ArgDblPtr argDbl0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
	return argDblN(shortopts, longopts, datatype, 0, 1, glossary);
}

ArgDblPtr argDbl1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
	return argDblN(shortopts, longopts, datatype, 1, 1, glossary);
}

ArgDblPtr argDblN(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary) {
	maxcount = (maxcount < mincount) ? mincount : maxcount;
	ArgDblPtr result = (ArgDblPtr)AllocateMemory(sizeof(ArgDbl));
	if (result) {
		result->hdr.flag = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts = longopts;
		result->hdr.datatype = datatype ? datatype : "<double>";
		result->hdr.glossary = glossary;
		result->hdr.mincount = mincount;
		result->hdr.maxcount = maxcount;
		result->hdr.parent = result;
		result->hdr.resetfn = argDblResetFn;
		result->hdr.scanfn = argDblScanFn;
		result->hdr.checkfn = argDblCheckFn;
		result->hdr.errorfn = argDblErrorFn;
		result->dval = (double*)AllocateMemory(sizeof(double)*maxcount);
		result->count = 0;
	}
	return result;
}
