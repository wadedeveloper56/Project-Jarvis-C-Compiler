#include "pch.h"
#include "ParameterDeclaration.h"

using namespace WadeSpace;

ParameterDeclaration::ParameterDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers) : declarationSpecifiers(declarationSpecifiers), abstractDeclarator(nullptr), declarator(nullptr)
{
}

ParameterDeclaration::ParameterDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<AbstractDeclarator> abstractDeclarator) : declarationSpecifiers(declarationSpecifiers), abstractDeclarator(abstractDeclarator), declarator(nullptr)
{
}

ParameterDeclaration::ParameterDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<Declarator> declarator) : declarationSpecifiers(declarationSpecifiers), abstractDeclarator(nullptr), declarator(declarator)
{
}
