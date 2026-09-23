#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "Lexer.h"
#include "AST.h"

using namespace std;

class Parser
{
	Lexer lexer;
	Token cur;
	istream& in;
	void next();
	bool accept(TokenKind k);
	void expect(TokenKind k, const char* msg = "");
public:
	Parser(string s, istream& in);
	unique_ptr<Program> parseProgram();
	unique_ptr<Declaration> parseDecl();
	unique_ptr<CompoundStatement> parseCompoundStmt();
	unique_ptr<Statement> parseStmt();
	unique_ptr<Expression> parseExpression();
	unique_ptr<Expression> parseAssignment();
	unique_ptr<Expression> parseAddSub();
	unique_ptr<Expression> parseMulDiv();
	unique_ptr<Expression> parseUnary();
	unique_ptr<Expression> parsePrimary();
	string parseType();
};

