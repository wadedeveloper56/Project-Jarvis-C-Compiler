#pragma once

#include "AST.h"
#include <ostream>
#include <unordered_map>
#include <string>

using namespace std;

struct JWasmGenerator : ASTVisitor {
	// bits: 16,32,64
	// isWindows: when bits==64 choose Windows x64 calling convention if true, otherwise SysV
	JWasmGenerator(ostream& os, int bits = 32, bool isWindows = true) : out(os), indent(0), bits(bits), inFunction(false), isWindows(isWindows) {}

	void generate(Program& p) { p.accept(*this); }

	// Visitor overrides
	void visit(Program& n) override;
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
	void ind() { for (int i = 0; i < indent; ++i) out << "  "; }

	// per-function state
	unordered_map<string,int> paramIndex; // param name -> index
	unordered_map<string,int> localIndex; // local name -> slot (0..)
	bool inFunction;
	int nextLocalIndex();
	string wasmType(const string& ty) const;
	vector<string> currentParams;
	vector<VarDecl*> currentLocals;
	string wordForBits() const;
	bool isWindows;
	string regA() const;
	string regB() const;
	int wordBytes() const { return bits / 8; }
	void emitPrologue(int localBytes);
	void emitEpilogue();
};
