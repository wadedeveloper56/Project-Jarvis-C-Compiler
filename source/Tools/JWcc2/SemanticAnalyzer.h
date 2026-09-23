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
	void visit(VariableDeclaration& n) override;
	void visit(FunctionDeclaration& n) override;
	void visit(CompoundStatement& n) override;
	void visit(ReturnStatement& n) override;
	void visit(ExpressionStatement& n) override;
	void visit(NumberExpression& n) override;
	void visit(VariableExpression& n) override;
	void visit(BinaryExpression& n) override;
	void visit(AssignExpression& n) override;
	void visit(CallExpression& n) override;

	// Control-flow checks
	void checkFunctionReturns(FunctionDeclaration& f);

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
