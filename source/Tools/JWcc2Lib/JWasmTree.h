#pragma once

#include <vector>

using namespace std;

typedef enum _AsmDataType
{
	ASM_DATA_TYPE_NONE,
	ASM_DATA_TYPE_INIT_DATA,
	ASM_DATA_TYPE_UNINIT_DATA,
	ASM_DATA_TYPE_METHOD,
	ASM_DATA_DIRECTIVE,
	ASM_DATA_TYPE_COMMENT
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
	OPERAND_IMMEDIATE,
	OPERAND_LABEL,
	OPERAND_MACRO,
}AsmOperandType;

typedef enum _AsmDirective
{
	NONE1
	,ASSEMBLER
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

typedef struct _AsmOperand
{
	AsmOperandType type;
	AsmRegisters registers;
	string variable;
	long long immediate;
	_AsmOperand() : type(OPERAND_NONE), registers(AsmRegisters::NONE), variable(""), immediate(0LL) {};
	_AsmOperand(AsmOperandType t, AsmRegisters r, string v, long long i) : type(t), registers(r), variable(v), immediate(i) {};
} AsmOperand;

typedef struct _AsmInstruction
{
	AsmOperandType type;
	string mnemonic;
	AsmOperand op1;
	AsmOperand op2;
	vector<string> macro;
	_AsmInstruction() : type(OPERAND_NONE), mnemonic(""), op1(), op2() {};
	_AsmInstruction(string m, AsmOperand o1, AsmOperand o2) : type(OPERAND_NONE), mnemonic(m), op1(o1), op2(o2) {};
} AsmInstruction;

typedef struct _AsmData
{
	string label;
	string type;
	string value;
	string comment;
	_AsmData() : label(""), type(""), value(""), comment("") {};
	_AsmData(string l, string t, string v, string c) : label(l), type(t), value(v), comment(c) {};
} AsmData;

typedef struct _AsmMethod
{
	string name;
	vector<pair<string, string>> parameters; // pair of type and name
	vector<pair<string, string>> locals; // pair of type and name
	vector<AsmInstruction> instructions;
	_AsmMethod() : name(""), parameters(), locals(), instructions() {};
	_AsmMethod(string n, vector<pair<string, string>> p, vector<pair<string, string>> l, vector<AsmInstruction> i) : name(n), parameters(p), locals(l), instructions(i) {};
} AsmMethod;

typedef struct _AsmDirectiveData
{
	AsmDirective directive;
	vector<string> directiveData;
	_AsmDirectiveData() : directive(AsmDirective::NONE1), directiveData() {};
	_AsmDirectiveData(AsmDirective d, vector<string> dd) : directive(d), directiveData(dd) {};
} AsmDirectiveData;

typedef struct _JWasmData
{
	AsmDataType type;
	AsmDirectiveData directive;
	vector<AsmData> initData;
	vector<AsmData> uninitData;
	vector<AsmMethod> methods;
	string comment;
	_JWasmData() : type(ASM_DATA_TYPE_NONE), directive(), initData(), uninitData(), methods() {};
	_JWasmData(AsmDataType t, AsmDirectiveData d, vector<AsmData> id, vector<AsmData> ud, vector<AsmMethod> m) : type(t), directive(d), initData(id), uninitData(ud), methods(m) {};
} JWasmData;

class JWasmTree
{
	vector<JWasmData> program;
public:
	JWasmTree();
	~JWasmTree();
	void addMethod(AsmMethod method);
	void addDirective(AsmDirectiveData directive);
	void addInitData(AsmData initData);
	void addUninitData(AsmData uninitData);
	void addComment(string comment);	vector<JWasmData>& getProgram() { return program; }
};

