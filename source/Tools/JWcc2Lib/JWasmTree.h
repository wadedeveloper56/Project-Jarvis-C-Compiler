#pragma once

#include <vector>

using namespace std;

typedef enum _AsmDataType
{
	ASM_DATA_TYPE_NONE,
	ASM_DATA_TYPE_INSTRUCTION,
	ASM_DATA_TYPE_DIRECTIVE,
	ASM_DATA_TYPE_COMMENT,
	ASM_DATA_TYPE_GLOBAL_DATA,
	ASM_DATA_TYPE_LOCAL_DATA,
	ASM_DATA_TYPE_PARAMETER_DATA
} AsmDataType;

typedef enum _AsmRegisters
{
	NONE,
	//Registers 16 - and 32 - bit Modes
	//8 - bit registers	
	AL,
	CL,
	DL,
	BL,
	AH,
	CH,
	DH,
	BH,
	//16 - bit registers	
	AX,
	CX,
	DX,
	BX,
	SP,
	BP,
	SI,
	DI,
	//32 - bit registers	
	EAX,
	ECX,
	EDX,
	EBX,
	ESP,
	EBP,
	ESI,
	EDI,
	//Segment registers	
	ES,
	CS,
	SS,
	DS,
	FS,
	GS,
	//Floating - point registers	
	ST,
	ST1,
	ST2,
	ST3,
	ST4,
	ST5,
	ST6,
	ST7,
	//MMX registers	
	MM0,
	MM1,
	MM2,
	MM3,
	MM4,
	MM5,
	MM6,
	MM7,
	//SSE registers	
	XMM0,
	XMM1,
	XMM2,
	XMM3,
	XMM4,
	XMM5,
	XMM6,
	XMM7,
	//AVX registers	
	YMM0,
	YMM1,
	YMM2,
	YMM3,
	YMM4,
	YMM5,
	YMM6,
	YMM7,
	//Control registers	
	CR0,
	CR2,
	CR3,
	CR4,
	//Debug registers	
	DR0,
	DR1,
	DR2,
	DR3,
	DR6,
	DR7,
	//Test registers[1]				
	TR3,
	TR4,
	TR5,
	TR6,
	TR7,
	//Additional Registers in 64 - bit Mode
	//8 - bit registers				
	SPL,
	BPL,
	SIL,
	DIL,
	R8B,
	R9B,
	R10B,
	R11B,
	R12B,
	R13B,
	R14B,
	R15B,
	//16 - bit registers	
	R8W,
	R9W,
	R10W,
	R11W,
	R12W,
	R13W,
	R14W,
	R15W,
	//32 - bit registers	
	R8D,
	R9D,
	R10D,
	R11D,
	R12D,
	R13D,
	R14D,
	R15D,
	//64 - bit registers	
	RAX,
	RCX,
	RDX,
	RBX,
	RSP,
	RBP,
	RSI,
	RDI,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,
	//SSE registers	
	XMM8,
	XMM9,
	XMM10,
	XMM11,
	XMM12,
	XMM13,
	XMM14,
	XMM15,
	//AVX registers	
	YMM8,
	YMM9,
	YMM10,
	YMM11,
	YMM12,
	YMM13,
	YMM14,
	YMM15,
	//Control registers	
	CR8
} AsmRegisters;

typedef enum _AsmOperandType
{
	OPERAND_NONE,
	OPERAND_MEMORY,
	OPERAND_REGISTER,
	OPERAND_IMMEDIATE
}AsmOperandType;

typedef struct _AsmOperand
{
	AsmOperandType type;
	AsmRegisters registers;
	string variable;
	long long immediate;
	_AsmOperand() {};
	_AsmOperand(AsmOperandType t, AsmRegisters r, string v, long long i) : type(t), registers(r), variable(v), immediate(i) {};
} AsmOperand;

typedef enum _AsmDirective
{
	NONE1
	,p8086
	, p186
	, p286
	, p286C
	, p286P
	, p386
	, p386C
	, p386P
	, p486
	, p486P
	, p586
	, p586P
	, p686
	, p686P
	, pK3D
	, pMMX
	, pXMM
	, pX64
	, pX64P
	, p8087
	, p287
	, p387
	, pNO87
	, pCREF
	, pLIST
	, pLISTALL
	, pLISTIF
	, pLFCOND
	, pNOCREF
	, pXCREF
	, pNOLIST
	, pXLIST
	, pNOLISTIF
	, pSFCOND
	, pTFCOND
	, PAGE
	, SUBTITLE
	, SUBTTL
	, TITLE
	, pLISTMACRO
	, pXALL
	, pLISTMACROALL
	, pLALL
	, pNOLISTMACRO
	, pSALL
	, pALPHA
	, pDOSSEG
	, DOSSEG
	, pSEQ
	, pCODE
	, pSTACK
	, pDATA
	, pDATAQ
	, pFARDATA
	, pFARDATAQ
	, pCONST
	, pIF
	, pREPEAT
	, pWHILE
	, pBREAK
	, pCONTINUE
	, pELSE
	, pELSEIF
	, pENDIF
	, pENDW
	, pUNTIL
	, pUNTILCXZ
	, pEXIT
	, pSTARTUP
	, pMODEL
	, pRADIX
	, pSAFESEH
	, pERR
	, pERR1
	, pERR2
	, pERRE
	, pERRNZ
	, pERRDIF
	, pERRDIFI
	, pERRIDN
	, pERRIDNI
	, pERRB
	, pERRNB
	, pERRDEF
	, pERRNDEF
	, COMMENT
	, IF
	, IFE
	, IF1
	, IF2
	, IFDIF
	, IFDIFI
	, IFIDN
	, IFIDNI
	, IFB
	, IFNB
	, IFDEF
	, IFNDEF
	, ELSE
	, ELSEIF
	, ELSEIFE
	, ELSEIF1
	, ELSEIF2
	, ELSEIFDIF
	, ELSEIFDIFI
	, ELSEIFIDN
	, ELSEIFIDNI
	, ELSEIFB
	, ELSEIFNB
	, ELSEIFDEF
	, ELSEIFNDEF
	, ENDIF
	, FOR
	, IRP
	, FORC
	, IRPC
	, REPEAT
	, REPT
	, WHILE
	, MACRO
	, EXITM
	, ENDM
	, GOTO
	, PURGE
	, INCLUDE
	, TEXTEQU
	, CATSTR
	, SUBSTR
	, INSTR
	, SIZESTR
	, DB
	, DW
	, DD
	, DF
	, DQ
	, DT
	, STRUCT
	, STRUC
	, UNION
	, TYPEDEF
	, RECORD
	, COMM
	, EXTERN
	, EXTRN
	, EXTERNDEF
	, PUBLIC
	, PROTO
	, PROC
	, ENDP
	, LOCAL
	, LABEL
	, INVOKE
	, ORG
	, ALIGN
	, EVEN
	, SEGMENT
	, ENDS
	, GROUP
	, ASSUME
	, ALIAS
	, ECHO
	, pOUT
	, END
	, EQU
	, INCBIN
	, INCLUDELIB
	, NAME
	, OPTION
	, POPCONTEXT
	, PUSHCONTEXT
}AsmDirective;

typedef struct _AsmInstruction
{
	string label;
	string mnemonic;
	AsmOperand operand1;
	AsmOperand operand2;
	string comment;
	_AsmInstruction() {};
}AsmInstruction;

typedef struct _AsmDirectiveData
{
	AsmDirective directive;
	vector<string> directiveData;
	string comment;
}AsmDirectiveData;

typedef struct _AsmData
{
	string label;
	string type;
	string value;
	string comment;
	_AsmData() {};
	_AsmData(string l, string t, string v, string c) : label(l), type(t), value(v), comment(c) {};
} AsmData;

typedef struct _AsmParameterData
{
	string type;
	string value;
	_AsmParameterData() {};
	_AsmParameterData(string t, string v) : type(t), value(v) {};
} AsmParameterData;

typedef struct _JWasmData
{
	AsmDataType type;
	AsmDirectiveData directive;
	AsmInstruction instruction;
	AsmData global;
	AsmParameterData parameter;
	AsmParameterData local;
	string comment;
	_JWasmData() {};
	_JWasmData(AsmDataType t, AsmDirectiveData d, AsmInstruction i, AsmData g, AsmParameterData p, AsmParameterData l, string c) : type(t), directive(d), instruction(i), global(g), parameter(p), local(l), comment(c) {};
} JWasmData;

class JWasmTree
{
	vector<JWasmData> program;
public:
	JWasmTree();
	~JWasmTree();
	void addInstruction(string instr, AsmOperand op1, AsmOperand op2, string comment);
	void addDirective(AsmDirective directive, string data, string comment);
	void addDirective(AsmDirective directive, string data1, string data2, string comment);
	void addComment(string comment);
	void addGlobalData(string label, string type, string value, string comment);
	void addLocalData(string type, string value);
	void addParameterData(string type, string value);
};

