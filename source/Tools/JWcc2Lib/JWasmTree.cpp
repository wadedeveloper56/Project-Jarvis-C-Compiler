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

void JWasmTree::addDirective(AsmDirective directive, string data, string comment)
{
	JWasmData directiveData;
	directiveData.type = AsmDataType::ASM_DATA_TYPE_DIRECTIVE;
	directiveData.directive.directive = directive;
	directiveData.directive.directiveData.push_back(data);
	directiveData.directive.comment = comment;
	program.push_back(directiveData);
}

void JWasmTree::addDirective(AsmDirective directive, string data1, string data2, string comment)
{
	JWasmData directiveData;
	directiveData.type = AsmDataType::ASM_DATA_TYPE_DIRECTIVE;
	directiveData.directive.directive = directive;
	directiveData.directive.directiveData.push_back(data1);
	directiveData.directive.directiveData.push_back(data2);
	directiveData.directive.comment = comment;
	program.push_back(directiveData);
}

void JWasmTree::addComment(string comment)
{
	JWasmData commentData;
	commentData.type = AsmDataType::ASM_DATA_TYPE_COMMENT;
	commentData.comment = comment;
	program.push_back(commentData);
}

void JWasmTree::addGlobalData(string label, string type, string value, string comment)
{
	JWasmData dataEntry;
	dataEntry.type = AsmDataType::ASM_DATA_TYPE_GLOBAL_DATA;
	dataEntry.global.label = label;
	dataEntry.global.type = type;
	dataEntry.global.value = value;
	dataEntry.global.comment = comment;
	program.push_back(dataEntry);
}

void JWasmTree::addLocalData(string type, string value)
{
	JWasmData dataEntry;
	dataEntry.type = AsmDataType::ASM_DATA_TYPE_LOCAL_DATA;
	dataEntry.local.type = type;
	dataEntry.local.value = value;
	program.push_back(dataEntry);
}

void JWasmTree::addParameterData(string type, string value)
{
	JWasmData dataEntry;
	dataEntry.type = AsmDataType::ASM_DATA_TYPE_PARAMETER_DATA;
	dataEntry.parameter.type = type;
	dataEntry.parameter.value = value;
	program.push_back(dataEntry);
}
