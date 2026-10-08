#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class ASTVisitor;

class Node
{
public:
    virtual ~Node() = default;
    virtual void accept(ASTVisitor& v) = 0;
    struct SourceLoc { int startPos = 0; int endPos = 0; int startLine = 1; int startColumn = 1; int endLine = 1; int endColumn = 1; } loc;
};

class Expression : public Node {};
class Statement : public Node {};
class Declaration : public Node {};

class Program : public Node
{
public:
    vector<unique_ptr<Declaration>> declarations;
    void accept(ASTVisitor& v) override;
};

// Declarations
class VariableDeclaration : public Declaration
{
public:
    string type;
    string name;
    unique_ptr<Expression> init; // optional
    void accept(ASTVisitor& v) override;
};

// Statements
class CompoundStatement: public Statement
{
public:
    vector<unique_ptr<Declaration>> localDeclarations;
    vector<unique_ptr<Statement>> statements;
    void accept(ASTVisitor& v) override;
};

class FunctionDeclaration : public Declaration
{
public:
    string retType;
    string name;
    vector<pair<string, string>> params; // (type,name)
    unique_ptr<CompoundStatement> body;
    void accept(ASTVisitor& v) override;
};

class ReturnStatement: public Statement
{
public:
    unique_ptr<Expression> expr; // optional
    void accept(ASTVisitor& v) override;
};

class ExpressionStatement : public Statement
{
public:
    unique_ptr<Expression> expr; // optional
    void accept(ASTVisitor& v) override;
};

// Expressions
class NumberExpression : public Expression
{
public:
    int value;
    NumberExpression(int v) : value(v) {}
    void accept(ASTVisitor& v) override;
};

class VariableExpression : public Expression
{
public:
    string name;
    VariableExpression(string n) : name(move(n)) {}
    void accept(ASTVisitor& v) override;
};

class BinaryExpression : public Expression
{
public:
    char operator1;
    unique_ptr<Expression> leftHandSide, rightHandSide;
    BinaryExpression(char o, unique_ptr<Expression> l, unique_ptr<Expression> r): operator1(o), leftHandSide(move(l)), rightHandSide(move(r)) {}
    void accept(ASTVisitor& v) override;
};

class AssignExpression : public Expression
{
public:
    string name;
    unique_ptr<Expression> value;
    AssignExpression(string n, unique_ptr<Expression> v): name(move(n)), value(move(v)) {}
    void accept(ASTVisitor& v) override;
};

class CallExpression : public Expression
{
public:
    string callee;
    vector<unique_ptr<Expression>> args;
    CallExpression(string c) : callee(move(c)) {}
    void accept(ASTVisitor& v) override;
};

// Visitor interface
class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(Program& n) = 0;
    virtual void visit(VariableDeclaration& n) = 0;
    virtual void visit(FunctionDeclaration& n) = 0;
    virtual void visit(CompoundStatement& n) = 0;
    virtual void visit(ReturnStatement& n) = 0;
    virtual void visit(ExpressionStatement& n) = 0;
    virtual void visit(NumberExpression& n) = 0;
    virtual void visit(VariableExpression& n) = 0;
    virtual void visit(BinaryExpression& n) = 0;
    virtual void visit(AssignExpression& n) = 0;
    virtual void visit(CallExpression& n) = 0;
};

