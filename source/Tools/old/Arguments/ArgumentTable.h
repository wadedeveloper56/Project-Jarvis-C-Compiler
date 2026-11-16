#ifndef ARGUMENTTABLE
#define ARGUMENTTABLE

#define ARGUMENTS_API

#define ARG_REX_ICASE 1
#define ARG_CMD_NAME_LEN 100
#define ARG_CMD_DESCRIPTION_LEN 256
#define START_VSNBUFF 16
#define ARG_DSTR_SIZE 200
#define ARG_DSTR_STATIC ((ArgDstrFreeFunctionPtr)0)
#define ARG_DSTR_VOLATILE ((ArgDstrFreeFunctionPtr)1)
#define ARG_DSTR_DYNAMIC ((ArgDstrFreeFunctionPtr)3)
#define ALT_E 0x01
#define ALT_O 0x02
#define LEGAL_ALT(x)           \
    {                          \
        if (alt_format & ~(x)) \
            return (0);        \
    }
#define TM_YEAR_BASE (1900)
#define TRexChar char
#define MAX_CHAR 0xFF
#define _TREXC(c) (c)
#define trex_strlen strlen
#define trex_printf printf

#define TRex_True 1
#define TRex_False 0

#define TREX_ICASE ARG_REX_ICASE
#define OP_GREEDY (MAX_CHAR + 1)        
#define OP_OR (MAX_CHAR + 2)
#define OP_EXPR (MAX_CHAR + 3)          
#define OP_NOCAPEXPR (MAX_CHAR + 4)     
#define OP_DOT (MAX_CHAR + 5)
#define OP_CLASS (MAX_CHAR + 6)
#define OP_CCLASS (MAX_CHAR + 7)
#define OP_NCLASS (MAX_CHAR + 8)       
#define OP_RANGE (MAX_CHAR + 9)
#define OP_CHAR (MAX_CHAR + 10)
#define OP_EOL (MAX_CHAR + 11)
#define OP_BOL (MAX_CHAR + 12)
#define OP_WB (MAX_CHAR + 13)

#define TREX_SYMBOL_ANY_CHAR ('.')
#define TREX_SYMBOL_GREEDY_ONE_OR_MORE ('+')
#define TREX_SYMBOL_GREEDY_ZERO_OR_MORE ('*')
#define TREX_SYMBOL_GREEDY_ZERO_OR_ONE ('?')
#define TREX_SYMBOL_BRANCH ('|')
#define TREX_SYMBOL_END_OF_STRING ('$')
#define TREX_SYMBOL_BEGINNING_OF_STRING ('^')
#define TREX_SYMBOL_ESCAPE_CHAR ('\\')
#define no_argument        0
#define required_argument  1
#define optional_argument  2

	enum { ARG_ERR_MINCOUNT = 1, ARG_ERR_MAXCOUNT, ARG_ERR_BADINT, ARG_ERR_OVERFLOW, ARG_ERR_BADDOUBLE, ARG_ERR_BADDATE, ARG_ERR_REGNOMATCH };

	enum {
		ARG_ELIMIT = 1,
		ARG_EMALLOC,
		ARG_ENOMATCH,
		ARG_ELONGOPT,
		ARG_EMISSARG
	};

	enum arg_hdr_flag {
		ARG_TERMINATOR = 0x1,
		ARG_HASVALUE = 0x2,
		ARG_HASOPTVALUE = 0x4
	};

	struct _ArgDstr;

	typedef ARGUMENTS_API void(*ArgResetFunctionPtr)(void* parent);
	typedef ARGUMENTS_API int(*ArgScanFunctionPtr)(void* parent, const char* argval);
	typedef ARGUMENTS_API int(*ArgCheckInFunctionPtr)(void* parent);
	typedef ARGUMENTS_API void(*ArgErrorFunctionPtr)(void* parent, struct _ArgDstr* ds, int error, const char* argval, const char* progname);
	typedef ARGUMENTS_API void(*ArgDstrFreeFunctionPtr)(char* buf);
	typedef ARGUMENTS_API int(*ArgCmdFunctionPtr)(int argc, char* argv[], struct _ArgDstr* res, void* ctx);
	typedef ARGUMENTS_API int(*ArgCompareFunctionPtr)(const void* k1, const void* k2);

	typedef struct ARGUMENTS_API _Option {
		const char* name;
		int has_arg;
		int* flag;
		int val;
	}Option, * OptionPtr, ** OptionPtrPtr;

	typedef struct ARGUMENTS_API _LongOptions {
		int getoptval;
		int noptions;
		OptionPtr options;
	}LongOptions, * LongOptionsPtr, ** LongOptionsPtrPtr;

	typedef struct ARGUMENTS_API _PrivHdr {
		const char* pattern;
		int flags;
	}PrivHdr, * PrivHdrPtr, ** PrivHdrPtrPtr;

	typedef struct ARGUMENTS_API _ArgHdr {
		char flag;
		const char* shortopts;
		const char* longopts;
		const char* datatype;
		const char* glossary;
		int mincount;
		int maxcount;
		void* parent;
		ArgResetFunctionPtr resetfn;
		ArgScanFunctionPtr scanfn;
		ArgCheckInFunctionPtr checkfn;
		ArgErrorFunctionPtr errorfn;
		void* priv;
	} ArgHdr, * ArgHdrPtr, ** ArgHdrPtrPtr;

	typedef struct ARGUMENTS_API _ArgRem {
		ArgHdr hdr;
	} ArgRem, * ArgRemPtr;

	typedef struct ARGUMENTS_API _ArgLit {
		ArgHdr hdr;
		int count;
	} ArgLit, * ArgLitPtr, ** ArgLitPtrPtr;

	typedef struct ARGUMENTS_API _ArgInt {
		ArgHdr hdr;
		int count;
		int* ival;
	} ArgInt, * ArgIntPtr, ** ArgIntPtrPtr;

	typedef struct ARGUMENTS_API _ArgDbl {
		ArgHdr hdr;
		int count;
		double* dval;
	} ArgDbl, * ArgDblPtr, ** ArgDblPtrPtr;

	typedef struct ARGUMENTS_API _ArgStr {
		ArgHdr hdr;
		int count;
		const char** sval;
	} ArgStr, * ArgStrPtr, ** ArgStrPtrPtr;

	typedef struct ARGUMENTS_API _ArgRex {
		ArgHdr hdr;
		int count;
		const char** sval;
	} ArgRex, * ArgRexPtr, ** ArgRexPtrPtr;

	typedef struct ARGUMENTS_API _ArgFile {
		ArgHdr hdr;
		int count;
		const char** filename;
		const char** basename;
		const char** extension;
	} ArgFile, * ArgFilePtr, ** ArgFilePtrPtr;

	typedef struct ARGUMENTS_API _ArgDate {
		ArgHdr hdr;
		const char* format;
		int count;
		struct tm* tmval;
	}ArgDate, * ArgDatePtr, ** ArgDatePtrPtr;

	typedef struct ARGUMENTS_API _ArgEnd {
		ArgHdr hdr;
		int count;
		int* error;
		void** parent;
		const char** argval;
	} ArgEnd, * ArgEndPtr, ** ArgEndPtrPtr;

	typedef struct ARGUMENTS_API _ArgCmdInfo {
		char name[ARG_CMD_NAME_LEN];
		char description[ARG_CMD_DESCRIPTION_LEN];
		ArgCmdFunctionPtr proc;
		void* ctx;
	} ArgCmdInfo, * ArgCmdInfoPtr, ** ArgCmdInfoPtrPtr;

	typedef struct ARGUMENTS_API _ArgDstr {
		char* data;
		ArgDstrFreeFunctionPtr free_proc;
		char sbuf[ARG_DSTR_SIZE + 1];
		char* append_data;
		int append_data_size;
		int append_used;
	} ArgDstr, * ArgDstrPtr, ** ArgDstrPtrPtr;

	typedef ARGUMENTS_API void* arg_cmd_itr_t;

	typedef ARGUMENTS_API int TRexNodeType;
	typedef ARGUMENTS_API unsigned int TRexBool;
	extern ARGUMENTS_API char* optarg;
	extern ARGUMENTS_API int optind, opterr, optopt;

	typedef struct ARGUMENTS_API _TRexMatch {
		const TRexChar* begin;
		int len;
	} TRexMatch, * TRexMatchPtr, ** TRexMatchPtrPtr;

	typedef struct ARGUMENTS_API _TRexNode {
		TRexNodeType type;
		int left;
		int right;
		int next;
	} TRexNode, * TRexNodePtr, ** TRexNodePtrPtr;

	typedef struct ARGUMENTS_API _TRex {
		const TRexChar* _eol;
		const TRexChar* _bol;
		const TRexChar* _p;
		int _first;
		int _op;
		TRexNode* _nodes;
		int _nallocated;
		int _nsize;
		int _nsubexpr;
		TRexMatchPtr _matches;
		int _currsubexp;
		void* _jmpbuf;
		const TRexChar** _error;
		int _flags;
	}TRex, * TRexPtr, ** TRexPtrPtr;

#ifdef __cplusplus
	extern "C" {
#endif

	// ArgGetOpt.cpp
	ARGUMENTS_API void warnx(const char* fmt, ...);
	ARGUMENTS_API void permuteArgs(int panonopt_start, int panonopt_end, int opt_end, char* const* nargv);
	ARGUMENTS_API int parseLongOptions(char* const* nargv, const char* options, const OptionPtr long_options, int* idx, int short_too, int flags);
	ARGUMENTS_API int getOptInternal(int nargc, char* const* nargv, const char* options, const OptionPtr long_options, int* idx, int flags);
	ARGUMENTS_API int getOpt(int nargc, char* const* nargv, const char* options);
	ARGUMENTS_API int getOptLong(int nargc, char* const* nargv, const char* options, const OptionPtr long_options, int* idx);
	ARGUMENTS_API int getOptLongOnly(int nargc, char* const* nargv, const char* options, const OptionPtr long_options, int* idx);
	ARGUMENTS_API void argCat(char** pdest, const char* src, size_t* pndest);
	ARGUMENTS_API void argCatOptionV(char* dest, size_t ndest, const char* shortopts, const char* longopts, const char* datatype, int optvalue, const char* separator);
	ARGUMENTS_API void argPrintOptionDs(ArgDstrPtr ds, const char* shortopts, const char* longopts, const char* datatype, const char* suffix);
	
	//ArdDate.cpp
	ARGUMENTS_API void argDateResetFn(void* parent_);
	ARGUMENTS_API int argDateScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argDateCheckFn(void* parent_);
	ARGUMENTS_API void argDateErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgDatePtr argDate0(const char* shortopts, const char* longopts, const char* format, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgDatePtr argDate1(const char* shortopts, const char* longopts, const char* format, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgDatePtr argDateN(const char* shortopts, const char* longopts, const char* format, const char* datatype, int mincount, int maxcount, const char* glossary);;
	ARGUMENTS_API int argStrCaseCmp(const char* s1, const char* s2);
	ARGUMENTS_API int argStrNCaseCmp(const char* s1, const char* s2, size_t n);
	ARGUMENTS_API char* argStrPTime(const char* buf, const char* fmt, struct tm* tm);
	ARGUMENTS_API int convNum(const char** buf, int* dest, int llim, int ulim);

	//ArgDbl.cpp
	ARGUMENTS_API void argDblResetFn(void* parent_);
	ARGUMENTS_API int argDblScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argDblCheckFn(void* parent_);
	ARGUMENTS_API void argDblErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgDblPtr argDbl0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgDblPtr argDbl1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgDblPtr argDblN(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

	//ArgDstr.cpp
	ARGUMENTS_API char* argDstrCStr(ArgDstrPtr ds);
	ARGUMENTS_API void argDstrReset(ArgDstrPtr ds);
	ARGUMENTS_API ArgDstrPtr argDstrCreate(void);
	ARGUMENTS_API void argDstrDestroy(ArgDstrPtr ds);
	ARGUMENTS_API void argDstrFree(ArgDstrPtr ds);
	ARGUMENTS_API void setupAppendBuf(ArgDstrPtr ds, int new_space);
	ARGUMENTS_API void argDstrCat(ArgDstrPtr ds, const char* str);
	ARGUMENTS_API void argDstrCatF(ArgDstrPtr ds, const char* fmt, ...);

	//ArgEnd.cpp
	ARGUMENTS_API void argEndResetFn(void* parent_);
	ARGUMENTS_API void argEndErrorFn(void* parent, ArgDstrPtr ds, int error, const char* argval, const char* progname);
	ARGUMENTS_API ArgEndPtr argEnd(int maxcount);
	ARGUMENTS_API void argPrintErrorsDs(ArgDstrPtr ds, ArgEndPtr end, const char* progname);
	ARGUMENTS_API void argPrintErrors(FILE* fp, ArgEndPtr end, const char* progname);

	//ArgFile.cpp
	ARGUMENTS_API void argFileResetFn(void* parent_);
	ARGUMENTS_API const char* argBasename(const char* filename);
	ARGUMENTS_API const char* argExtension(const char* basename);
	ARGUMENTS_API int argFileScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argFileCheckFn(void* parent_);
	ARGUMENTS_API void argFileErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgFilePtr argFile0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgFilePtr argFile1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgFilePtr argFileN(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

	//ArgInt.cpp
	ARGUMENTS_API void argIntResetFn(void* parent_);
	ARGUMENTS_API long int strtol0X(const char* str, const char** endptr, char X, int base);
	ARGUMENTS_API int detectsuffix(const char* str, const char* suffix);
	ARGUMENTS_API int argIntScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argIntCheckFn(void* parent_);
	ARGUMENTS_API void argIntErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgIntPtr argIntN(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);
	ARGUMENTS_API ArgIntPtr argInt0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgIntPtr argInt1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);

	//ArgLit.cpp
	ARGUMENTS_API void argLitResetFn(void* parent_);
	ARGUMENTS_API int argLitScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argLitCheckFn(void* parent_);
	ARGUMENTS_API void argLitErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgLitPtr argLitN(const char* shortopts, const char* longopts, int mincount, int maxcount, const char* glossary);
	ARGUMENTS_API ArgLitPtr argLit0(const char* shortopts, const char* longopts, const char* glossary);
	ARGUMENTS_API ArgLitPtr argLit1(const char* shortopts, const char* longopts, const char* glossary);

	//ArgRex.cpp
	ARGUMENTS_API void argRexResetFn(void* parent_);
	ARGUMENTS_API int argRexScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argRexCheckFn(void* parent_);
	ARGUMENTS_API void argRexErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgRexPtr argRex0(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary);
	ARGUMENTS_API ArgRexPtr argRex1(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary);
	ARGUMENTS_API ArgRexPtr argRexN(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int mincount, int maxcount, int flags, const char* glossary);
	ARGUMENTS_API int trex_newnode(TRex* exp, TRexNodeType type);
	ARGUMENTS_API void trex_error(TRex* exp, const TRexChar* error);
	ARGUMENTS_API void trex_expect(TRex* exp, int n);
	ARGUMENTS_API TRexChar trex_escapechar(TRex* exp);
	ARGUMENTS_API int trex_charclass(TRex* exp, int classid);
	ARGUMENTS_API int trex_charnode(TRex* exp, TRexBool isclass);
	ARGUMENTS_API int trex_class(TRex* exp);
	ARGUMENTS_API int trex_parsenumber(TRex* exp);
	ARGUMENTS_API int trex_element(TRex* exp);
	ARGUMENTS_API int trex_list(TRex* exp);
	ARGUMENTS_API TRexBool trex_matchcclass(int cclass, TRexChar c);
	ARGUMENTS_API TRexBool trex_matchclass(TRex* exp, TRexNode* node, TRexChar c);
	ARGUMENTS_API const TRexChar* trex_matchnode(TRex* exp, TRexNode* node, const TRexChar* str, TRexNode* next);
	ARGUMENTS_API TRex* trex_compile(const TRexChar* pattern, const TRexChar** error, int flags);
	ARGUMENTS_API void trex_free(TRex* exp);
	ARGUMENTS_API TRexBool trex_match(TRex* exp, const TRexChar* text);
	ARGUMENTS_API TRexBool trex_searchrange(TRex* exp, const TRexChar* text_begin, const TRexChar* text_end, const TRexChar** out_begin, const TRexChar** out_end);
	ARGUMENTS_API TRexBool trex_search(TRex* exp, const TRexChar* text, const TRexChar** out_begin, const TRexChar** out_end);
	ARGUMENTS_API int trex_getsubexpcount(TRex* exp);
	ARGUMENTS_API TRexBool trex_getsubexp(TRex* exp, int n, TRexMatch* subexp);

	ARGUMENTS_API void argStrResetFn(void* parent_);
	ARGUMENTS_API int argStrScanFn(void* parent_, const char* argval);
	ARGUMENTS_API int argStrCheckFn(void* parent_);
	ARGUMENTS_API void argStrErrorFn(void* parent_, ArgDstrPtr ds, int errorcode, const char* argval, const char* progname);
	ARGUMENTS_API ArgStrPtr argStr0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgStrPtr argStr1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
	ARGUMENTS_API ArgStrPtr argStrN(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

	ARGUMENTS_API void argFreeTable(void** argtable, size_t n);
	ARGUMENTS_API ArgRemPtr arg_rem(const char* datatype, const char* glossary);
	ARGUMENTS_API int argNullCheck(void** argtable);
	ARGUMENTS_API void arg_reset(void** argtable);
	ARGUMENTS_API int arg_endindex(ArgHdrPtrPtr table);
	ARGUMENTS_API void arg_register_error(ArgEndPtr end, void* parent, int error, const char* argval);
	ARGUMENTS_API void arg_parse_check(ArgHdrPtrPtr table, ArgEndPtr endtable);
	ARGUMENTS_API LongOptionsPtr alloc_longoptions(ArgHdrPtrPtr table);
	ARGUMENTS_API char* alloc_shortoptions(ArgHdrPtrPtr table);
	ARGUMENTS_API int find_shortoption(ArgHdrPtrPtr table, char shortopt);
	ARGUMENTS_API void arg_parse_untagged(int argc, char** argv, ArgHdrPtrPtr table, ArgEndPtr endtable);
	ARGUMENTS_API void arg_parse_tagged(int argc, char** argv, ArgHdrPtrPtr table, ArgEndPtr endtable);
	ARGUMENTS_API int argParse(int argc, char** argv, void** argtable);
	ARGUMENTS_API void arg_print_gnuswitch_ds(ArgDstrPtr ds, ArgHdrPtrPtr table);
	ARGUMENTS_API void arg_cat_option(char* dest, size_t ndest, const char* shortopts, const char* longopts, const char* datatype, int optvalue);
	ARGUMENTS_API void arg_print_syntax_ds(ArgDstrPtr ds, void** argtable, const char* suffix);
	ARGUMENTS_API void argPrintSyntax(FILE* fp, void** argtable, const char* suffix);
	ARGUMENTS_API void arg_print_glossary_ds(ArgDstrPtr ds, void** argtable, const char* format);
	ARGUMENTS_API void argPrintGlossary(FILE* fp, void** argtable, const char* format);
#ifdef __cplusplus
}
#endif
#endif
