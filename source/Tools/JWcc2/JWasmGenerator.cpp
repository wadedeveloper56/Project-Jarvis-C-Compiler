#include "pch.h"
#include "JWasmGenerator.h"
#include "SymbolTable.h"
#include "types.h"

using namespace std;

JWasmGenerator::JWasmGenerator(ostream& os, int bits, bool isWindows) : out(os), bits(bits), indent(0), isWindows(isWindows) {}

void JWasmGenerator::generate(Program& program) { program.accept(*this); }

void JWasmGenerator::ind() { for (int i = 1; i <= indent; ++i) out << "  "; }

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
		tree.addDirective(AsmDirective::NONE1, "." + processor,"");
		tree.addDirective(AsmDirective::NONE1, "option segment:use16", "");
		tree.addDirective(AsmDirective::NONE1, ".model small, c;", "");
	}
	else if (bits == 32)
	{
		tree.addDirective(AsmDirective::NONE1, "." + processor,"");
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
	if (type == "int" && bits == 16) tree.addData(name, "SWORD", value, ";global var " + name + " type = " + type);
	if (type == "int" && bits == 32) tree.addData(name, "SDWORD", value, ";global var " + name + " type = " + type);
	if (type == "int" && bits == 64) tree.addData(name, "SDWORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 16) tree.addData(name, "WORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 32) tree.addData(name, "DWORD", value, ";global var " + name + " type = " + type);
	if (type == "unsigned int" && bits == 64) tree.addData(name, "DWORD", value, ";global var " + name + " type = " + type);
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

void JWasmGenerator::asmOutputVariables(string type,string name)
{
	if (type == "int" && bits == 16) out << "_" << name << ":SWORD";
	if (type == "int" && bits == 32) out << "_" << name << ":SDWORD";
	if (type == "int" && bits == 64) out << "_" << name << ":SDWORD";
	if (type == "unsigned int" && bits == 16) out << "_" << name << ":WORD";
	if (type == "unsigned int" && bits == 32) out << "_" << name << ":DWORD";
	if (type == "unsigned int" && bits == 64) out << "_" << name << ":DWORD";
}

void JWasmGenerator::outputFunctionHeader(FunctionDeclaration& function)
{
	int index = 0;
	out << "_" << function.name << " PROC ";
	for (auto& p : function.params)
	{
		asmOutputVariables(p.first, p.second);
		if (index < (int)function.params.size() - 1) out << ",";
		index++;

	}
	out << endl;
}

void JWasmGenerator::outputFunctionLocals(FunctionDeclaration& function)
{
	if (auto comp = dynamic_cast<CompoundStatement*>(function.body.get()))
	{
		int index = 0, idx = 0;
		int size = (int)comp->localDeclarations.size();
		if (size > 0)
		{
			ind(); out << "LOCAL ";
			for (auto& ld : comp->localDeclarations)
			{
				if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
				{
					localIndex[v->name] = { v->type, idx++ };
					asmOutputVariables(v->type, v->name);
					if (index < size - 1) out << ",";
					index++;
				}
			}
			out << endl;
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
	tree.addInstruction("movsx", AsmOperand(OPERAND_REGISTER, AsmRegisters::RAX, "", 0), AsmOperand(OPERAND_MEMORY, AsmRegisters::NONE, "_" +name, 0), comment);
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
	if (bits == 64) { ind(); out << "add " << regA(32) << ", " << regB(32) << comment << endl; }
	else if (bits == 32) { ind(); out << "add " << regA(32) << ", " << regB(32) << comment << endl; }
	else if (bits == 16) { ind(); out << "add " << regA(16) << ", " << regB(16) << comment << endl; }
}

void JWasmGenerator::asmStatementSubRegisterAAndRegisterB(string comment)
{
	if (bits == 64) { ind(); out << "sub " << regA(32) << ", " << regB(32) << comment << endl; }
	else if (bits == 32) { ind(); out << "sub " << regA(32) << ", " << regB(32) << comment << endl; }
	else if (bits == 16) { ind(); out << "sub " << regA(16) << ", " << regB(16) << comment << endl; }
}

void JWasmGenerator::asmStatementImulRegisterAAndRegisterB(string comment)
{
	if (bits == 64) { ind(); out << "imul " << regA(32) << ", " << regB(32) << comment << endl; }
	else if (bits == 32) { ind(); out << "imul " << regA(32) << ", " << regB(32) << comment << endl; }
	else if (bits == 16) { ind(); out << "imul " << regA(16) << ", " << regB(16) << comment << endl; }
}

void JWasmGenerator::asmStatementIdivRegisterAAndRegisterB()
{
	if (bits == 64)
	{
		ind(); out << "cdq \t\t\t\t;extend eax to edx:eax for idiv" << endl;
		ind(); out << "idiv " << regB(32) << "\t\t\t\t;integer divide registers A and B and store result in A" << endl;
	}
	if (bits == 32)
	{
		ind(); out << "cdq \t\t\t\t;extend eax to edx:eax for idiv" << endl;
		ind(); out << "idiv " << regB(32) << "\t\t\t\t;integer divide registers A and B and store result in A" << endl;
	}
	if (bits == 16)
	{
		ind(); out << "cwd \t\t\t\t;extend ax to dx:ax for idiv" << endl;
		ind(); out << "idiv " << regB(16) << "\t\t\t\t;integer divide registers AX and BX and store result in AX" << endl;
	}
}

void JWasmGenerator::outputReturnBinaryExpression(BinaryExpression* be)
{
	be->leftHandSide->accept(*this);
	be->rightHandSide->accept(*this);
	asmStatementPopRegisterB("\t\t\t;pop top of stack into register B");
	asmStatementPopRegisterA("\t\t\t;pop top of stack into register A");
	if (be->operator1 == '+')
	{
		asmStatementAddRegisterAAndRegisterB("\t\t\t;add registers A and B and store result in A");
	}
	else if (be->operator1 == '-')
	{
		asmStatementSubRegisterAAndRegisterB("\t\t\t;subtract register B from A and store result in A");
	}
	else if (be->operator1 == '*')
	{
		asmStatementImulRegisterAAndRegisterB("\t\t\t;multiply registers A and B and store result in A");
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
		ind(); out << "ret \t\t\t\t\t;return from function" << endl;
	}
	else
	{
		ind(); out << "ret \t\t\t\t;return from function" << endl;
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
		asmStatementMoveImmediateToRegisterA("int", to_string(n.value), "\t\t;load immediate into register");
		asmStatementPushRegisterA("\t\t\t;push register A on to the stack");
	}
	else if (bits == 32)
	{
		asmStatementMoveImmediateToRegisterA("int", to_string(n.value), "\t\t;load immediate into register");
		asmStatementPushRegisterA("\t\t\t;push register A on to the stack");
	}
	else
	{
		asmStatementMoveImmediateToRegisterA("int", to_string(n.value), "\t\t;load immediate into register");
		asmStatementPushRegisterA("\t\t\t;push register A on to the stack");
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
			asmStatementMoveSignExtendVariableToRegisterA(it->second.type, it->first, "\t\t;load and sign extend '" + it->first + "' in to register A");
			asmStatementPushRegisterA("\t\t\t;push register A on to stack");
		}
		else
		{
			asmStatementMoveVariableToRegisterA(it->second.type, it->first, "\t\t;load parameter " + it->first);
			asmStatementPushRegisterA("\t\t\t;push register A on to stack");
		}
	}
	else if (itLocal != localIndex.end())
	{
		if (itLocal->second.type == "int" && bits == 64)
		{
			asmStatementMoveSignExtendVariableToRegisterA(it->second.type, itLocal->first, "\t\t;load and sign extend '" + itLocal->first + "' in to register A");
			asmStatementPushRegisterA("\t\t\t;push register A on to stack");
		}
		else
		{
			asmStatementMoveVariableToRegisterA(itLocal->second.type, itLocal->first, "\t\t;load parameter " + itLocal->first);
			asmStatementPushRegisterA("\t\t\t;push register A on to stack");
		}
	}
	else
	{
		out << ";?? '" << expression.name << "'" << endl;
	}
}

void JWasmGenerator::asmStatementPopRegisterA(string comment)
{
	if (bits == 64)
	{
		ind(); 	out << "pop " << regA(64) << comment << endl;
	}
	else if (bits == 32)
	{
		ind(); 	out << "pop " << regA(32) << comment << endl;
	}
	else
	{
		ind(); 	out << "pop " << regA(16) << comment << endl;
	}
}

void JWasmGenerator::asmStatementPushRegisterA(string comment)
{
	if (bits == 64)
	{
		ind(); 	out << "push " << regA(64) << comment << endl;
	}
	else if (bits == 32)
	{
		ind(); 	out << "push " << regA(32) << comment << endl;
	}
	else
	{
		ind(); 	out << "push " << regA(16) << comment << endl;
	}
}

void JWasmGenerator::asmStatementPopRegisterB(string comment)
{
	if (bits == 64)
	{
		ind(); 	out << "pop " << regB(64) << comment << endl;
	}
	else if (bits == 32)
	{
		ind(); 	out << "pop " << regB(32) << comment << endl;
	}
	else
	{
		ind(); 	out << "pop " << regB(16) << comment << endl;
	}
}

void JWasmGenerator::asmStatementPushRegisterB(string comment)
{
	if (bits == 64)
	{
		ind(); 	out << "push " << regB(64) << comment << endl;
	}
	else if (bits == 32)
	{
		ind(); 	out << "push " << regB(32) << comment << endl;
	}
	else
	{
		ind(); 	out << "push " << regB(16) << comment << endl;
	}
}

void JWasmGenerator::visit(BinaryExpression& be)
{
	be.leftHandSide->accept(*this);
	be.rightHandSide->accept(*this);
	asmStatementPopRegisterB("\t\t\t;pop top of stack into register B");
	asmStatementPopRegisterA("\t\t\t;pop top of stack into register A");
	if (be.operator1 == '+')
	{
		asmStatementAddRegisterAAndRegisterB("\t\t\t;add registers A and B and store result in A");
	}
	else if (be.operator1 == '-')
	{
		asmStatementSubRegisterAAndRegisterB("\t\t\t;subtract register B from A and store result in A");
	}
	else if (be.operator1 == '*')
	{
		asmStatementImulRegisterAAndRegisterB("\t\t\t;multiply registers A and B and store result in A");
	}
	else if (be.operator1 == '/')
	{
		asmStatementIdivRegisterAAndRegisterB();
	}
	asmStatementPushRegisterA("\t\t\t;push register A on to the stack");
}

void JWasmGenerator::visit(AssignExpression& n)
{
	// evaluate value then set_local
	n.value->accept(*this); // pushes value
	// pop into regA and store
	out << "pop " << regA(bits) << "\t\t\t;pop value into register 1" << endl;
	auto it = paramIndex.find(n.name);
	auto itLocal = localIndex.find(n.name);
	if (it != paramIndex.end())
	{
		asmStatementMoveRegisterAToVariable(it->second.type, it->first, "\t\t;store parameter " + it->first);
		out << "mov " << n.name << ", " << regA(bits) << "\t\t\t; store parameter" << n.name << endl;
	}
	else if (itLocal != localIndex.end())
	{
		asmStatementMoveRegisterAToVariable(itLocal->second.type, itLocal->first, "\t\t;store local " + itLocal->first);
		out << "mov _" << n.name << ", " << regA(bits) << "\t\t\t; store local " << n.name << endl;
	}
	else
	{
		out << "; store to global " << n.name << endl;
	}
}

void JWasmGenerator::visit(CallExpression& n)
{

}
