#include "pch.h"
#include "AST.h"

using namespace std;

// accept implementations
void Program::accept(ASTVisitor& v) { v.visit(*this); }
void VarDecl::accept(ASTVisitor& v) { v.visit(*this); }
void FunctionDecl::accept(ASTVisitor& v) { v.visit(*this); }
void CompoundStatement::accept(ASTVisitor& v) { v.visit(*this); }
void ReturnStatement::accept(ASTVisitor& v) { v.visit(*this); }
void ExprStatement::accept(ASTVisitor& v) { v.visit(*this); }
void NumberExpr::accept(ASTVisitor& v) { v.visit(*this); }
void VarExpr::accept(ASTVisitor& v) { v.visit(*this); }
void BinaryExpr::accept(ASTVisitor& v) { v.visit(*this); }
void AssignExpr::accept(ASTVisitor& v) { v.visit(*this); }
void CallExpr::accept(ASTVisitor& v) { v.visit(*this); }

