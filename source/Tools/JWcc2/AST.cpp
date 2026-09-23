#include "pch.h"
#include "AST.h"

using namespace std;

// accept implementations
void Program::accept(ASTVisitor& v) { v.visit(*this); }
void VariableDeclaration::accept(ASTVisitor& v) { v.visit(*this); }
void FunctionDeclaration::accept(ASTVisitor& v) { v.visit(*this); }
void CompoundStatement::accept(ASTVisitor& v) { v.visit(*this); }
void ReturnStatement::accept(ASTVisitor& v) { v.visit(*this); }
void ExpressionStatement::accept(ASTVisitor& v) { v.visit(*this); }
void NumberExpression::accept(ASTVisitor& v) { v.visit(*this); }
void VariableExpression::accept(ASTVisitor& v) { v.visit(*this); }
void BinaryExpression::accept(ASTVisitor& v) { v.visit(*this); }
void AssignExpression::accept(ASTVisitor& v) { v.visit(*this); }
void CallExpression::accept(ASTVisitor& v) { v.visit(*this); }

