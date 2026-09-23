#include "pch.h"
#include "JWasmGenerator.h"
#include "SymbolTable.h"


using namespace std;

int JWasmGenerator::nextLocalIndex()
{
	int idx = 0;
	for (auto& kv : paramIndex) idx = max(idx, kv.second.index);
	return idx + 1;
}

string JWasmGenerator::wordForBits() const
{
	if (bits == 64) return "qword";
	if (bits == 16) return "word";
	return "dword";
}

string JWasmGenerator::regA(int size) const {
	if (size == 64) return "rax";
	if (size == 16) return "ax";
	return "eax";
}

string JWasmGenerator::regC(int size) const {
	if (size == 64) return "rcx";
	if (size == 16) return "cx";
	return "ecx";
}
string JWasmGenerator::regB() const {
	if (bits == 64) return "rbx";
	if (bits == 16) return "bx";
	return "ebx";
}

string JWasmGenerator::wasmType(const string& ty) const
{
	if (ty == "int")
	{
		if (bits == 16) return "i16"; // placeholder
		if (bits == 64) return "i64";
		return "i32";
	}
	return "i32";
}

void JWasmGenerator::visit(Program& program)
{
	if (bits == 16)
	{
		out << ".286" << endl;
		out << ".model small;" << endl;
	}
	else if (bits == 32)
	{
		out << ".386" << endl;
		out << ".model flat, c;" << endl;
	}
	else
	{
		out << ".x64p" << endl;
	}
	out << "option casemap : none" << endl;

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

	out << endl << ".code" << endl;
	for (auto& d : program.declarations)
	{
		if (auto gv = dynamic_cast<FunctionDeclaration*>(d.get()))
		{
			gv->accept(*this);
		}
	}
	out << "end" << endl;
}

void JWasmGenerator::visit(VariableDeclaration& n)
{
	//ind(); out << "mov _" << n.name << ", " << regA(32) << " ;var decl " << n.name << " type = " << n.type << endl;
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
					preparingFunctionParms = true;
					exp->accept(*this);
					ind(); out << "mov _" << v->name << ", " << regA(32) << "\t\t\t;var decl " << v->name << " type = " << v->type << endl;
					if (v->type == "int" && bits == 64) { ind(); out << "movsxd " << regA(bits) << ", _" << v->name << "\t\t;load and sign extend '" << v->name << "' in to register 1\n"; }
					if (v->type == "int" && bits == 32) { ind(); out << "mov " << regA(bits) << ", _" << v->name << "\t\t\t;load '" << v->name << "' in to register 2\n"; }
					ind(); out << "push " << regA(64) << "\t\t\t\t;push result onto stack" << endl;
					preparingFunctionParms = false;
				}
				else if (auto exp = dynamic_cast<Expression*>(v->init.get()))
				{
					exp->accept(*this);
					if (v->type == "int" && bits == 64) { ind(); out << "movsxd " << regA(bits) << ", _" << v->name << "\t\t;assign decl " << v->name << " type = " << v->type << endl; }
					if (v->type == "int" && bits == 32) { ind(); out << "mov " << regA(bits) << ", _" << v->name << "\t\t\t;assign decl " << v->name << " type = " << v->type << endl; }
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
		out << ";----------------------------------------------------" << endl;
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
{}

void JWasmGenerator::visit(ReturnStatement& n)
{
	if (n.expr)
	{
		n.expr->accept(*this);
		ind(); out << "ret \t\t\t\t;return 1" << endl;
	}
	else
	{
		ind(); out << "ret \t\t\t\t;return 2" << endl;
	}
}

void JWasmGenerator::visit(ExpressionStatement& n)
{
	if (n.expr) n.expr->accept(*this);
}

void JWasmGenerator::visit(NumberExpression& n)
{
	// push immediate into register/stack
	ind();
	if (bits == 64) out << "mov " << regA(bits) << ", " << n.value << "\t\t;load immediate into register\n";
	else out << "mov " << regA(bits) << ", " << n.value << "\t\t;load immediate into register\n";
	// push onto stack to follow simple eval convention
	ind(); out << "push " << regA(bits) << "\t\t\t;push register A on to the stack\n";
}

void JWasmGenerator::visit(VariableExpression& expression)
{
	auto it = paramIndex.find(expression.name);
	auto itLocal = localIndex.find(expression.name);
	if (it != paramIndex.end())
	{
		ind();
		if (it->second.type == "int" && bits == 64) out << "movsxd " << regA(bits) << ", " << expression.name << "\t\t;load and sign extend '" << expression.name << "' in to register 1\n";
		if (it->second.type == "int" && bits == 32) out << "mov " << regA(bits) << ", " << expression.name << "\t\t\t;load '" << expression.name << "' in to register 2\n";
		ind();
		out << "push " << regA(bits) << "\t\t\t;push 4 on to stack\n";
	}
	else if (itLocal != localIndex.end())
	{
		if (itLocal->second.type == "int" && preparingFunctionParms)
		{
			ind(); out << "mov " << regA(32) << ", _" << expression.name << "\t\t\t;load local '" << expression.name << "' in to register A\n";
		}
	}
	else
	{
		ind(); out << "mov " << regA(32) << ", " << expression.name << "\t\t\t;load global '" << expression.name << "' in to register A\n";
		ind(); out << "push " << regA(bits) << "\t\t\t;push register A on to the stack\n";
	}
}

void JWasmGenerator::visit(BinaryExpression& expression)
{
	expression.leftHandSide->accept(*this);
	expression.rightHandSide->accept(*this);
	ind();  out << "pop " << regB() << "\t\t\t;pop rhs into register B\n";
	ind();  out << "pop " << regA(bits) << "\t\t\t;pop lhs into register A\n";
	if (expression.operator1 == '+') { ind(); out << "add " << regA(bits) << ", " << regB() << "\t\t;add registers A and B and store result in A\n"; }
	else if (expression.operator1 == '-') { ind(); out << "sub " << regA(bits) << ", " << regB() << "\t\t;subtract register B from A and store result in A\n"; }
	else if (expression.operator1 == '*') { ind(); out << "imul " << regA(bits) << ", " << regB() << "\t\t;multiply registers A and B and store result in A\n"; }
	else if (expression.operator1 == '/')
	{
		ind(); out << "cdq \t\t\t\t;extend eax to edx:eax for idiv" << endl;
		ind(); out << "idiv " << regB() << "\t\t\t;integer divide registers A and B and store result in A\n";
	}
	// push result
	ind(); 	out << "push " << regA(bits) << "\t\t\t;push register A on to the stack\n";
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
	ind(); out << "invoke _" << n.callee;
	for (auto& it : n.args)
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
	//ind(); out << "push " << regA(bits) << " ;push register A on to the stack\n";
	out << "\t;invoke function '" << n.callee << "' with " << n.args.size() << " arguments" << endl;
}
