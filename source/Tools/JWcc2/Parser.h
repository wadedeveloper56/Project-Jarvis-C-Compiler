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
    vector<Token> tokens;
    size_t pos = 0;
public:
    Parser(vector<Token> t);
    // Main entry point: Program -> Function*
    unique_ptr<ProgramNode> parse_program();
private:
    // Function -> "int" Identifier "(" ")" "{" Statement* "}"
    unique_ptr<FunctionNode> parse_function();
    // Statement -> "return" Expression ";"
    unique_ptr<StmtNode> parse_statement();
    // Expression -> IntegerLiteral
    unique_ptr<ExprNode> parse_expression();
    // --- Parser Utilities ---
    // Look at the current token without moving forward
    Token peek() const;
    // Assert the current token type matches expectations, then advance
    Token consume(TokenType type, const string& error_msg);
};


