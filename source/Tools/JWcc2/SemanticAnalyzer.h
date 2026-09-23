#pragma once

#include "AST.h"
#include "SymbolTable.h"
#include <string>
#include <vector>

using namespace std;

struct SemanticAnalyzer : ASTVisitor {
	SemanticAnalyzer();

	// Analyze program; returns true if no semantic errors found
	bool analyze(Program& prog);

	// Retrieve formatted error messages
	vector<string> errors() const { return errs; }

	// Visitor overrides
	void visit(Program& n) override;
	void visit(VarDecl& n) override;
	void visit(FunctionDecl& n) override;
	void visit(CompoundStatement& n) override;
	void visit(ReturnStatement& n) override;
	void visit(ExprStatement& n) override;
	void visit(NumberExpr& n) override;
	void visit(VarExpr& n) override;
	void visit(BinaryExpr& n) override;
	void visit(AssignExpr& n) override;
	void visit(CallExpr& n) override;

	// Control-flow checks
	void checkFunctionReturns(FunctionDecl& f);

	// Evaluate expression type or empty string on error
	string evalExprType(Expression* e);

	// Report an error (with optional source location)
	void error(const string& msg);
	void error(const string& msg, const Node::SourceLoc& loc);

private:
	SymbolTable symbols;
	vector<string> errs;
	string currentFunctionRetType;
};
