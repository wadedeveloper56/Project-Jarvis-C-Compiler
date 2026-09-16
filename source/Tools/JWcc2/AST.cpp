#include "pch.h"
#include "AST.h"

using namespace std;

void IntegerLiteral::accept(ASTVisitor* visitor) { visitor->visit(this); }
void IdentifierExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void BinaryExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void VarDeclNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ReturnStmtNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FunctionDeclNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ProgramNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
