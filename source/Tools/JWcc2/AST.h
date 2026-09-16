#pragma once

#include <memory>
#include <string>
#include <vector>

using namespace std;

class ASTVisitor;

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};
using ASTPtr = unique_ptr<ASTNode>;

struct Expr : ASTNode {}; 
using ExprPtr = unique_ptr<Expr>;

struct IntegerLiteral : Expr 
{ 
    int value; 
    IntegerLiteral(int v) : value(v) {} 
    void accept(ASTVisitor* visitor) override;
};

struct IdentifierExpr : Expr 
{ 
    string name; 
    IdentifierExpr(string n) : name(move(n)) {} 
    void accept(ASTVisitor* visitor) override;
};

struct BinaryExpr : Expr 
{ 
    string op; 
    ExprPtr lhs, rhs; 
    BinaryExpr(string op, ExprPtr l, ExprPtr r) : op(move(op)), lhs(move(l)), rhs(move(r)) {} 
    void accept(ASTVisitor* visitor) override; 
};

class VarDeclNode : public ASTNode
{
public:
    string type;
    string name;
    string initValue; // Simple literal initialization for this example
    bool isGlobal;

    VarDeclNode(string t, string n, string val, bool global) : type(move(t)), name(move(n)), initValue(move(val)), isGlobal(global) {}
    void accept(ASTVisitor* visitor) override;
};

class ReturnStmtNode : public ASTNode
{
public:
    string value;
    explicit ReturnStmtNode(string val) : value(move(val)) {}
    void accept(ASTVisitor* visitor) override;
};

class FunctionDeclNode : public ASTNode
{
public:
    string returnType;
    string name;
    vector<pair<string, string>> params; // pair<type, name>
    vector<unique_ptr<ASTNode>> body;

    FunctionDeclNode(string rType, string n) : returnType(move(rType)), name(move(n)) {}
    void accept(ASTVisitor* visitor) override;
};

class ProgramNode : public ASTNode
{
public:
    vector<unique_ptr<ASTNode>> externalDeclarations;
    void accept(ASTVisitor* visitor) override;
};

// Visitor Interface for AST traversal (e.g., Code Gen or Printing)
class ASTVisitor
{
public:
    virtual void visit(ProgramNode* node) = 0;
    virtual void visit(VarDeclNode* node) = 0;
    virtual void visit(FunctionDeclNode* node) = 0;
    virtual void visit(ReturnStmtNode* node) = 0;
	virtual void visit(IntegerLiteral* node) = 0;
	virtual void visit(IdentifierExpr* node) = 0;
	virtual void visit(BinaryExpr* node) = 0;
};

