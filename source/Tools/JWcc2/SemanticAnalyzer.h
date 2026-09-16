#pragma once

#include "pch.h"
#include "AST.h"
#include "SymbolTable.h"
#include <vector>
#include <string>

using namespace std;

class SemanticAnalyzer : public ASTVisitor
{
public:
	SemanticAnalyzer();
	bool analyze(Program& prog);
	const vector<string>& errors() const { return errs; }

	// visitor methods
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
	SymbolTable symbols;
	vector<string> errs;
	string currentFunctionRetType;

	// expression type evaluation
	string evalExprType(Expr* e);
	void error(const string& msg) { errs.push_back(msg); }
};
