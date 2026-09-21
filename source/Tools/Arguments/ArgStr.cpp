#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

void argStrResetFn(void* parent_) {
	ArgStrPtr parent = (ArgStrPtr)parent_;
	int i;

	for (i = 0; i < parent->count; i++) {
		parent->sval[i] = "";
	}
	parent->count = 0;
}

int argStrScanFn(void* parent_, const char* argval) {
	ArgStrPtr parent = (ArgStrPtr)parent_;
	int errorcode = 0;

	if (parent->count == parent->hdr.maxcount) {
		errorcode = ARG_ERR_MAXCOUNT;
	}
	else if (!argval) {
		parent->count++;
	}
	else {
		parent->sval[parent->count++] = argval;
	}

	return errorcode;
}

int argStrCheckFn(void* parent_) {
	ArgStrPtr parent = (ArgStrPtr)parent_;
	int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
	return errorcode;
}

void argStrErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname) {
	ArgStrPtr parent = (ArgStrPtr)parent_;
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
	}
}

ArgStrPtr argStr0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
	return argStrN(shortopts, longopts, datatype, 0, 1, glossary);
}

ArgStrPtr argStr1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
	return argStrN(shortopts, longopts, datatype, 1, 1, glossary);
}

ArgStrPtr argStrN(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary) {
	maxcount = (maxcount < mincount) ? mincount : maxcount;
	ArgStrPtr result = (ArgStrPtr)AllocateMemory(sizeof(ArgStr));
	if (result) {
		result->hdr.flag = ARG_HASVALUE;
		result->hdr.shortopts = shortopts;
		result->hdr.longopts = longopts;
		result->hdr.datatype = datatype ? datatype : "<string>";
		result->hdr.glossary = glossary;
		result->hdr.mincount = mincount;
		result->hdr.maxcount = maxcount;
		result->hdr.parent = result;
		result->hdr.resetfn = argStrResetFn;
		result->hdr.scanfn = argStrScanFn;
		result->hdr.checkfn = argStrCheckFn;
		result->hdr.errorfn = argStrErrorFn;

		result->sval = (const char**)AllocateMemory(sizeof(const char*)*maxcount);
		result->count = 0;

		for (int i = 0; i < maxcount; i++) {
			result->sval[i] = "";
		}
	}
	return result;
}
