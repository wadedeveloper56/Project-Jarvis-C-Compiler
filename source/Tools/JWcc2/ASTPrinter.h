#pragma once

#include "Lexer.h"
#include "Parser.h"

struct ASTPrinter : ASTVisitor
{
    int indent = 0;
    void ind();
    //void printLoc(const Node& n);
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
};
