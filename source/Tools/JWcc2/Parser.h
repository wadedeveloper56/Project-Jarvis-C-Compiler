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
public:
    explicit Parser(vector<Token> tokens);
    unique_ptr<ProgramNode> parseProgram();
private:
    vector<Token> tokens_;
    size_t index_;
    Token peek();
    Token advance();
    void consume(TokenType type, const string& errorMsg);
    unique_ptr<ASTNode> parseExternalDeclaration();
    unique_ptr<FunctionDeclNode> parseFunctionDefinition(const string& returnType, const string& name);
};
