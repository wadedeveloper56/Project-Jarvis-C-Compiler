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

struct ReturnStmt : Statement
{
    unique_ptr<Expression> expr; // optional
    void accept(ASTVisitor& v) override;
};

struct ExprStmt : Statement
{
    unique_ptr<Expression> expr; // optional
    void accept(ASTVisitor& v) override;
};

// Expressions
struct NumberExpr : Expression
{
    int value;
    NumberExpr(int v) : value(v) {}
    void accept(ASTVisitor& v) override;
};

struct VarExpr : Expression
{
    string name;
    VarExpr(string n) : name(move(n)) {}
    void accept(ASTVisitor& v) override;
};

struct BinaryExpr : Expression
{
    char op;
    unique_ptr<Expression> lhs, rhs;
    BinaryExpr(char o, unique_ptr<Expression> l, unique_ptr<Expression> r): op(o), lhs(move(l)), rhs(move(r)) {}
    void accept(ASTVisitor& v) override;
};

struct AssignExpr : Expression
{
    string name;
    unique_ptr<Expression> value;
    AssignExpr(string n, unique_ptr<Expression> v): name(move(n)), value(move(v)) {}
    void accept(ASTVisitor& v) override;
};

struct CallExpr : Expression
{
    string callee;
    vector<unique_ptr<Expression>> args;
    CallExpr(string c) : callee(move(c)) {}
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
    virtual void visit(ReturnStmt& n) = 0;
    virtual void visit(ExprStmt& n) = 0;
    virtual void visit(NumberExpr& n) = 0;
    virtual void visit(VarExpr& n) = 0;
    virtual void visit(BinaryExpr& n) = 0;
    virtual void visit(AssignExpr& n) = 0;
    virtual void visit(CallExpr& n) = 0;
};

