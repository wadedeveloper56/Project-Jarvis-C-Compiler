#pragma once

#include <ostream>
#include <unordered_map>
#include <string>
#include "AST.h"
#include "JWasmTree.h"

using namespace std;

struct VarData
{
	string type;
	int index;
};

class JWasmGenerator : ASTVisitor
{
	//ostream& out;
	int indent;
	int bits;
	bool isWindows;
	unordered_map<string, VarData> paramIndex;
	unordered_map<string, VarData> localIndex;
	JWasmTree tree;
	AsmMethod *method;
public:
	JWasmGenerator(ostream& os, int bits = 32, bool isWindows = true);
	void generate(Program& program);
	void outputMethodHeader(AsmMethod& method, std::ostream& out);
	void outputMethodLocals(AsmMethod& method, std::ostream& out);
	void outputMethodInstructionsMacro(std::ostream& out, AsmInstruction& instr);
	void outputMethodInstructionsOperand(std::string mnemonic, AsmOperand& op, std::ostream& out, bool comma = true);
	void outputMethodInstructions(AsmMethod& method, std::ostream& out);
	void output();

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
	//void ind();

	void asmStatementIdivRegisterAAndRegisterB(AsmMethod* method);
	void outputReturnBinaryExpression(AsmMethod *method, BinaryExpression* be);
	void asmStatementPushRegisterA(AsmMethod* method, string comment);
	void asmStatementPopRegisterA(AsmMethod* method, string comment);
	void asmStatementPushRegisterB(AsmMethod* method, string comment);
	void asmStatementPopRegisterB(AsmMethod* method, string comment);
	void asmStatementMoveVariableToRegisterA(AsmMethod* method, string type, string name, string comment);
	void asmStatementMoveSignExtendVariableToRegisterA(AsmMethod* method, string type, string name, string comment);
	void asmStatementMoveRegisterAToVariable(AsmMethod* method, string type, string name, string comment);
	void moveResultOfInvokeIntoRegisterA(AsmMethod* method, VariableDeclaration* v);
	void asmStatementMoveImmediateToRegisterA(AsmMethod* method, string type, string name, string comment);
	void asmStatementAddRegisterAAndRegisterB(AsmMethod* method, string comment);
	void asmStatementSubRegisterAAndRegisterB(AsmMethod* method, string comment);
	void asmStatementImulRegisterAAndRegisterB(AsmMethod* method, string comment);

	void outputFunctionComment(FunctionDeclaration& function);
	void outputFunctionLocals(AsmMethod* method, FunctionDeclaration& function);
	void outputFunctionLocalsInitializationFunctionCall(AsmMethod* method, CallExpression* exp);
	void outputFunctionLocalsInitialization(AsmMethod* method, FunctionDeclaration& function);
	void asmOutputParameterVariables(AsmMethod* method, string type, string name);
	void asmOutputLocalVariables(AsmMethod* method, string type, string name);
	void outputFunctionHeader(AsmMethod* method, FunctionDeclaration& function);
	void outputFunctionBody(AsmMethod* method, FunctionDeclaration& function);
	void outputProgramFileHeader();
	void asmGlobalData(string name, string type, string value);
	void outputProgramUninitializedData(Program& program);
	void outputProgramInitializedData(Program& program);
	void outputProgramCode(Program& program);

	string regA(int size) const;
	string regB(int size) const;
	string regC(int size) const;
	string regD(int size) const;
};
