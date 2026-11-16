#include "pch.h"
#include "FunctionDefinition.h"
#include  "BaseStatement.h"

using namespace WadeSpace;
using namespace std;

FunctionDefinition::FunctionDefinition(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<Declarator> declarator, shared_ptr<vector<shared_ptr<Declaration>>> vectorDeclaration, shared_ptr<BaseStatement> baseStatement) 
    : declarationSpecifiers(declarationSpecifiers), declarator(declarator), vectorDeclaration(vectorDeclaration), baseStatement(baseStatement)
{
}

FunctionDefinition::FunctionDefinition(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<Declarator> declarator, shared_ptr<BaseStatement> baseStatement)
	: declarationSpecifiers(declarationSpecifiers), declarator(declarator), baseStatement(baseStatement), vectorDeclaration(nullptr)
{
}

FunctionDefinition::FunctionDefinition(shared_ptr<Declarator> declarator, shared_ptr<vector<shared_ptr<Declaration>>> vectorDeclaration, shared_ptr<BaseStatement> baseStatement)
	: declarator(declarator), vectorDeclaration(vectorDeclaration), baseStatement(baseStatement), declarationSpecifiers(nullptr)
{
}

FunctionDefinition::FunctionDefinition(shared_ptr<Declarator> declarator, shared_ptr<BaseStatement> baseStatement)
	: declarator(declarator), baseStatement(baseStatement), vectorDeclaration(nullptr), declarationSpecifiers(nullptr)
{
}
