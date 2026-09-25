#include "pch.h"
#include "JWasmGenerator.h"
#include "SymbolTable.h"
#include "types.h"

using namespace std;

JWasmGenerator::JWasmGenerator(ostream& os, int bits, bool isWindows) : out(os), bits(bits), indent(0), isWindows(isWindows) {}

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
		out << "." << processor << endl;
		out << "option segment:use16" << endl;
		out << ".model small, c;" << endl;
	}
	else if (bits == 32)
	{
		out << "." << processor << endl;
		out << "option segment:use32" << endl;
		out << ".model flat, c;" << endl;
	}
	else
	{
		out << ".x64p" << endl;
	}
	out << "option casemap : none" << endl;
}

void JWasmGenerator::outputProgramUninitializedData(Program& program)
{
	// uninitialized data section for globals
	out << endl << ".data?" << endl;
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<VariableDeclaration*>(d.get()))
		{
			auto init = dynamic_cast<Expression*>(gv->init.get());
			if (init == nullptr)
			{
				if (gv->type == "int")	out << gv->name << " SDWORD ? ;global var " << gv->name << " type = " << gv->type << "\n";
			}
		}
	}
}

void JWasmGenerator::outputProgramInitializedData(Program& program)
{
	// initialized data section for globals
	out << endl << ".data" << endl;
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<VariableDeclaration*>(d.get()))
		{
			if (auto init = dynamic_cast<Expression*>(gv->init.get()))
			{
				if (auto expr = dynamic_cast<NumberExpression*>(init))
				{
					if (gv->type == "int")	out << gv->name << " SDWORD " << expr->value << " ;global var " << gv->name << " type = " << gv->type << " value = " << expr->value << endl;
				}
			}
		}
	}
}

void JWasmGenerator::outputProgramCode(Program& program)
{
	out << endl << ".code" << endl;
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
	out << "end" << endl;
}

void JWasmGenerator::visit(VariableDeclaration& n)
{
}

void JWasmGenerator::outputFunctionComment(FunctionDeclaration& function)
{
	out << ";-----------------\n";
	out << "; params:\n";
	if (!function.params.empty())
	{
		for (auto& p : function.params)
		{
			out << ";   " << p.second << " : " << p.first << "\n";
		}
	}
	out << "; locals:\n";
	if (function.body)
	{
		if (auto comp = dynamic_cast<CompoundStatement*>(function.body.get()))
		{
			for (auto& ld : comp->localDeclarations)
			{
				if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
				{
					out << ";   " << v->name << " : " << v->type << "\n";
				}
			}
		}
	}
	out << ";-----------------\n";
}

void JWasmGenerator::outputFunctionHeader(FunctionDeclaration& function)
{
	int index = 0;
	out << "_" << function.name << " PROC ";
	for (auto& p : function.params)
	{
		if (p.first == "int") out << p.second << ":SDWORD";
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
		ind(); out << "LOCAL ";
		for (auto& ld : comp->localDeclarations)
		{
			if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
			{
				localIndex[v->name] = { v->type, idx++ };
				if (v->type == "int") out << "_" << v->name << ":SDWORD";
				if (index < size - 1) out << ",";
				index++;
			}
		}
		out << endl;
	}
}

void JWasmGenerator::moveResultOfInvokeIntoRegisterA(VariableDeclaration* v)
{
	auto type = v->type;
	if (processorBitType == ProcessorBitType::BIT16_8086 || processorBitType == ProcessorBitType::BIT16_186 || processorBitType == ProcessorBitType::BIT16_286)
	{
        auto reg = (type == "int") ? regA(32) : regA(16);
		ind(); out << "mov _" << v->name << ", " << reg << "\t\t\t;move result of invoke from register AX to variable '" << v->name << "'" << endl;
		ind(); out << "push " << reg << "\t\t\t\t;push the result in register A onto stack" << endl;
	}
	else if (processorBitType == ProcessorBitType::BIT16_386 || processorBitType == ProcessorBitType::BIT16_486 || processorBitType == ProcessorBitType::BIT16_586 || processorBitType == ProcessorBitType::BIT16_686)
	{
		auto reg = (type == "int") ? regA(32) : regA(16);
		ind(); out << "mov _" << v->name << ", " << reg << "\t\t\t;move result of invoke from register EAX to variable '" << v->name << "'" << endl;
		ind(); out << "push " << reg << "\t\t\t\t;push the result in register A onto stack" << endl;
	}
	else if (processorBitType == ProcessorBitType::BIT32_386 || processorBitType == ProcessorBitType::BIT32_486 || processorBitType == ProcessorBitType::BIT32_586 || processorBitType == ProcessorBitType::BIT32_686)
	{
		auto reg = (type == "int") ? regA(32) : regA(16);
		ind(); out << "mov _" << v->name << ", " << reg << "\t\t\t;move result of invoke from register EAX to variable '" << v->name << "'" << endl;
		ind(); out << "push " << reg << "\t\t\t\t;push the result in register A onto stack" << endl;
	}
	else if (processorBitType == ProcessorBitType::BIT64_x64)
	{
		auto reg = (type == "int") ? regA(32) : regA(16);
		ind(); out << "mov _" << v->name << ", " << reg << "\t\t\t;move result of invoke from register RAX to variable '" << v->name << "'" << endl;
		//ind(); out << "push " << regA(64) << "\t\t\t\t;push the result in register RAX onto stack" << endl;
	}
}

void JWasmGenerator::outputFunctionLocalsInitializationFunctionCall(CallExpression* exp)
{
	ind(); out << "invoke _" << exp->callee;
	for (auto& it : exp->args)
	{
		if (auto exp = dynamic_cast<VariableExpression*>(it.get()))
		{
			out << ", " << exp->name;
		}
		else if (auto exp = dynamic_cast<NumberExpression*>(it.get()))
		{
			out << ", " << exp->value;
		}
	}
	out << "\t;invoke function '" << exp->callee << "' with " << exp->args.size() << " arguments" << endl;
}

void JWasmGenerator::outputFunctionLocalsInitialization(FunctionDeclaration& function)
{
	out << ";----------- Local variable initialization ----------" << endl;
	if (auto comp = dynamic_cast<CompoundStatement*>(function.body.get()))
	{
		for (auto& ld : comp->localDeclarations)
		{
			if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
			{
				if (auto exp = dynamic_cast<CallExpression*>(v->init.get()))
				{
					outputFunctionLocalsInitializationFunctionCall(exp);
					moveResultOfInvokeIntoRegisterA(v);
				}
				else if (auto exp = dynamic_cast<Expression*>(v->init.get()))
				{
					if (auto be = dynamic_cast<BinaryExpression*>(exp))
					{
						be->accept(*this);
					}
					else
					{
						exp->accept(*this);
					}
					if (v->type == "int") { ind(); out << "mov _" << v->name << ", " << regA(32) << "\t\t;move result of binary expression from register A in to " << v->name << endl; }
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
		out << ";----------Handle Function Body Statements-----------" << endl;
		for (auto& statement : body.statements)
		{
			statement->accept(*this);
		}
		out << ";----------------------------------------------------" << endl;
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
	out << "_" << function.name << " ENDP" << endl << endl;
}

void JWasmGenerator::visit(CompoundStatement& n)
{
}

void JWasmGenerator::outputReturnBinaryExpression(BinaryExpression* be)
{
	be->leftHandSide->accept(*this);
	be->rightHandSide->accept(*this);
	ind();  out << "pop " << regB(bits) << "\t\t\t\t;pop top of stack into register B\n";
	ind();  out << "pop " << regA(bits) << "\t\t\t\t;pop top of stack into register A\n";
	if (be->operator1 == '+')
	{
		ind(); out << "add " << regA(bits) << ", " << regB(bits) << "\t\t\t;add registers A and B and store result in A\n";
	}
	else if (be->operator1 == '-')
	{
		ind(); out << "sub " << regA(bits) << ", " << regB(bits) << "\t\t\t;subtract register B from A and store result in A\n";
	}
	else if (be->operator1 == '*')
	{
		ind(); out << "imul " << regA(bits) << ", " << regB(bits) << "\t\t\t;multiply registers A and B and store result in A\n";
	}
	else if (be->operator1 == '/')
	{
		ind(); out << "cdq \t\t\t\t;extend eax to edx:eax for idiv" << endl;
		ind(); out << "idiv " << regB(bits) << "\t\t\t\t;integer divide registers A and B and store result in A\n";
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
		ind(); out << "mov " << regA(64) << ", " << n.value << "\t\t;load immediate into register\n";
		ind(); out << "push " << regA(64) << "\t\t\t;push register A on to the stack\n";
	}
	else
	{
		ind(); out << "mov " << regA(32) << ", " << n.value << "\t\t;load immediate into register\n";
		ind(); out << "push " << regA(32) << "\t\t\t;push register A on to the stack\n";
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
			ind(); out << "movsxd " << regA(64) << ", " << expression.name << "\t\t;load and sign extend '" << expression.name << "' in to register A\n";
			ind(); out << "push " << regA(64) << "\t\t\t;push register A on to stack\n";
		}
		else
		{
			ind(); out << "mov " << regA(32) << ", " << expression.name << "\t\t\t;load '" << expression.name << "' in to register A\n";
			ind(); out << "push " << regA(32) << "\t\t\t;push register A on to stack\n";
		}
	}
	else if (itLocal != localIndex.end())
	{
		if (itLocal->second.type == "int" && bits == 64)
		{
			ind(); out << "movsxd " << regA(64) << ", _" << expression.name << "\t\t;load and sign extend '" << expression.name << "' in to register A\n";
			ind(); out << "push " << regA(64) << "\t\t\t;push register A on to stack\n";
		}
		else
		{
			ind(); out << "mov " << regA(32) << ", _" << expression.name << "\t\t\t;load '" << expression.name << "' in to register A\n";
			ind(); out << "push " << regA(32) << "\t\t\t;push register A on to stack\n";
		}
	}
	else
	{
		ind(); out << "mov " << regA(32) << ", _" << expression.name << "\t\t\t;load global '" << expression.name << "' in to register A\n";
		ind(); out << "push " << regA(bits) << "\t\t\t;push register A on to the stack\n";
	}
}

void JWasmGenerator::visit(BinaryExpression& be)
{
	be.leftHandSide->accept(*this);
	be.rightHandSide->accept(*this);
	if (bits == 64)
	{
		ind();  out << "pop " << regB(64) << "\t\t\t;pop top of stack into register B\n";
		ind();  out << "pop " << regA(64) << "\t\t\t;pop top of stack into register A\n";
	}
	else
	{
		ind();  out << "pop " << regB(32) << "\t\t\t;pop top of stack into register B\n";
		ind();  out << "pop " << regA(32) << "\t\t\t;pop top of stack into register A\n";
	}
	if (be.operator1 == '+')
	{
		ind(); out << "add " << regA(32) << ", " << regB(32) << "\t\t;add registers A and B and store result in A\n";
	}
	else if (be.operator1 == '-')
	{
		ind(); out << "sub " << regA(32) << ", " << regB(32) << "\t\t;subtract register B from A and store result in A\n";
	}
	else if (be.operator1 == '*')
	{
		ind(); out << "imul " << regA(32) << ", " << regB(32) << "\t\t;multiply registers A and B and store result in A\n";
	}
	else if (be.operator1 == '/')
	{
		ind(); out << "cdq \t\t\t\t;signed 32-bit value in the EAX register and sign-extends it into the 64-bit pair EDX:EAX" << endl;
		ind(); out << "idiv " << regB(32) << "\t\t\t;divides the 64-bit value in EDX:EAX by EBX - quotient is stored in EAX -remainder is stored in EDX\n";
	}
	if (bits == 64)
	{
		ind(); 	out << "push " << regA(64) << "\t\t\t;push register RAX on to the stack\n";
	}
	else
	{
		ind(); 	out << "push " << regA(32) << "\t\t\t;push register EAX on to the stack\n";
	}
}

void JWasmGenerator::visit(AssignExpression& n)
{
	// evaluate value then set_local
	n.value->accept(*this); // pushes value
	// pop into regA and store
	out << "pop " << regA(bits) << "\t\t\t;pop value into register 1\n";
	auto it = paramIndex.find(n.name);
	auto itLocal = localIndex.find(n.name);
	if (it != paramIndex.end())
	{
		out << "mov " << n.name << ", " << regA(bits) << "\t\t\t; store parameter" << n.name << "\n";
	}
	else if (itLocal != localIndex.end())
	{
		out << "mov _" << n.name << ", " << regA(bits) << "\t\t\t; store local " << n.name << "\n";
	}
	else
	{
		out << "; store to global " << n.name << endl;
	}
}

void JWasmGenerator::visit(CallExpression& n)
{

}
