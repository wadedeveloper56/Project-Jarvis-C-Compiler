#include "pch.h"
#include "InitDeclarator.h"

using namespace WadeSpace;

InitDeclarator::InitDeclarator(shared_ptr<Declarator> declarator) : declarator(declarator), initializer(nullptr)
{
}

InitDeclarator::InitDeclarator(shared_ptr<Declarator> declarator, shared_ptr<Initializer> initializer) : declarator(declarator), initializer(initializer)
{
}

string InitDeclarator::getVariableName() const
{
	shared_ptr<CToken> identifier = getDeclarator()->getDirectDeclarator()->getIdentifier();
	if (!identifier) identifier = getDeclarator()->getDirectDeclarator()->getDirectDeclarator()->getIdentifier();
	return (identifier) ? identifier->getSymbolName() : "";
}
