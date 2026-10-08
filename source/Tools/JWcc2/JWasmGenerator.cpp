#include "pch.h"
#include "JWasmGenerator.h"
#include "SymbolTable.h"
#include "types.h"

using namespace std;

JWasmGenerator::JWasmGenerator(ostream& os, int bits, bool isWindows) : out(os), bits(bits), indent(0), isWindows(isWindows) {}

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
	data.directive = AsmDirective::ASSEMBLER;
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
	tree.addDirective(AsmDirectiveData(AsmDirective::pDATA, { "\n.data?" }));
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
	tree.addDirective(AsmDirectiveData(AsmDirective::pDATA, { "\n.data" }));
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<VariableDeclaration*>(d.get()))
		{
			if (auto init = dynamic_cast<Expression*>(gv->init.get()))
			{
				if (auto expr = dynamic_cast<NumberExpression*>(init))
				{
					asmGlobalData(gv->name, gv->type, to_string(expr->value));
				}
			}
		}
	}
}

void JWasmGenerator::outputProgramCode(Program& program)
{
	tree.addDirective(AsmDirectiveData(AsmDirective::pCODE, { "\n.code" }));
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

void JWasmGenerator::asmOutputParameterVariables(AsmMethod* method, string type, string name)
{
	if (type == "int" && bits == 16) method->parameters.push_back({ name, "SWORD" });
	if (type == "int" && bits == 32) method->parameters.push_back({ name, "SDWORD" });
	if (type == "int" && bits == 64) method->parameters.push_back({ name, "SDWORD" });
	if (type == "unsigned int" && bits == 16) method->parameters.push_back({ name, "WORD" });
	if (type == "unsigned int" && bits == 32) method->parameters.push_back({ name, "DWORD" });
	if (type == "unsigned int" && bits == 64) method->parameters.push_back({ name, "DWORD" });
}

void JWasmGenerator::asmOutputLocalVariables(AsmMethod* method, string type, string name)
{
	if (type == "int" && bits == 16) method->locals.push_back({ name, "SWORD" });
	if (type == "int" && bits == 32) method->locals.push_back({ name, "SDWORD" });
	if (type == "int" && bits == 64) method->locals.push_back({ name, "SDWORD" });
	if (type == "unsigned int" && bits == 16) method->locals.push_back({ name, "WORD" });
	if (type == "unsigned int" && bits == 32) method->locals.push_back({ name, "DWORD" });
	if (type == "unsigned int" && bits == 64) method->locals.push_back({ name, "DWORD" });
}

void JWasmGenerator::outputFunctionHeader(AsmMethod* method, FunctionDeclaration& function)
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
	instr.type = AsmInstructionType::INSTRUCTION_MACRO;
	instr.macro.push_back("invoke ");
	instr.macro.push_back("_" + exp->callee);
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
	method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "movsx", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment));
}

void JWasmGenerator::asmStatementMoveVariableToRegisterA(AsmMethod* method, string type, string name, string comment)
{
	if (type == "int" && bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment)); }
	if (type == "int" && bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment)); }
	if (type == "int" && bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment)); }
	if (type == "unsigned int" && bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment)); }
	if (type == "unsigned int" && bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment)); }
	if (type == "unsigned int" && bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_MEMORY_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), comment)); }

}

void JWasmGenerator::asmStatementMoveImmediateToRegisterA(AsmMethod* method, string type, string name, string comment)
{
	if (type == "int" && bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_IMMEDIATE_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment)); }
	if (type == "int" && bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_IMMEDIATE_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment)); }
	if (type == "int" && bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_IMMEDIATE_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment)); }
	if (type == "unsigned int" && bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_IMMEDIATE_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment)); }
	if (type == "unsigned int" && bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_IMMEDIATE_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment)); }
	if (type == "unsigned int" && bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_IMMEDIATE_TO_REGISTER, "mov", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_IMMEDIATE, AsmRegisters::NONE, "", stoll(name)), comment)); }
}

void JWasmGenerator::asmStatementMoveRegisterAToVariable(AsmMethod* method, string type, string name, string comment)
{
	if (type == "int" && bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_MEMORY, "mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), comment)); }
	if (type == "int" && bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_MEMORY, "mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment)); }
	if (type == "int" && bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_MEMORY, "mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment)); }
	if (type == "unsigned int" && bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_MEMORY, "mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), comment)); }
	if (type == "unsigned int" && bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_MEMORY, "mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment)); }
	if (type == "unsigned int" && bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_MEMORY, "mov", AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" + name, 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), comment)); }
}

void JWasmGenerator::outputFunctionLocalsInitialization(AsmMethod* method, FunctionDeclaration& function)
{
	method->instructions.push_back(AsmInstruction(INSTRUCTION_COMMENT, "", AsmOperand(), AsmOperand(), ";----------- Local variable initialization ----------"));
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
		method->instructions.push_back(AsmInstruction(INSTRUCTION_COMMENT, "", AsmOperand(), AsmOperand(), ";----------Handle Function Body Statements-----------"));
		for (auto& statement : body.statements)
		{
			statement->accept(*this);
		}
		method->instructions.push_back(AsmInstruction(INSTRUCTION_COMMENT, "", AsmOperand(), AsmOperand(), ";----------------------------------------------------"));
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

	if (bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "add", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment)); }
	else if (bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "add", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment)); }
	else if (bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "add", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), comment)); }
}

void JWasmGenerator::asmStatementSubRegisterAAndRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment)); }
	else if (bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment)); }
	else if (bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "sub", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), comment)); }
}

void JWasmGenerator::asmStatementImulRegisterAAndRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment)); }
	else if (bits == 32) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), comment)); }
	else if (bits == 16) { method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER_TO_REGISTER, "imul", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX, "", 0), AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), comment)); }
}

void JWasmGenerator::asmStatementIdivRegisterAAndRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_NONE, "cdq", AsmOperand(), AsmOperand(), comment));
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(), comment));
	}
	if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_NONE, "cdq", AsmOperand(), AsmOperand(), comment));
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX, "", 0), AsmOperand(), comment));
	}
	if (bits == 16)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_NONE, "cwd", AsmOperand(), AsmOperand(), comment));
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "idiv", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX, "", 0), AsmOperand(), comment));
	}
}

void JWasmGenerator::outputReturnBinaryExpression(AsmMethod* method, BinaryExpression* be)
{
	be->leftHandSide->accept(*this);
	be->rightHandSide->accept(*this);
	asmStatementPopRegisterB(method, ";pop top of stack into register B");
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
		asmStatementIdivRegisterAAndRegisterB(method, ";divide register A by register B and store result in A");
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
		method->instructions.push_back(AsmInstruction(INSTRUCTION_NONE, "ret", AsmOperand(), AsmOperand(), ";return from function"));
	}
	else
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_NONE, "ret", AsmOperand(), AsmOperand(), ";return from function"));
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
		asmStatementMoveImmediateToRegisterA(method, "int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(method, ";push register A on to the stack");
	}
	else if (bits == 32)
	{
		asmStatementMoveImmediateToRegisterA(method, "int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(method, ";push register A on to the stack");
	}
	else
	{
		asmStatementMoveImmediateToRegisterA(method, "int", to_string(n.value), ";load immediate into register");
		asmStatementPushRegisterA(method, ";push register A on to the stack");
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
			asmStatementMoveVariableToRegisterA(method, it->second.type, it->first, ";load and sign extend '" + it->first + "' in to register A");
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
			asmStatementMoveVariableToRegisterA(method, itLocal->second.type, itLocal->first, ";load and sign extend '" + itLocal->first + "' in to register A");
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

void JWasmGenerator::asmStatementPopRegisterA(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX), AsmOperand(), comment));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX), AsmOperand(), comment));
	}
	else
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX), AsmOperand(), comment));
	}
}

void JWasmGenerator::asmStatementPushRegisterA(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "push", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX), AsmOperand(), comment));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "push", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX), AsmOperand(), comment));
	}
	else
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "push", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX), AsmOperand(), comment));
	}
}

void JWasmGenerator::asmStatementPopRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RBX), AsmOperand(), comment));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX), AsmOperand(), comment));
	}
	else
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX), AsmOperand(), comment));
	}
}

void JWasmGenerator::asmStatementPushRegisterB(AsmMethod* method, string comment)
{
	if (bits == 64)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "push", AsmOperand(OPERAND_REGISTER, AsmRegisters::RBX), AsmOperand(), comment));
	}
	else if (bits == 32)
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "push", AsmOperand(OPERAND_REGISTER, AsmRegisters::EBX), AsmOperand(), comment));
	}
	else
	{
		method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "push", AsmOperand(OPERAND_REGISTER, AsmRegisters::BX), AsmOperand(), comment));
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
		asmStatementIdivRegisterAAndRegisterB(method, ";divide register A by register B and store result in A");
	}
	asmStatementPushRegisterA(method, ";push register A on to the stack");
}

void JWasmGenerator::visit(CallExpression& n)
{

}

void JWasmGenerator::visit(AssignExpression& n)
{
	n.value->accept(*this);
	if (bits == 64) method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX), AsmOperand(), ";pop top of stack into register A"));
	else if (bits == 16) method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::AX), AsmOperand(), ";pop top of stack into register A"));
	else method->instructions.push_back(AsmInstruction(INSTRUCTION_REGISTER, "pop", AsmOperand(OPERAND_REGISTER, AsmRegisters::EAX), AsmOperand(), ";pop top of stack into register A"));
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

void JWasmGenerator::outputMethodHeader(AsmMethod& method, ostream& out)
{
	int size = (int)method.parameters.size();
	out << method.name << " PROC";
	if (size > 0)
	{
		int index = 0;
		for (auto& p : method.parameters)
		{
			out << " _" << p.first << ":" << p.second;
			if (index < size - 1) out << ", ";
			index++;
		}
	}
	out << endl;
}

void JWasmGenerator::outputMethodLocals(AsmMethod& method, ostream& out)
{
	int size = (int)method.locals.size();
	if (size > 0)
	{
		out << "LOCAL ";
		int index = 0;
		for (auto& p : method.locals)
		{
			out << " _" << p.first << ":" << p.second;
			if (index < size - 1) out << ", ";
			index++;
		}
		out << endl;
	}
}

void JWasmGenerator::outputMethodInstructionsMacro(ostream& out, AsmInstruction& instr)
{
	if (instr.macro.size() > 0)
	{
		out << instr.macro[0];
		int size = (int)instr.macro.size();
		int index = 0;
		for (size_t i = 1; i < size; i++)
		{
			out << " " << instr.macro[i];
			if (index < size - 2) out << ", ";
			index++;
		}
		out << endl;
	}
}

string JWasmGenerator::getRegister(AsmRegisters reg)
{
	switch (reg)
	{
		case AsmRegisters::RAX: return "rax";
		case AsmRegisters::RBX: return "rbx";
		case AsmRegisters::RCX: return "rcx";
		case AsmRegisters::RDX: return "rdx";
		case AsmRegisters::EAX: return "eax";
		case AsmRegisters::EBX: return "ebx";
		case AsmRegisters::ECX: return "ecx";
		case AsmRegisters::EDX: return "edx";
		case AsmRegisters::AX: return "ax";
		case AsmRegisters::BX: return "bx";
		case AsmRegisters::CX: return "cx";
		case AsmRegisters::DX: return "dx";
		default: return "";
	}
}

void JWasmGenerator::outputMethodInstructions(AsmMethod& method, ostream& out)
{
	for (auto& instr : method.instructions)
	{
		switch (instr.type)
		{
			case INSTRUCTION_NONE:
				out << instr.mnemonic << endl;
				break;
			case INSTRUCTION_MEMORY_TO_REGISTER:
			{
				string reg = getRegister(instr.op1.registers);
				out << instr.mnemonic << " " << reg << ", " << instr.op2.variable << " " << instr.comment << endl;
				break;
			}
			case INSTRUCTION_REGISTER_TO_MEMORY:
			{
				string reg = getRegister(instr.op2.registers);
				out << instr.mnemonic << " " << instr.op1.variable << ", " << reg << " " << instr.comment << endl;
				break;
			}
			case INSTRUCTION_IMMEDIATE_TO_REGISTER:
			{
				string reg = getRegister(instr.op1.registers);
				out << instr.mnemonic << " " << reg << ", " << instr.op2.immediate << " " << instr.comment << endl;
				break;
			}
			case INSTRUCTION_IMMEDIATE_TO_MEMORY:
			{
				string reg = getRegister(instr.op1.registers);
				out << instr.mnemonic << " " << reg << ", " << instr.op2.immediate << " " << instr.comment << endl;
				break;
			}
			case INSTRUCTION_REGISTER_TO_REGISTER:
			{
				string reg1 = getRegister(instr.op1.registers);
				string reg2 = getRegister(instr.op2.registers);
				out << instr.mnemonic << " " << reg1 << ", " << reg2 << " " << instr.comment << endl;
				break;
			}
			case INSTRUCTION_REGISTER:
			{
				string reg = getRegister(instr.op1.registers);
				out << instr.mnemonic << " " << reg << " " << instr.comment << endl;
				break;
			}
			case INSTRUCTION_MACRO:
			{
				int index = 0;
				int size = instr.macro.size();
				for (auto& m : instr.macro)
				{
					if (index >= 1 && index < size - 1)
						out << m << ", ";
					else
						out << m << " ";
					index++;
				}
				out << endl;
				break;
			}
			case INSTRUCTION_COMMENT:
				out << instr.comment << endl;
				break;
			default:
				out << "?? " << instr.type;
				break;
		}
	}
}

void JWasmGenerator::output()
{
	//ostream& out = cout;
	for (auto& line : tree.getProgram())
	{
		switch (line.type)
		{
			case ASM_DATA_TYPE_NONE:
				break;
			case ASM_DATA_TYPE_INIT_DATA:
				for (auto& d : line.initData)
				{
					out << d.label << " " << d.type << " " << d.value << " " << d.comment << endl;
				}
				break;
			case ASM_DATA_TYPE_UNINIT_DATA:
				for (auto& d : line.uninitData)
				{
					out << d.label << " " << d.type << " " << d.value << " " << d.comment << endl;
				}
				break;
			case ASM_DATA_TYPE_METHOD:
				for (auto& method : line.methods)
				{
					outputMethodHeader(method, out);
					outputMethodLocals(method, out);
					outputMethodInstructions(method, out);
					out << method.name << " ENDP" << endl << endl;
				}
				break;
			case ASM_DATA_DIRECTIVE:
				for (auto& d : line.directive.directiveData)
				{
					out << d << endl;
				}
				break;
			case ASM_DATA_TYPE_COMMENT:
				out << line.comment << endl;
				break;
		}
	}
}