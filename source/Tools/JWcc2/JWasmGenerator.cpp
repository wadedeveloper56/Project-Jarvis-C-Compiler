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

void JWasmGenerator::visit(FunctionDeclaration& n)
{
	paramIndex.clear();
	localIndex.clear();

	int idx = 0;
	for (auto& p : n.params)
	{
		paramIndex[p.second] = { p.first, idx++ };
	}

	// params (MASM uses stack-based params; emit comments and locals block)
	out << ";-----------------\n";
	out << "; params:\n";
	if (!n.params.empty())
	{
		for (auto& p : n.params)
		{
			out << ";   " << p.second << " : " << p.first << "\n";
		}
	}
	out << "; locals:\n";
	if (n.body)
	{
		if (auto comp = dynamic_cast<CompoundStatement*>(n.body.get()))
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

	//proc header
	out << "_" << n.name << " PROC ";
	int index = 0;
	int size = (int)n.params.size();
	for (auto& p : n.params)
	{
		if (p.first == "int") out << p.second << ":SDWORD";
		if (index < (int)n.params.size() - 1) out << ",";
		index++;

	}
	out << endl;

	//proc body
	//local variables def
	idx = 0;
	indent++;
	if (auto comp = dynamic_cast<CompoundStatement*>(n.body.get()))
	{
		int size = (int)comp->localDeclarations.size();
		ind();
		out << "LOCAL ";
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

	//local variables init
	if (auto comp = dynamic_cast<CompoundStatement*>(n.body.get()))
	{
		index = 0;
		int size = (int)comp->localDeclarations.size();
		for (auto& ld : comp->localDeclarations)
		{
			if (auto v = dynamic_cast<VariableDeclaration*>(ld.get()))
			{
				if (auto exp = dynamic_cast<CallExpression*>(v->init.get()))
				{
					preparingFunctionParms = true;
					exp->accept(*this);
					ind(); out << "mov _" << v->name << ", " << regA(32) << " ;var decl " << v->name << " type = " << v->type << endl;
					if (v->type == "int" && bits == 64) { ind(); out << "movsxd " << regA(bits) << ", _" << v->name << " ;load and sign extend '" << v->name << "' in to register 1\n"; }
					if (v->type == "int" && bits == 32) { ind(); out << "mov " << regA(bits) << ", _" << v->name << " ;load '" << v->name << "' in to register 2\n"; }
					ind(); out << "push " << regA(64) << " ; push result onto stack" << endl;
					preparingFunctionParms = false;
				}
				else if (auto exp = dynamic_cast<Expression*>(v->init.get()))
				{
					exp->accept(*this);
					ind();  out << ";assign decl " << v->name << " type = " << v->type << endl;
				}
			}
		}
	}

	if (n.body) n.body->accept(*this);

	indent--;
	//proc trailer
	out << "_" << n.name << " ENDP" << endl << endl;
}

void JWasmGenerator::visit(CompoundStatement& n)
{
	for (auto& d : n.localDeclarations)
	{
		d->accept(*this);
	}
	for (auto& s : n.stmts)
	{
		s->accept(*this);
	}
}

void JWasmGenerator::visit(ReturnStatement& n)
{
	if (n.expr)
	{
		n.expr->accept(*this);
		ind(); out << "ret ;return 1" << endl;
	}
	else
	{
		ind(); out << "ret ;return 2" << endl;
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
	if (bits == 64) out << "mov " << regA(bits) << ", " << n.value << " ;load immediate into register\n";
	else out << "mov " << regA(bits) << ", " << n.value << " ;load immediate into register\n";
	// push onto stack to follow simple eval convention
	ind(); out << "push " << regA(bits) << " ;push register A on to the stack\n";
}

void JWasmGenerator::visit(VariableExpression& n)
{
	auto it = paramIndex.find(n.name);
	auto itLocal = localIndex.find(n.name);
	if (it != paramIndex.end())
	{
		ind();
		if (it->second.type == "int" && bits == 64) out << "movsxd " << regA(bits) << ", " << n.name << " ;load and sign extend '" << n.name << "' in to register 1\n";
		if (it->second.type == "int" && bits == 32) out << "mov " << regA(bits) << ", " << n.name << " ;load '" << n.name << "' in to register 2\n";
		ind();
		out << "push " << regA(bits) << " ;push 4 on to stack\n";
	}
	else if (itLocal != localIndex.end())
	{
		if (itLocal->second.type == "int" && preparingFunctionParms)
		{
			ind(); out << "mov " << regA(32) << ", _" << n.name << " ;load local '" << n.name << "' in to register A\n";
		}
	}
	else
	{
		ind(); out << "mov " << regA(32) << ", " << n.name << " ;load global '" << n.name << "' in to register A\n";
		ind(); out << "push " << regA(bits) << " ;push register A on to the stack\n";
	}
}

void JWasmGenerator::visit(BinaryExpression& n)
{
	// evaluate lhs and rhs, then emit op
	n.lhs->accept(*this); // pushes lhs
	n.rhs->accept(*this); // pushes rhs
	// pop rhs into regB, pop lhs into regA
	ind();  out << "pop " << regB() << " ;pop rhs into register B\n";
	ind();  out << "pop " << regA(bits) << " ;pop lhs into register A\n";
	if (n.op == '+') { ind(); out << "add " << regA(bits) << ", " << regB() << " ;add registers A and B and store result in A\n"; }
	else if (n.op == '-') { ind(); out << "sub " << regA(bits) << ", " << regB() << " ;subtract register B from A and store result in A\n"; }
	else if (n.op == '*') { ind(); out << "imul " << regA(bits) << ", " << regB() << " ;multiply registers A and B and store result in A\n"; }
	else if (n.op == '/')
	{
		ind(); out << "cdq ;extend eax to edx:eax for idiv" << endl;
		ind(); out << "idiv " << regB() << " ;integer divide registers A and B and store result in A\n";
	}
	// push result
	ind(); 	out << "push " << regA(bits) << " ;push register A on to the stack\n";
}

void JWasmGenerator::visit(AssignExpression& n)
{
	// evaluate value then set_local
	n.value->accept(*this); // pushes value
	// pop into regA and store
	out << "pop " << regA(bits) << " ;pop value into register 1\n";
	auto it = paramIndex.find(n.name);
	auto itLocal = localIndex.find(n.name);
	if (it != paramIndex.end())
	{
		out << "mov " << n.name << ", " << regA(bits) << " ; store parameter" << n.name << "\n";
	}
	else if (itLocal != localIndex.end())
	{
		out << "mov _" << n.name << ", " << regA(bits) << " ; store local " << n.name << "\n";
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
	out << " ;invoke function '" << n.callee << "' with " << n.args.size() << " arguments" << endl;
}
