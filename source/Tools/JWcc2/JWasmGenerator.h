#pragma once

#include "AST.h"
#include <ostream>
#include <unordered_map>
#include <string>

using namespace std;

struct VarData
{
	string type;
	int index;
};

struct JWasmGenerator : ASTVisitor {
	// bits: 16,32,64
	// isWindows: choose Windows x64 calling convention if true, otherwise SysV
	JWasmGenerator(ostream& os, int bits = 32, bool isWindows = true) : out(os), bits(bits), indent(0), isWindows(isWindows), preparingFunctionParms(false) {}

	void generate(Program& program) { program.accept(*this); }

	// Visitor overrides
	void visit(Program& program) override;
	void visit(VarDecl& n) override;
	void visit(FunctionDecl& n) override;
	void visit(CompoundStmt& n) override;
	void visit(ReturnStmt& n) override;
	void visit(ExprStmt& n) override;
	void visit(NumberExpr& n) override;
	void visit(VarExpr& n) override;
	void visit(BinaryExpr& n) override;
	void visit(AssignExpr& n) override;
	void visit(CallExpr& n) override;

private:
	ostream& out;
	int indent;
	int bits;
	bool isWindows;
	bool preparingFunctionParms;
	unordered_map<string,VarData> paramIndex; // param name -> index
	unordered_map<string,VarData> localIndex; // local name -> slot (0..)
	void ind() { for (int i = 1; i <= indent; ++i) out << "  "; }

	// per-function state
	//bool inFunction;
	int nextLocalIndex();
	string wasmType(const string& ty) const;
	//vector<string> currentParams;
	//vector<VarDecl*> currentLocals;
	string wordForBits() const;
	string regA(int size) const;
	string regC(int size) const;
	string regB() const;
	int wordBytes() const { return bits / 8; }
};
