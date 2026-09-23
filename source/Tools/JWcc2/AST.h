#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

struct ASTVisitor;

struct Node
{
    virtual ~Node() = default;
    virtual void accept(ASTVisitor& v) = 0;
    struct SourceLoc { int startPos = 0; int endPos = 0; int startLine = 1; int startColumn = 1; int endLine = 1; int endColumn = 1; } loc;
};

using Ptr = unique_ptr<Node>;

struct Expression : Node {};
struct Statement : Node {};
struct Declaration : Node {};

struct Program : Node
{
    vector<unique_ptr<Declaration>> declarations;
    void accept(ASTVisitor& v) override;
};

// Declarations
struct VarDecl : Declaration
{
    string type;
    string name;
    unique_ptr<Expression> init; // optional
    void accept(ASTVisitor& v) override;
};

struct FunctionDecl : Declaration
{
    string retType;
    string name;
    vector<pair<string, string>> params; // (type,name)
    unique_ptr<Statement> body;
    void accept(ASTVisitor& v) override;
};

// Statements
struct CompoundStatement: Statement
{
    vector<unique_ptr<Declaration>> localDeclarations;
    vector<unique_ptr<Statement>> stmts;
    void accept(ASTVisitor& v) override;
};

struct ReturnStatement: Statement
{
    unique_ptr<Expression> expr; // optional
    void accept(ASTVisitor& v) override;
};

struct ExpressionStatement : Statement
{
    unique_ptr<Expression> expr; // optional
    void accept(ASTVisitor& v) override;
};

// Expressions
struct NumberExpression : Expression
{
    int value;
    NumberExpression(int v) : value(v) {}
    void accept(ASTVisitor& v) override;
};

struct VarExpression : Expression
{
    string name;
    VarExpression(string n) : name(move(n)) {}
    void accept(ASTVisitor& v) override;
};

struct BinaryExpression : Expression
{
    char op;
    unique_ptr<Expression> lhs, rhs;
    BinaryExpression(char o, unique_ptr<Expression> l, unique_ptr<Expression> r): op(o), lhs(move(l)), rhs(move(r)) {}
    void accept(ASTVisitor& v) override;
};

struct AssignExpression : Expression
{
    string name;
    unique_ptr<Expression> value;
    AssignExpression(string n, unique_ptr<Expression> v): name(move(n)), value(move(v)) {}
    void accept(ASTVisitor& v) override;
};

struct CallExpression : Expression
{
    string callee;
    vector<unique_ptr<Expression>> args;
    CallExpression(string c) : callee(move(c)) {}
    void accept(ASTVisitor& v) override;
};

// Visitor interface
struct ASTVisitor
{
    virtual ~ASTVisitor() = default;
    virtual void visit(Program& n) = 0;
    virtual void visit(VarDecl& n) = 0;
    virtual void visit(FunctionDecl& n) = 0;
    virtual void visit(CompoundStatement& n) = 0;
    virtual void visit(ReturnStatement& n) = 0;
    virtual void visit(ExpressionStatement& n) = 0;
    virtual void visit(NumberExpression& n) = 0;
    virtual void visit(VarExpression& n) = 0;
    virtual void visit(BinaryExpression& n) = 0;
    virtual void visit(AssignExpression& n) = 0;
    virtual void visit(CallExpression& n) = 0;
};

