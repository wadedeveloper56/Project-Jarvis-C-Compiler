#include "pch.h"
#include "AST.h"

using namespace std;

void VarDeclNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ReturnStmtNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FunctionDeclNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ProgramNode::accept(ASTVisitor* visitor) { visitor->visit(this); }
