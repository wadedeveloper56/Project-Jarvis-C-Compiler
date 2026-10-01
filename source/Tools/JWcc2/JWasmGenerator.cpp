#include "pch.h"
#include "JWasmGenerator.h"
#include "SymbolTable.h"
#include "types.h"

using namespace std;

JWasmGenerator::JWasmGenerator(ostream& os, int bits, bool isWindows) : /*out(os),*/ bits(bits), indent(0), isWindows(isWindows) {}

void JWasmGenerator::generate(Program& program) { program.accept(*this); }

//void JWasmGenerator::ind() { for (int i = 1; i <= indent; ++i) out << "  "; }

string JWasmGenerator::regA(int size) const {
	if (size == 64) return "rax";
	if (size == 16) return "ax";
	return "eax";
}

string JWasmGenerator::regB(int size) const {
	if (size == 64) return "rbx";
	if (size == 16) return "bx";
	return "ebx";
}

string JWasmGenerator::regC(int size) const {
	if (size == 64) return "rcx";
	if (size == 16) return "cx";
	return "ecx";
}

string JWasmGenerator::regD(int size) const {
	if (size == 64) return "rdx";
	if (size == 16) return "dx";
	return "edx";
}

void JWasmGenerator::outputProgramFileHeader()
{
	if (bits == 16)
	{
		tree.addDirective(AsmDirective::NONE1, "." + processor, "");
		tree.addDirective(AsmDirective::NONE1, "option segment:use16", "");
		tree.addDirective(AsmDirective::NONE1, ".model small, c;", "");
	}
	else if (bits == 32)
	{
		tree.addDirective(AsmDirective::NONE1, "." + processor, "");
		tree.addDirective(AsmDirective::NONE1, "option segment:use32", "");
		tree.addDirective(AsmDirective::NONE1, ".model flat, c;", "");
	}
	else
	{
		tree.addDirective(AsmDirective::NONE1, ".x64p", "");
	}
	tree.addDirective(AsmDirective::NONE1, "option casemap : none", "");
}

void JWasmGenerator::asmGlobalData(string name, string type, string value)
{
	if (type == "int" && bits == 16) tree.addGlobalData(name, "SWORD", value, ";global var " + name + " type = " + type);
	if (type == "int" && bits == 32) tree.addGlobalData(name, "SDWORD", value, ";global var " + name + " type = " + type);
	if (type == "int" && bits == 64) tree.addGlobalData(name, "SDWORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 16) tree.addGlobalData(name, "WORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 32) tree.addGlobalData(name, "DWORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 64) tree.addGlobalData(name, "DWORD", value, ";global var " + name + " type = " + type);
}

void JWasmGenerator::outputProgramUninitializedData(Program& program)
{
	// uninitialized data section for globals
	tree.addDirective(AsmDirective::NONE1, ".data?", "");
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<VariableDeclaration*>(d.get()))
		{
			auto init = dynamic_cast<Expression*>(gv->init.get());
			if (init == nullptr)
			{
				asmGlobalData(gv->name, gv->type, "?");
			}
		}
	}
}

void JWasmGenerator::outputProgramInitializedData(Program& program)
{
	// initialized data section for globals
	tree.addDirective(AsmDirective::NONE1, ".data", "");
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<VariableDeclaration*>(d.get()))
		{
			if (auto init = dynamic_cast<Expression*>(gv->init.get()))
			{
				if (auto expr = dynamic_cast<NumberExpression*>(init))
				{
					asmGlobalData(gv->name, gv->type, expr->value + "");
				}
			}
		}
	}
}

void JWasmGenerator::outputProgramCode(Program& program)
{
	tree.addDirective(AsmDirective::NONE1, ".code", "");
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<FunctionDeclaration*>(d.get()))
		{
			gv->accept(*this);
		}
	}
}

void JWasmGenerator::visit(Program& program)
{
	outputProgramFileHeader();
	outputProgramUninitializedData(program);
	outputProgramInitializedData(program);
	outputProgramCode(program);
	tree.addDirective(AsmDirective::NONE1, "end", "");
}

void JWasmGenerator::visit(VariableDeclaration& n)
{}

void JWasmGenerator::outputFunctionComment(FunctionDeclaration& function)
{
	tree.addComment(";-----------------");
	tree.addComment("; params:");
	if (!function.params.empty())
	{
		for (auto& p : function.params)
		{
			tree.addComment(";   " + p.second + " : " + p.first);
		}
	}
	tree.addComment("; locals:");
	if (function.body)
	{
		if (auto comp = dynamic_cast<CompoundStatement*>(function.body.get()))
		{
			for (auto& ld : comp->localDeclarations)
			{
				if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
				{
					tree.addComment(";   " + v->name + " : " + v->type);
				}
			}
		}
	}
	tree.addComment(";-----------------");
}

void JWasmGenerator::asmOutputParameterVariables(string type, string name)
{
	if (type == "int" && bits == 16) tree.addParameterData(name, "SWORD");
	if (type == "int" && bits == 32) tree.addParameterData(name, "SDWORD");
	if (type == "int" && bits == 64) tree.addParameterData(name, "SDWORD");
	if (type == "unsigned int" && bits == 16) tree.addParameterData(name, "WORD");
	if (type == "unsigned int" && bits == 32) tree.addParameterData(name, "DWORD");
	if (type == "unsigned int" && bits == 64) tree.addParameterData(name, "DWORD");
}

void JWasmGenerator::asmOutputLocalVariables(string type, string name)
{
	if (type == "int" && bits == 16) tree.addLocalData("SWORD", name);
	if (type == "int" && bits == 32) tree.addLocalData("SDWORD", name);
	if (type == "int" && bits == 64) tree.addLocalData("SDWORD", name);
	if (type == "unsigned int" && bits == 16) tree.addLocalData("WORD", name);
	if (type == "unsigned int" && bits == 32) tree.addLocalData("DWORD", name);
	if (type == "unsigned int" && bits == 64) tree.addLocalData("DWORD", name);
}

void JWasmGenerator::outputFunctionHeader(FunctionDeclaration& function)
{
	int index = 0;
	tree.addDirective(AsmDirective::PROC, "_" + function.name, ";function " + function.name + " with " + to_string(function.params.size()) + " parameters");
	for (auto& p : function.params)
	{
		asmOutputParameterVariables(p.first, p.second);
	}
}

void JWasmGenerator::outputFunctionLocals(FunctionDeclaration& function)
{
	if (auto comp = dynamic_cast<CompoundStatement*>(function.body.get()))
	{
		int index = 0, idx = 0;
		int size = (int)comp->localDeclarations.size();
		if (size > 0)
		{
			for (auto& ld : comp->localDeclarations)
			{
				if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
				{
					localIndex[v->name] = { v->type, idx++ };
					asmOutputLocalVariables(v->type, v->name);
					index++;
				}
			}
		}
	}
}

void JWasmGenerator::moveResultOfInvokeIntoRegisterA(VariableDeclaration* v)
{
	asmStatementMoveRegisterAToVariable(v->type, v->name, "\t\t\t;move result of invoke from register A to variable '" + v->name + "'");
	asmStatementPushRegisterA("\t\t\t;push the result in register A onto stack");
}

void JWasmGenerator::outputFunctionLocalsInitializationFunctionCall(CallExpression* exp)
{
	ostringstream ss;
	ss << "invoke _" << exp->callee;
	for (auto& it : exp->args)
	{
		if (auto exp = dynamic_cast<VariableExpression*>(it.get()))
		{
			ss << ", " << exp->name;
		}
		else if (auto exp = dynamic_cast<NumberExpression*>(it.get()))
		{
			ss << ", " << exp->value;
		}
	}
	tree.addDirective(AsmDirective::INVOKE, "_" + exp->callee, ss.str(), ";invoke function '" + exp->callee + "' with " + to_string(exp->args.size()) + " arguments");
}

void JWasmGenerator::asmStatementMoveSignExtendVariableToRegisterA(string type, string name, string comment)
{
	tree.addInstruction("movsx", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment);
}

void JWasmGenerator::asmStatementMoveVariableToRegisterA(string type, string name, string comment)
{
	if (type == "int" && bits == 16) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment); }
	if (type == "int" && bits == 32) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment); }
	if (type == "int" && bits == 64) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment); }
	if (type == "unsigned int" && bits == 16) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment); }
	if (type == "unsigned int" && bits == 32) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment); }
	if (type == "unsigned int" && bits == 64) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment); }

}

void JWasmGenerator::asmStatementMoveImmediateToRegisterA(string type, string name, string comment)
{
	if (type == "int" && bits == 16) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment); }
	if (type == "int" && bits == 32) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment); }
	if (type == "int" && bits == 64) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment); }
	if (type == "unsigned int" && bits == 16) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment); }
	if (type == "unsigned int" && bits == 32) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment); }
	if (type == "unsigned int" && bits == 64) { tree.addInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment); }
}

void JWasmGenerator::asmStatementMoveRegisterAToVariable(string type, string name, string comment)
{
	if (type == "int" && bits == 16) { tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), comment); }
	if (type == "int" && bits == 32) { tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment); }
	if (type == "int" && bits == 64) { tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment); }
	if (type == "unsigned int" && bits == 16) { tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), comment); }
	if (type == "unsigned int" && bits == 32) { tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment); }
	if (type == "unsigned int" && bits == 64) { tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment); }
}

void JWasmGenerator::outputFunctionLocalsInitialization(FunctionDeclaration& function)
{
	tree.addComment(";----------- Local variable initialization ----------");
	if (auto compoundStatement = dynamic_cast<CompoundStatement*>(function.body.get()))
	{
		for (auto& localDeclaration : compoundStatement->localDeclarations)
		{
			if (auto variableDeclaration = dynamic_cast<VariableDeclaration*>(localDeclaration.get()))
			{
				if (auto callExpression = dynamic_cast<CallExpression*>(variableDeclaration->init.get()))
				{
					outputFunctionLocalsInitializationFunctionCall(callExpression);
					moveResultOfInvokeIntoRegisterA(variableDeclaration);
				}
				else if (auto exp = dynamic_cast<Expression*>(variableDeclaration->init.get()))
				{
					if (auto be = dynamic_cast<BinaryExpression*>(exp))
					{
						be->accept(*this);
					}
					else
					{
						exp->accept(*this);
					}
					asmStatementMoveRegisterAToVariable(variableDeclaration->type, variableDeclaration->name, "\t\t;move result of binary expression from register A to variable '" + variableDeclaration->name + "'");
				}
			}
		}
	}
}

void JWasmGenerator::outputFunctionBody(FunctionDeclaration& function)
{
	if (function.body)
	{
		auto& body = *function.body;
		tree.addComment(";----------Handle Function Body Statements-----------");
		for (auto& statement : body.statements)
		{
			statement->accept(*this);
		}
		tree.addComment(";----------------------------------------------------");
	}
}

void JWasmGenerator::visit(FunctionDeclaration& function)
{
	paramIndex.clear();
	localIndex.clear();
	int idx = 0;
	for (auto& parameter : function.params)
	{
		paramIndex[parameter.second] = { parameter.first, idx++ };
	}
	outputFunctionComment(function);
	outputFunctionHeader(function);
	indent++;
	outputFunctionLocals(function);
	outputFunctionLocalsInitialization(function);
	outputFunctionBody(function);
	indent--;
	tree.addDirective(AsmDirective::ENDP, "_" + function.name, ";end of function " + function.name);
}

void JWasmGenerator::visit(CompoundStatement& n)
{}

void JWasmGenerator::asmStatementAddRegisterAAndRegisterB(string comment)
{

	if (bits == 64) { tree.addInstruction("add", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment); }
	else if (bits == 32) { tree.addInstruction("add", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment); }
	else if (bits == 16) { tree.addInstruction("add", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), comment); }
}

void JWasmGenerator::asmStatementSubRegisterAAndRegisterB(string comment)
{
	if (bits == 64) { tree.addInstruction("sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment); }
	else if (bits == 32) { tree.addInstruction("sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment); }
	else if (bits == 16) { tree.addInstruction("sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), comment); }
}

void JWasmGenerator::asmStatementImulRegisterAAndRegisterB(string comment)
{
	if (bits == 64) { tree.addInstruction("imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment); }
	else if (bits == 32) { tree.addInstruction("imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment); }
	else if (bits == 16) { tree.addInstruction("imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), comment); }
}

void JWasmGenerator::asmStatementIdivRegisterAAndRegisterB()
{
	if (bits == 64)
	{
		tree.addInstruction("cdq", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";extend eax to edx : eax for idiv");
		tree.addInstruction("idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";integer divide registers A and B and store result in A");
	}
	if (bits == 32)
	{
		tree.addInstruction("cdq", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";extend eax to edx : eax for idiv");
		tree.addInstruction("idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";integer divide registers A and B and store result in A");
	}
	if (bits == 16)
	{
		tree.addInstruction("cwd", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";extend ax to dx : ax for idiv");
		tree.addInstruction("idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";integer divide registers AX and BX and store result in AX");
	}
}

void JWasmGenerator::outputReturnBinaryExpression(BinaryExpression* be)
{
	be->leftHandSide->accept(*this);
	be->rightHandSide->accept(*this);
	asmStatementPopRegisterB(";pop top of stack into register B");
	asmStatementPopRegisterA(";pop top of stack into register A");
	if (be->operator1 == '+')
	{
		asmStatementAddRegisterAAndRegisterB(";add registers A and B and store result in A");
	}
	else if (be->operator1 == '-')
	{
		asmStatementSubRegisterAAndRegisterB(";subtract register B from A and store result in A");
	}
	else if (be->operator1 == '*')
	{
		asmStatementImulRegisterAAndRegisterB(";multiply registers A and B and store result in A");
	}
	else if (be->operator1 == '/')
	{
		asmStatementIdivRegisterAAndRegisterB();
	}
}

void JWasmGenerator::visit(ReturnStatement& n)
{
	if (n.expr)
	{
		if (auto be = dynamic_cast<BinaryExpression*>(n.expr.get()))
		{
			outputReturnBinaryExpression(be);
		}
		else
		{
			n.expr->accept(*this);
		}
		tree.addInstruction("ret", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";return from function");
	}
	else
	{
		tree.addInstruction("ret", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";return from function");

	}
}

void JWasmGenerator::visit(ExpressionStatement& n)
{
	if (n.expr) n.expr->accept(*this);
}

void JWasmGenerator::visit(NumberExpression& n)
{
	// push immediate into register/stack
	if (bits == 64)
	{
		asmStatementMoveImmediateToRegisterA("int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(";push register A on to the stack");
	}
	else if (bits == 32)
	{
		asmStatementMoveImmediateToRegisterA("int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(";push register A on to the stack");
	}
	else
	{
		asmStatementMoveImmediateToRegisterA("int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(";push register A on to the stack");
	}
}

void JWasmGenerator::visit(VariableExpression& expression)
{
	auto it = paramIndex.find(expression.name);
	auto itLocal = localIndex.find(expression.name);
	if (it != paramIndex.end())
	{
		if (it->second.type == "int" && bits == 64)
		{
			asmStatementMoveSignExtendVariableToRegisterA(it->second.type, it->first, ";load and sign extend '" + it->first + "' in to register A");
			asmStatementPushRegisterA(";push register A on to stack");
		}
		else
		{
			asmStatementMoveVariableToRegisterA(it->second.type, it->first, ";load parameter " + it->first);
			asmStatementPushRegisterA(";push register A on to stack");
		}
	}
	else if (itLocal != localIndex.end())
	{
		if (itLocal->second.type == "int" && bits == 64)
		{
			asmStatementMoveSignExtendVariableToRegisterA(it->second.type, itLocal->first, ";load and sign extend '" + itLocal->first + "' in to register A");
			asmStatementPushRegisterA(";push register A on to stack");
		}
		else
		{
			asmStatementMoveVariableToRegisterA(itLocal->second.type, itLocal->first, ";load parameter " + itLocal->first);
			asmStatementPushRegisterA(";push register A on to stack");
		}
	}
	else
	{
		//out << ";?? '" << expression.name << "'" << endl;
	}
}

void JWasmGenerator::asmStatementPopRegisterA(string comment)
{
	if (bits == 64)
	{
		tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else if (bits == 32)
	{
		tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else
	{
		tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
}

void JWasmGenerator::asmStatementPushRegisterA(string comment)
{
	if (bits == 64)
	{
		tree.addInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else if (bits == 32)
	{
		tree.addInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else
	{
		tree.addInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
}

void JWasmGenerator::asmStatementPopRegisterB(string comment)
{
	if (bits == 64)
	{
		tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else if (bits == 32)
	{
		tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else
	{
		tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
}

void JWasmGenerator::asmStatementPushRegisterB(string comment)
{
	if (bits == 64)
	{
		tree.addInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::RBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else if (bits == 32)
	{
		tree.addInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
	else
	{
		tree.addInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), comment);
	}
}

void JWasmGenerator::visit(BinaryExpression& be)
{
	be.leftHandSide->accept(*this);
	be.rightHandSide->accept(*this);
	asmStatementPopRegisterB(";pop top of stack into register B");
	asmStatementPopRegisterA(";pop top of stack into register A");
	if (be.operator1 == '+')
	{
		asmStatementAddRegisterAAndRegisterB(";add registers A and B and store result in A");
	}
	else if (be.operator1 == '-')
	{
		asmStatementSubRegisterAAndRegisterB(";subtract register B from A and store result in A");
	}
	else if (be.operator1 == '*')
	{
		asmStatementImulRegisterAAndRegisterB(";multiply registers A and B and store result in A");
	}
	else if (be.operator1 == '/')
	{
		asmStatementIdivRegisterAAndRegisterB();
	}
	asmStatementPushRegisterA(";push register A on to the stack");
}

void JWasmGenerator::visit(AssignExpression& n)
{
	n.value->accept(*this);
	if (bits == 64) tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";pop value into register 1");
	else if (bits == 16) tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";pop value into register 1");
	else tree.addInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), ";pop value into register 1");
	auto it = paramIndex.find(n.name);
	auto itLocal = localIndex.find(n.name);
	if (it != paramIndex.end())
	{
		asmStatementMoveRegisterAToVariable(it->second.type, it->first, ";store parameter " + it->first);
		if (bits == 64) tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + n.name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), ";pop value into register 1");
		else if (bits == 16) tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + n.name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), ";pop value into register 1");
		else tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + n.name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), ";pop value into register A");

	}
	else if (itLocal != localIndex.end())
	{
		asmStatementMoveRegisterAToVariable(itLocal->second.type, itLocal->first, ";store local " + itLocal->first);
		if (bits == 64) tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + n.name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), ";pop value into register 1");
		else if (bits == 16) tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + n.name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), ";pop value into register 1");
		else tree.addInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + n.name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), ";pop value into register A");
	}
	else
	{
		//out << "; store to global " << n.name << endl;
	}
}

void JWasmGenerator::visit(CallExpression& n)
{

}
