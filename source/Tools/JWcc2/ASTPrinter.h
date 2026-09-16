#pragma once

#include "Lexer.h"
#include "Parser.h"

class ASTPrinter : public ASTVisitor
{
public:
	void visit(ProgramNode* node) override;
	void visit(VarDeclNode* node) override;
	void visit(FunctionDeclNode* node) override;
	void visit(ReturnStmtNode* node) override;
};