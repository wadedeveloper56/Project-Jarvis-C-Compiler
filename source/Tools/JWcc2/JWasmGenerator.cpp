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
	AsmDirectiveData data;
	data.directive = AsmDirective::NONE1;
	if (bits == 16)
	{
		data.directiveData.push_back("." + processor);
		data.directiveData.push_back("option segment:use16");
		data.directiveData.push_back(".model small, c;");
	}
	else if (bits == 32)
	{
		data.directiveData.push_back("." + processor);
		data.directiveData.push_back("option segment:use32");
		data.directiveData.push_back(".model flat, c;");
	}
	else
	{
		data.directiveData.push_back(".x64p");
	}
	data.directiveData.push_back("option casemap : none");
	tree.addDirective(data);
}

void JWasmGenerator::asmGlobalData(string name, string type, string value)
{
	AsmData* data = nullptr;
	if (type == "int" && bits == 16) data = new AsmData(name, "SWORD", value, ";global var " + name + " type = " + type);
	if (type == "int" && bits == 32) data = new AsmData(name, "SDWORD", value, ";global var " + name + " type = " + type);
	if (type == "int" && bits == 64) data = new AsmData(name, "SDWORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 16) data = new AsmData(name, "WORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 32) data = new AsmData(name, "DWORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 64) data = new AsmData(name, "DWORD", value, ";global var " + name + " type = " + type);
	if (value == "?")
		tree.addUninitData(*data);
	else
		tree.addInitData(*data);
}

void JWasmGenerator::outputProgramUninitializedData(Program& program)
{
	// uninitialized data section for globals
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
	tree.addDirective(AsmDirectiveData(AsmDirective::pCODE, { ".code" }));
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
	tree.addDirective(AsmDirectiveData(AsmDirective::END, { "end" }));
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

void JWasmGenerator::asmOutputParameterVariables(AsmMethod *method, string type, string name)
{
	if (type == "int" && bits == 16) method->parameters.push_back({name, "SWORD"});
	if (type == "int" && bits == 32) method->parameters.push_back({ name, "SDWORD" });
	if (type == "int" && bits == 64) method->parameters.push_back({ name, "SDWORD" });
	if (type == "unsigned int" && bits == 16) method->parameters.push_back({ name, "WORD" });
	if (type == "unsigned int" && bits == 32) method->parameters.push_back({ name, "DWORD" });
	if (type == "unsigned int" && bits == 64) method->parameters.push_back({ name, "DWORD" });
}

void JWasmGenerator::asmOutputLocalVariables(AsmMethod *method,string type, string name)
{
	if (type == "int" && bits == 16) method->locals.push_back({ name, "SWORD" });
	if (type == "int" && bits == 32) method->locals.push_back({ name, "SDWORD" });
	if (type == "int" && bits == 64) method->locals.push_back({ name, "SDWORD" });
	if (type == "unsigned int" && bits == 16) method->locals.push_back({ name, "WORD" });
	if (type == "unsigned int" && bits == 32) method->locals.push_back({ name, "DWORD" });
	if (type == "unsigned int" && bits == 64) method->locals.push_back({ name, "DWORD" });
}

void JWasmGenerator::outputFunctionHeader(AsmMethod *method, FunctionDeclaration& function)
{
	int index = 0;
	for (auto& p : function.params)
	{
		asmOutputParameterVariables(method, p.first, p.second);
	}
}

void JWasmGenerator::outputFunctionLocals(AsmMethod* method, FunctionDeclaration& function)
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
					asmOutputLocalVariables(method, v->type, v->name);
					index++;
				}
			}
		}
	}
}

void JWasmGenerator::moveResultOfInvokeIntoRegisterA(AsmMethod* method, VariableDeclaration* v)
{
	asmStatementMoveRegisterAToVariable(method, v->type, v->name, "\t\t\t;move result of invoke from register A to variable '" + v->name + "'");
	asmStatementPushRegisterA(method, "\t\t\t;push the result in register A onto stack");
}

void JWasmGenerator::outputFunctionLocalsInitializationFunctionCall(AsmMethod* method, CallExpression* exp)
{
	AsmInstruction instr;
	instr.type = AsmOperandType::OPERAND_MACRO;
	instr.macro.push_back("invoke _");
	instr.macro.push_back(exp->callee);
	for (auto& it : exp->args)
	{
		if (auto exp = dynamic_cast<VariableExpression*>(it.get()))
		{
			instr.macro.push_back(exp->name);
		}
		else if (auto exp = dynamic_cast<NumberExpression*>(it.get()))
		{
			instr.macro.push_back(to_string(exp->value));
		}
	}
	method->instructions.push_back(instr);
}

void JWasmGenerator::asmStatementMoveSignExtendVariableToRegisterA(AsmMethod* method, string type, string name, string comment)
{
	method->instructions.push_back(AsmInstruction("movsx", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0)));
}

void JWasmGenerator::asmStatementMoveVariableToRegisterA(AsmMethod* method, string type, string name, string comment)
{
	if (type == "int" && bits == 16) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0))); }
	if (type == "int" && bits == 32) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0))); }
	if (type == "int" && bits == 64) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0))); }
	if (type == "unsigned int" && bits == 16) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0))); }
	if (type == "unsigned int" && bits == 32) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0))); }
	if (type == "unsigned int" && bits == 64) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0))); }

}

void JWasmGenerator::asmStatementMoveImmediateToRegisterA(AsmMethod* method, string type, string name, string comment)
{
	if (type == "int" && bits == 16) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)))); }
	if (type == "int" && bits == 32) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)))); }
	if (type == "int" && bits == 64) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)))); }
	if (type == "unsigned int" && bits == 16) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)))); }
	if (type == "unsigned int" && bits == 32) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)))); }
	if (type == "unsigned int" && bits == 64) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)))); }
}

void JWasmGenerator::asmStatementMoveRegisterAToVariable(AsmMethod* method, string type, string name, string comment)
{
	if (type == "int" && bits == 16) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0))); }
	if (type == "int" && bits == 32) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0))); }
	if (type == "int" && bits == 64) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0))); }
	if (type == "unsigned int" && bits == 16) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0))); }
	if (type == "unsigned int" && bits == 32) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0))); }
	if (type == "unsigned int" && bits == 64) { method->instructions.push_back(AsmInstruction("mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0))); }
}

void JWasmGenerator::outputFunctionLocalsInitialization(AsmMethod* method, FunctionDeclaration& function)
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
					outputFunctionLocalsInitializationFunctionCall(method, callExpression);
					moveResultOfInvokeIntoRegisterA(method, variableDeclaration);
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
					asmStatementMoveRegisterAToVariable(method, variableDeclaration->type, variableDeclaration->name, "\t\t;move result of binary expression from register A to variable '" + variableDeclaration->name + "'");
				}
			}
		}
	}
}

void JWasmGenerator::outputFunctionBody(AsmMethod* method, FunctionDeclaration& function)
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
	method = new AsmMethod();
	method->name = "_" + function.name;
	outputFunctionComment(function);
	outputFunctionHeader(method, function);
	indent++;
	outputFunctionLocals(method, function);
	outputFunctionLocalsInitialization(method, function);
	outputFunctionBody(method, function);
	indent--;
	tree.addMethod(*method);
}

void JWasmGenerator::visit(CompoundStatement& n)
{}

void JWasmGenerator::asmStatementAddRegisterAAndRegisterB(AsmMethod* method, string comment)
{

	if (bits == 64) { method->instructions.push_back(AsmInstruction("add", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0))); }
	else if (bits == 32) { method->instructions.push_back(AsmInstruction("add", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0))); }
	else if (bits == 16) { method->instructions.push_back(AsmInstruction("add", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0))); }
}

void JWasmGenerator::asmStatementSubRegisterAAndRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64) { method->instructions.push_back(AsmInstruction("sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0))); }
	else if (bits == 32) { method->instructions.push_back(AsmInstruction("sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0))); }
	else if (bits == 16) { method->instructions.push_back(AsmInstruction("sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0))); }
}

void JWasmGenerator::asmStatementImulRegisterAAndRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64) { method->instructions.push_back(AsmInstruction("imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0))); }
	else if (bits == 32) { method->instructions.push_back(AsmInstruction("imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0))); }
	else if (bits == 16) { method->instructions.push_back(AsmInstruction("imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0))); }
}

void JWasmGenerator::asmStatementIdivRegisterAAndRegisterB(AsmMethod* method)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction("cdq", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
		method->instructions.push_back(AsmInstruction("idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction("cdq", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
		method->instructions.push_back(AsmInstruction("idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	if (bits == 16)
	{
		method->instructions.push_back(AsmInstruction("cwd", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
		method->instructions.push_back(AsmInstruction("idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
}

void JWasmGenerator::outputReturnBinaryExpression(AsmMethod *method, BinaryExpression* be)
{
	be->leftHandSide->accept(*this);
	be->rightHandSide->accept(*this);
	asmStatementPopRegisterB(method,";pop top of stack into register B");
	asmStatementPopRegisterA(method, ";pop top of stack into register A");
	if (be->operator1 == '+')
	{
		asmStatementAddRegisterAAndRegisterB(method, ";add registers A and B and store result in A");
	}
	else if (be->operator1 == '-')
	{
		asmStatementSubRegisterAAndRegisterB(method, ";subtract register B from A and store result in A");
	}
	else if (be->operator1 == '*')
	{
		asmStatementImulRegisterAAndRegisterB(method, ";multiply registers A and B and store result in A");
	}
	else if (be->operator1 == '/')
	{
		asmStatementIdivRegisterAAndRegisterB(method);
	}
}

void JWasmGenerator::visit(ReturnStatement& n)
{
	if (n.expr)
	{
		if (auto be = dynamic_cast<BinaryExpression*>(n.expr.get()))
		{
			outputReturnBinaryExpression(method, be);
		}
		else
		{
			n.expr->accept(*this);
		}
		method->instructions.push_back(AsmInstruction("ret", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else
	{
		method->instructions.push_back(AsmInstruction("ret", AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
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
		asmStatementMoveImmediateToRegisterA(method,"int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(method,";push register A on to the stack");
	}
	else if (bits == 32)
	{
		asmStatementMoveImmediateToRegisterA(method, "int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(method,";push register A on to the stack");
	}
	else
	{
		asmStatementMoveImmediateToRegisterA(method, "int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(method,";push register A on to the stack");
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
			asmStatementMoveSignExtendVariableToRegisterA(method, it->second.type, it->first, ";load and sign extend '" + it->first + "' in to register A");
			asmStatementPushRegisterA(method, ";push register A on to stack");
		}
		else
		{
			asmStatementMoveVariableToRegisterA(method, it->second.type, it->first, ";load parameter " + it->first);
			asmStatementPushRegisterA(method, ";push register A on to stack");
		}
	}
	else if (itLocal != localIndex.end())
	{
		if (itLocal->second.type == "int" && bits == 64)
		{
			asmStatementMoveSignExtendVariableToRegisterA(method, itLocal->second.type, itLocal->first, ";load and sign extend '" + itLocal->first + "' in to register A");
			asmStatementPushRegisterA(method, ";push register A on to stack");
		}
		else
		{
			asmStatementMoveVariableToRegisterA(method, itLocal->second.type, itLocal->first, ";load parameter " + itLocal->first);
			asmStatementPushRegisterA(method, ";push register A on to stack");
		}
	}
	else
	{
		//out << ";?? '" << expression.name << "'" << endl;
	}
}

void JWasmGenerator::asmStatementPopRegisterA(AsmMethod* method,string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else
	{
		method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
}

void JWasmGenerator::asmStatementPushRegisterA(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else
	{
		method->instructions.push_back(AsmInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
}

void JWasmGenerator::asmStatementPopRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else
	{
		method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
}

void JWasmGenerator::asmStatementPushRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::RBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
	else
	{
		method->instructions.push_back(AsmInstruction("push", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	}
}

void JWasmGenerator::visit(BinaryExpression& be)
{
	be.leftHandSide->accept(*this);
	be.rightHandSide->accept(*this);
	asmStatementPopRegisterB(method, ";pop top of stack into register B");
	asmStatementPopRegisterA(method, ";pop top of stack into register A");
	if (be.operator1 == '+')
	{
		asmStatementAddRegisterAAndRegisterB(method, ";add registers A and B and store result in A");
	}
	else if (be.operator1 == '-')
	{
		asmStatementSubRegisterAAndRegisterB(method, ";subtract register B from A and store result in A");
	}
	else if (be.operator1 == '*')
	{
		asmStatementImulRegisterAAndRegisterB(method, ";multiply registers A and B and store result in A");
	}
	else if (be.operator1 == '/')
	{
		asmStatementIdivRegisterAAndRegisterB(method);
	}
	asmStatementPushRegisterA(method, ";push register A on to the stack");
}

void JWasmGenerator::visit(CallExpression& n)
{

}

void JWasmGenerator::visit(AssignExpression& n)
{
	n.value->accept(*this);
	if (bits == 64) method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	else if (bits == 16) method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	else method->instructions.push_back(AsmInstruction("pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_NONE, AsmRegisters::NONE, "", 0)));
	auto it = paramIndex.find(n.name);
	auto itLocal = localIndex.find(n.name);
	if (it != paramIndex.end())
	{
		asmStatementMoveRegisterAToVariable(method, it->second.type, it->first, ";store parameter " + it->first);
	}
	else if (itLocal != localIndex.end())
	{
		asmStatementMoveRegisterAToVariable(method, itLocal->second.type, itLocal->first, ";store local " + itLocal->first);
	}
	else
	{
		//out << "; store to global " << n.name << endl;
	}
}

void JWasmGenerator::output()
{
	ostream& out = cout;
	for (auto& line : tree.getProgram())
	{
	}
}