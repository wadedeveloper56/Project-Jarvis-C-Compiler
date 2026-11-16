#include "pch.h"
#include "Declaration.h"
#include "ExternalDeclaration.h"

using namespace WadeSpace;
using namespace std;

Declaration::Declaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers) : declarationSpecifiers(declarationSpecifiers), vectorInitDeclarator(nullptr)
{
}

Declaration::Declaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<vector<shared_ptr<InitDeclarator>>> vectorInitDeclarator) :
declarationSpecifiers(declarationSpecifiers), vectorInitDeclarator(vectorInitDeclarator)
{
}
