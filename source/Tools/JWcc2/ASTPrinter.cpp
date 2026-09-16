#include "pch.h"
#include "ASTPrinter.h"

void ASTPrinter::visit(IntegerLiteral* node) { cout << "[IntegerLiteral] " << node->value << "\n"; }

void ASTPrinter::visit(IdentifierExpr* node) { cout << "[IdentifierExpr] " << node->name << "\n"; }

void ASTPrinter::visit(BinaryExpr* node)
{
	cout << "[BinaryExpr] " << node->op << "\n";
	cout << "  LHS: ";
	node->lhs->accept(this);
	cout << "  RHS: ";
	node->rhs->accept(this);
}

void ASTPrinter::visit(ProgramNode* node)
{
	cout << "--- AST PROGRAM START ---\n";
	for (auto& decl : node->externalDeclarations)
	{
		decl->accept(this);
	}
	cout << "--- AST PROGRAM END ---\n";
}

void ASTPrinter::visit(VarDeclNode* node)
{
	cout << (node->isGlobal ? "[Global Var] " : "[Local Var] ")	<< node->type << " " << node->name;
	if (!node->initValue.empty())
	{
		cout << " = " << node->initValue;
	}
	cout << ";\n";
}

void ASTPrinter::visit(FunctionDeclNode* node)
{
	cout << "[Function] " << node->returnType << " " << node->name << "(";
	for (size_t i = 0; i < node->params.size(); ++i)
	{
		cout << node->params[i].first << " " << node->params[i].second;
		if (i + 1 < node->params.size()) cout << ", ";
	}
	cout << ") {\n";
	for (auto& stmt : node->body)
	{
		cout << "  ";
		stmt->accept(this);
	}
	cout << "}\n";
}

void ASTPrinter::visit(ReturnStmtNode* node) { cout << "return " << node->value << ";\n"; }
