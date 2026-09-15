#pragma once

#include <memory>

using namespace std;

#include <memory>
#include <string>
#include <vector>

using namespace std;

// Base Class
class ASTNode
{
public:
	virtual ~ASTNode() = default;
};

// Base Expression Class
class ExprNode : public ASTNode {};

// Literal Expression (e.g., "2")
class LiteralNode : public ExprNode
{
public:
	string value;
	LiteralNode(string val) : value(val) {}
};

// Base Statement Class
class StmtNode : public ASTNode {};

// Return Statement (e.g., "return 2;")
class ReturnStmtNode : public StmtNode
{
public:
	unique_ptr<ExprNode> expr;
	ReturnStmtNode(unique_ptr<ExprNode> e) : expr(move(e)) {}
};

// Function Declaration (e.g., "int main() { ... }")
class FunctionNode : public ASTNode
{
public:
	string name;
	vector<unique_ptr<StmtNode>> body;
	FunctionNode(string n, vector<unique_ptr<StmtNode>> b)
		: name(n), body(move(b)) {}
};

// Root Program Node
class ProgramNode : public ASTNode
{
public:
	vector<unique_ptr<FunctionNode>> functions;
};

