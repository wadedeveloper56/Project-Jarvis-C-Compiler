#include "pch.h"
#include "JWasmTree.h"

JWasmTree::JWasmTree() {}

JWasmTree::~JWasmTree() {}

void JWasmTree::addInstruction(string instr, AsmOperand op1, AsmOperand op2, string comment)
{
	JWasmData instruction;
	instruction.type = AsmDataType::ASM_DATA_TYPE_INSTRUCTION;
	instruction.instruction.mnemonic = instr;
	instruction.instruction.operand1 = op1;
	instruction.instruction.operand2 = op2;
	instruction.instruction.comment = comment;
	program.push_back(instruction);
}

void JWasmTree::addDirective(AsmDirective directive, string data)
{
	JWasmData directiveData;
	directiveData.type = AsmDataType::ASM_DATA_TYPE_DIRECTIVE;
	directiveData.directive.directive = directive;
	directiveData.directive.directiveData.push_back(data);
	program.push_back(directiveData);
}

void JWasmTree::addDirective(AsmDirective directive, string data1, string data2)
{
	JWasmData directiveData;
	directiveData.type = AsmDataType::ASM_DATA_TYPE_DIRECTIVE;
	directiveData.directive.directive = directive;
	directiveData.directive.directiveData.push_back(data1);
	directiveData.directive.directiveData.push_back(data2);
	program.push_back(directiveData);
}

void JWasmTree::addComment(string comment)
{
	JWasmData commentData;
	commentData.type = AsmDataType::ASM_DATA_TYPE_COMMENT;
	commentData.comment = comment;
	program.push_back(commentData);
}

void JWasmTree::addData(string label, string type, string value, string comment)
{
	JWasmData dataEntry;
	dataEntry.type = AsmDataType::ASM_DATA_TYPE_DATA;
	dataEntry.data.label = label;
	dataEntry.data.type = type;
	dataEntry.data.value = value;
	dataEntry.data.comment = comment;
	program.push_back(dataEntry);
}
