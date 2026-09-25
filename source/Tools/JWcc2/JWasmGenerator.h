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

class JWasmGenerator : ASTVisitor 
{
private:
	ostream& out;
	int indent;
	int bits;
	bool isWindows;
	unordered_map<string,VarData> paramIndex;
	unordered_map<string,VarData> localIndex;
public:
	JWasmGenerator(ostream& os, int bits = 32, bool isWindows = true);
	void generate(Program& program) { program.accept(*this); }
	void visit(Program& program) override;
	void visit(VariableDeclaration& n) override;
	void visit(FunctionDeclaration& function) override;
	void visit(CompoundStatement& n) override;
	void asmStatementAddRegisterAAndRegisterB();
	void asmStatementIdivRegisterAAndRegisterB();
	void outputReturnBinaryExpression(BinaryExpression* be);
	void visit(ReturnStatement& n) override;
	void visit(ExpressionStatement& n) override;
	void visit(NumberExpression& n) override;
	void visit(VariableExpression& n) override;
	void visit(BinaryExpression& n) override;
	void visit(AssignExpression& n) override;
	void visit(CallExpression& n) override;
private:
	void ind();

	void asmStatementPushRegisterA(string comment);
	void asmStatementPopRegisterA(string comment);
	void asmStatementPushRegisterB(string comment);
	void asmStatementPopRegisterB(string comment);
	void asmStatementMoveVariableToRegisterA(string type, string name, string comment);
	void asmStatementMoveSignExtendVariableToRegisterA(string type, string name, string comment);
	void asmStatementMoveRegisterAToVariable(string type, string name, string comment);
	void moveResultOfInvokeIntoRegisterA(VariableDeclaration* v);
	void asmStatementMoveImmediateToRegisterA(string type, string name, string comment);
	void asmStatementAddRegisterAAndRegisterB(string comment);
	void asmStatementSubRegisterAAndRegisterB(string comment);
	void asmStatementImulRegisterAAndRegisterB(string comment);

	void outputFunctionComment(FunctionDeclaration& function);
	void outputFunctionLocals(FunctionDeclaration& function);
	void outputFunctionLocalsInitializationFunctionCall(CallExpression* exp);
	void outputFunctionLocalsInitialization(FunctionDeclaration& function);
	void outputFunctionHeader(FunctionDeclaration& function);
	void outputFunctionBody(FunctionDeclaration& function);
	void outputProgramFileHeader();
	void outputProgramUninitializedData(Program& program);
	void outputProgramInitializedData(Program& program);
	void outputProgramCode(Program& program);

	string regA(int size) const;
	string regB(int size) const;
	string regC(int size) const;
	string regD(int size) const;
};
