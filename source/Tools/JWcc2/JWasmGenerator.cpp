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

string JWasmGenerator::regA() const {
	if (bits == 64) return "rax";
	if (bits == 16) return "ax";
	return "eax";
}

string JWasmGenerator::regB() const {
	if (bits == 64) return "rbx";
	if (bits == 16) return "bx";
	return "ebx";
}

void JWasmGenerator::emitPrologue(int localBytes)
{
	// For 32-bit and 16-bit, use ebp frame
	if (bits == 32 || bits == 16)
	{
		//ind(); out << "push ebp\n";
		//ind(); out << "mov ebp, esp\n";
		//// save callee-saved registers (ebx, esi, edi)
		//ind(); out << "push ebx\n";
		//ind(); out << "push esi\n";
		//ind(); out << "push edi\n";
		//if (localBytes > 0) { ind(); out << "sub esp, " << localBytes << "\n"; }
	}
	else if (bits == 64)
	{
		//// For x64, follow chosen calling convention: save callee-saved (rbx, rsi, rdi, rbp, r12-r15 as needed)
		//ind(); out << "push rbp\n";
		//ind(); out << "mov rbp, rsp\n";
		//// save some callee-saved registers (rbx, rsi, rdi)
		//ind(); out << "push rbx\n";
		//ind(); out << "push rsi\n";
		//ind(); out << "push rdi\n";
		//if (localBytes > 0) { ind(); out << "sub rsp, " << localBytes << "\n"; }
	}
}

void JWasmGenerator::emitEpilogue()
{
	if (bits == 32 || bits == 16)
	{
		//if (inFunction)
		//{
		//	// restore stack
		//	ind(); out << "mov esp, ebp\n";
		//	ind(); out << "pop edi\n";
		//	ind(); out << "pop esi\n";
		//	ind(); out << "pop ebx\n";
		//	ind(); out << "pop ebp\n";
		//	ind(); out << "ret\n";
		//}
	}
	else if (bits == 64)
	{
		//if (inFunction)
		//{
		//	ind(); out << "add rsp, 0 ; adjust (if locals allocated)\n";
		//	ind(); out << "pop rdi\n";
		//	ind(); out << "pop rsi\n";
		//	ind(); out << "pop rbx\n";
		//	ind(); out << "pop rbp\n";
		//	ind(); out << "ret\n";
		//}
	}
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

void JWasmGenerator::visit(Program& n)
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
		//out << ".model flat, c;" << endl;
	}
	out << "option casemap : none" << endl;

	// data section for globals
	out << endl << ".data?" << endl;
	for (auto& d : n.decls)
	{
		if (auto gv = dynamic_cast<VarDecl*>(d.get()))
		{
			auto init = dynamic_cast<Expr*>(gv->init.get());
			if (init == nullptr)
			{
				if (gv->type == "int")	out << gv->name << " SDWORD ? ; global var " << gv->name << " type = " << gv->type << "\n";
			}
		}
	}

	// data section for globals
	out << endl << ".data" << endl;
	for (auto& d : n.decls)
	{
		if (auto gv = dynamic_cast<VarDecl*>(d.get()))
		{
			if (auto init = dynamic_cast<Expr*>(gv->init.get()))
			{
				if (auto expr = dynamic_cast<NumberExpr*>(init))
				{
					if (gv->type == "int")	out << gv->name << " SDWORD " << expr->value << "; global var " << gv->name << " type = " << gv->type << " value = " << expr->value << endl;
				}
			}
		}
	}

	out << endl << ".code" << endl;
	for (auto& d : n.decls)
	{
		if (auto gv = dynamic_cast<FunctionDecl*>(d.get()))
		{
			gv->accept(*this);
		}
	}
	out << "end" << endl;
}

void JWasmGenerator::visit(VarDecl& n)
{
	ind();
}

void JWasmGenerator::visit(FunctionDecl& n)
{
	paramIndex.clear();
	//currentParams.clear();
	//currentLocals.clear();
	//inFunction = true;
	// assign indices for params starting at 0
	int idx = 0;
	for (auto& p : n.params)
	{
		paramIndex[p.second] = { p.first, idx++ };
	}

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
	if (auto comp = dynamic_cast<CompoundStmt*>(n.body.get()))
	{
		index = 0;
		int size = (int)comp->localDecls.size();
		for (auto& ld : comp->localDecls)
		{
			if (auto v = dynamic_cast<VarDecl*>(ld.get()))
			{
				ind();
				out << "LOCAL ";
				if (v->type == "int") out << "_" << v->name << ":SDWORD";
				if (index < size - 1) out << ",";
				index++;
			}
		}
		out << endl;
	}

	//local variables init
	if (auto comp = dynamic_cast<CompoundStmt*>(n.body.get()))
	{
		index = 0;
		int size = (int)comp->localDecls.size();
		for (auto& ld : comp->localDecls)
		{
			if (auto v = dynamic_cast<VarDecl*>(ld.get()))
			{
				if (auto exp = dynamic_cast<Expr*>(v->init.get()))
				{
					exp->accept(*this);
				}
			}
		}
		out << endl;
	}

	indent++;
	if (n.body) n.body->accept(*this);
	indent--;

	//proc trailer
	out << "_" << n.name << " ENDP" << endl << endl;



	/*
	// reset per-function locals map
	localIndex.clear();
	currentParams.clear();
	currentLocals.clear();
	inFunction = true;
	// assign indices for params starting at 0
	int idx = 0;
	for (auto& p : n.params)
	{
		localIndex[p.second] = idx++;
	}
	// params (MASM uses stack-based params; emit comments and locals block)
	out << ";-----------------\n";
	if (!n.params.empty())
	{
		ind(); out << "; params:\n";
		for (auto& p : n.params)
		{
			ind(); out << ";   " << p.second << " : " << p.first << "\n";
			currentParams.push_back(p.second);
			localIndex[p.second] = idx++;
		}
	}
	// collect locals
	int localCountStart = idx;
	if (n.body)
	{
		if (auto comp = dynamic_cast<CompoundStmt*>(n.body.get()))
		{
			for (auto& ld : comp->localDecls)
			{
				if (auto v = dynamic_cast<VarDecl*>(ld.get()))
				{
					currentLocals.push_back(v);
					localIndex[v->name] = idx++;
				}
			}
		}
	}
	if (!currentLocals.empty())
	{
		ind(); out << "; locals:\n";
		for (auto& ln : currentLocals)
		{
			ind(); out << ";   " << ln->name << " : " << "int" << "\n";
		}
	}
	out << ";-----------------\n";
	// emit MASM proc header
	out << "_" << n.name << " PROC";
	for (auto& p : n.params)
	{
		if (p.first == "int")	out << "," << p.second << ":SDWORD";

	}
	out << endl;
	if (!currentLocals.empty())
	{
		ind();
		out << "      LOCAL ";
		int size = (int)currentLocals.size();
		int idx = 0;
		for (auto& ln : currentLocals)
		{
			if (ln->type == "int") out << "_" << ln->name << ":SDWORD";
			if (idx < size - 1) out << ",";
			idx++;
		}
		out << endl;
	}
	// body prologue
	int totalLocalBytes = (idx - localCountStart) * wordBytes();
	emitPrologue(totalLocalBytes);
	// no-op patch: preserve behavior, adjust history
	indent++;
	if (n.body) n.body->accept(*this);
	indent--;
	emitEpilogue();
	out << "_" << n.name << " ENDP" << endl << endl;
	inFunction = false;
	*/
}

void JWasmGenerator::visit(CompoundStmt& n)
{
	for (auto& d : n.localDecls)
	{
		d->accept(*this);
	}
	for (auto& s : n.stmts)
	{
		s->accept(*this);
	}
}

void JWasmGenerator::visit(ReturnStmt& n)
{
	if (n.expr)
	{
		n.expr->accept(*this);
		ind(); out << "ret" << endl;
	}
	else
	{
		ind(); out << "ret" << endl;
	}
}

void JWasmGenerator::visit(ExprStmt& n)
{
	if (n.expr) n.expr->accept(*this);
}

void JWasmGenerator::visit(NumberExpr& n)
{
	// push immediate into register/stack
	ind();
	if (bits == 64) out << "mov " << regA() << ", " << n.value << "\n";
	else out << "mov " << regA() << ", " << n.value << "\n";
	// push onto stack to follow simple eval convention
	ind(); out << "push " << regA() << "\n";
}

void JWasmGenerator::visit(VarExpr& n)
{
	ind();
	auto it = paramIndex.find(n.name);
	if (it != paramIndex.end())
	{
		if (it->second.type == "int" && bits == 64) out << "movsxd " << regA() << ", " << n.name << " ; load " << n.name << "\n";
		if (it->second.type == "int" && bits == 32) out << "mov " << regA() << ", " << n.name << " ; load " << n.name << "\n";
		ind(); out << "push " << regA() << "\n";
	}
	else
	{
		out << "; global ref " << n.name << "\n";
	}
}

void JWasmGenerator::visit(BinaryExpr& n)
{
	// evaluate lhs and rhs, then emit op
	n.lhs->accept(*this); // pushes lhs
	n.rhs->accept(*this); // pushes rhs
	// pop rhs into regB, pop lhs into regA
	ind(); out << "pop " << regB() << "\n";
	ind(); out << "pop " << regA() << "\n";
	ind();
	if (n.op == '+') out << "add " << regA() << ", " << regB() << "\n";
	else if (n.op == '-') out << "sub " << regA() << ", " << regB() << "\n";
	else if (n.op == '*') out << "imul " << regA() << ", " << regB() << "\n";
	else if (n.op == '/')
	{
		ind(); out << "cdq\n"; // extend eax to edx:eax for idiv
		ind(); out << "idiv " << regB() << "\n";
	}
	// push result
	ind(); out << "push " << regA() << "\n";
}

void JWasmGenerator::visit(AssignExpr& n)
{
	// evaluate value then set_local
	n.value->accept(*this); // pushes value
	// pop into regA and store
	ind(); out << "pop " << regA() << "\n";
	ind();
	auto it = paramIndex.find(n.name);
	if (it != paramIndex.end())
	{
		out << "mov " << n.name << ", " << regA() << " ; store " << n.name << "\n";
	}
	else
	{
		out << "; store to global " << n.name << "\n";
	}
}

void JWasmGenerator::visit(CallExpr& n)
{
	// push arguments in reverse (cdecl-like)
	for (auto it = n.args.rbegin(); it != n.args.rend(); ++it)
	{
		(*it)->accept(*this); // pushes arg
	}
	// emit call
	ind(); out << "call _" << n.callee << "\n";
	// after call, caller cleans up arguments: add esp, argsbytes
	if (!n.args.empty())
	{
		int argsBytes = (int)n.args.size() * wordBytes();
		ind(); out << "add esp, " << argsBytes << "\n";
	}
	// push return value (assumed in regA)
	ind(); out << "push " << regA() << "\n";
}
