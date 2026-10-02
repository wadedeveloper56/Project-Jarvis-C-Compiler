#include "pch.h"
#include "JWasmTree.h"

JWasmTree::JWasmTree() {}

JWasmTree::~JWasmTree() {}

void JWasmTree::addMethod(AsmMethod method)
{
	JWasmData instruction;
	instruction.type = AsmDataType::ASM_DATA_TYPE_METHOD;
	instruction.methods.push_back(method);
	program.push_back(instruction);
}

void JWasmTree::addDirective(AsmDirectiveData directive)
{
	JWasmData instruction;
	instruction.type = AsmDataType::ASM_DATA_DIRECTIVE;
	instruction.directive = directive;
	program.push_back(instruction);
}

void JWasmTree::addInitData(AsmData initData)
{
	JWasmData instruction;
	instruction.type = AsmDataType::ASM_DATA_TYPE_DATA;
	instruction.initData.push_back(initData);
	program.push_back(instruction);
}

void JWasmTree::addUninitData(AsmData uninitData)
{
	JWasmData instruction;
	instruction.type = AsmDataType::ASM_DATA_TYPE_DATA;
	instruction.uninitData.push_back(uninitData);
	program.push_back(instruction);
}

void JWasmTree::addComment(string comment)
{
	JWasmData instruction;
	instruction.type = AsmDataType::ASM_DATA_TYPE_COMMENT;
	instruction.comment = comment;
	program.push_back(instruction);
}
