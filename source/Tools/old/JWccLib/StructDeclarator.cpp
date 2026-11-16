#include "pch.h"
#include "StructDeclarator.h"
#include "ExpressionTree.h"

using namespace WadeSpace;

StructDeclarator::StructDeclarator(shared_ptr<Declarator> declarator) : declarator(declarator), constantExpression(nullptr)
{
}

StructDeclarator::StructDeclarator(shared_ptr<ExpressionTree> constantExpression) : declarator(nullptr), constantExpression(constantExpression)
{
}

StructDeclarator::StructDeclarator(shared_ptr<Declarator> declarator, shared_ptr<ExpressionTree> constantExpression) : declarator(declarator), constantExpression(constantExpression)
{
}
