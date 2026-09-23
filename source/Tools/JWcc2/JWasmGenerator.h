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

struct JWasmGenerator : ASTVisitor 
{

private:
	ostream& out;
	int indent;
	int bits;
	bool isWindows;
	unordered_map<string,VarData> paramIndex;
	unordered_map<string,VarData> localIndex;
public:
	JWasmGenerator(ostream& os, int bits = 32, bool isWindows = true) : out(os), bits(bits), indent(0), isWindows(isWindows) {}
	void generate(Program& program) { program.accept(*this); }
	void visit(Program& program) override;
	void visit(VariableDeclaration& n) override;
	void visit(FunctionDeclaration& function) override;
	void visit(CompoundStatement& n) override;
	void visit(ReturnStatement& n) override;
	void visit(ExpressionStatement& n) override;
	void visit(NumberExpression& n) override;
	void visit(VariableExpression& n) override;
	void visit(BinaryExpression& n) override;
	void visit(AssignExpression& n) override;
	void visit(CallExpression& n) override;
private:
	void ind() { for (int i = 1; i <= indent; ++i) out << "  "; }
	void outputFunctionComment(FunctionDeclaration& function);
	void outputFunctionLocals(FunctionDeclaration& function);
	void moveResultOfInvokeIntoRegisterA(VariableDeclaration* v);
	void outputFunctionLocalsInitialization(FunctionDeclaration& function);
	void outputFunctionHeader(FunctionDeclaration& function);
	void outputFunctionBody(FunctionDeclaration& function);
	int nextLocalIndex();
	string wasmType(const string& ty) const;
	string wordForBits() const;
	string regA(int size) const;
	string regC(int size) const;
	string regB(int size) const;
	int wordBytes() const { return bits / 8; }
};
