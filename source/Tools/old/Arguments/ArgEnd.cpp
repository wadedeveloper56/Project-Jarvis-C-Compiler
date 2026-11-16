#include "pch.h"
#include "framework.h"
#include "ArgumentTable.h"

void argEndResetFn(void* parent_) {
    ArgEndPtr parent = (ArgEndPtr)parent_;
    parent->count = 0;
}

void argEndErrorFn(void* parent, ArgDstrPtr ds, int error, const char* argval, const char* progname) {
    (void)parent;

    progname = progname ? progname : "";
    argval = argval ? argval : "";

    argDstrCatF(ds, "%s: ", progname);
    switch (error) {
        case ARG_ELIMIT:
            argDstrCat(ds, "too many errors to display");
            break;
        case ARG_EMALLOC:
            argDstrCat(ds, "insufficient memory");
            break;
        case ARG_ENOMATCH:
            argDstrCatF(ds, "unexpected argument \"%s\"", argval);
            break;
        case ARG_EMISSARG:
            argDstrCatF(ds, "option \"%s\" requires an argument", argval);
            break;
        case ARG_ELONGOPT:
            argDstrCatF(ds, "invalid option \"%s\"", argval);
            break;
        default:
            argDstrCatF(ds, "invalid option \"-%c\"", error);
            break;
    }

    argDstrCat(ds, "\n");
}

ArgEndPtr argEnd(int maxcount) {
    size_t nbytes;
    ArgEndPtr result;

    nbytes = sizeof(ArgEnd) + (size_t)maxcount * sizeof(int)
        + (size_t)maxcount * sizeof(void*)                            
        + (size_t)maxcount * sizeof(char*);                           

    result = (ArgEndPtr)malloc(nbytes);
    if (result) {
        result->hdr.flag = ARG_TERMINATOR;
        result->hdr.shortopts = NULL;
        result->hdr.longopts = NULL;
        result->hdr.datatype = NULL;
        result->hdr.glossary = NULL;
        result->hdr.mincount = 1;
        result->hdr.maxcount = maxcount;
        result->hdr.parent = result;
        result->hdr.resetfn = argEndResetFn;
        result->hdr.scanfn = NULL;
        result->hdr.checkfn = NULL;
        result->hdr.errorfn = argEndErrorFn;
        result->error = (int*)(result + 1);
        result->parent = (void**)(result->error + maxcount);
        result->argval = (const char**)(result->parent + maxcount);
    }
    return result;
}

void argPrintErrorsDs(ArgDstrPtr ds, ArgEndPtr end, const char* progname) {
    int i;
    for (i = 0; i < end->count; i++) {
        ArgHdrPtr errorparent = (ArgHdrPtr)(end->parent[i]);
        if (errorparent->errorfn)
            errorparent->errorfn(end->parent[i], ds, end->error[i], end->argval[i], progname);
    }
}

void argPrintErrors(FILE* fp, ArgEndPtr end, const char* progname) {
    ArgDstrPtr ds = argDstrCreate();
    argPrintErrorsDs(ds, end, progname);
    fputs(argDstrCStr(ds), fp);
    argDstrDestroy(ds);
}
