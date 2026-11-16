#include "pch.h"
#include "ExternalDeclaration.h"
#include "GlobalVars.h"
#include "Compiler.h"

using namespace WadeSpace;
using namespace std;
 
ExternalDeclaration::ExternalDeclaration(shared_ptr<FunctionDefinition> functionDefinition) : functionDefinition(functionDefinition), declaration(nullptr)
{
}

ExternalDeclaration::ExternalDeclaration(shared_ptr<Declaration> declaration) : functionDefinition(nullptr), declaration(declaration)
{
	if (isTypedef())
	{
		shared_ptr<DeclarationSpecifiers> declSpecifiers = declaration->getDeclarationSpecifiers();
		shared_ptr<vector<shared_ptr<InitDeclarator>>> initDeclaratorsList = declaration->getVectorInitDeclarator();

		shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> list = declSpecifiers->getDeclarationSpecifiersNodeList();
		string name = initDeclaratorsList->at(0)->getDeclarator()->getDirectDeclarator()->getIdentifier()->getSymbolName();
		compiler->addTypedef(name, make_shared<DeclarationSpecifiersNode>(*list->at(0)));
	}
	else
	{
		TokenType type = findType(declaration)->getType().value();
		shared_ptr<DeclarationSpecifiers> declSpecifiers = declaration->getDeclarationSpecifiers();
		shared_ptr<vector<shared_ptr<InitDeclarator>>> initDeclaratorsList = declaration->getVectorInitDeclarator();
		shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> list = declSpecifiers->getDeclarationSpecifiersNodeList();
		switch (type)
		{
			case ENUM:
			{
				string enumName;
				shared_ptr<TypeSpecifier> type_specifier = nullptr;
				for (shared_ptr<DeclarationSpecifiersNode> ptr : *list)
				{
					type_specifier = ptr->getTypeSpecifier();
					if (type_specifier != nullptr)
					{
						enumName = type_specifier->getEnumSpec()->getNameStr()->getSymbolName();
						break;
					}
				}
				if (type_specifier != nullptr)
				{
					compiler->addEnum(enumName, type_specifier->getEnumSpec());
				}
				break;
			}
			case TYPE_NAME:
				break;
			case STRUCT:
			case UNION:
			{
				string structName;
				shared_ptr<TypeSpecifier> type_specifier = nullptr;
				for (shared_ptr<DeclarationSpecifiersNode> ptr : *list)
				{
					type_specifier = ptr->getTypeSpecifier();
					if (type_specifier != nullptr)
					{
						structName = type_specifier->getStructOrUnionSpecifier()->getName()->getSymbolName();
						break;
					}
				}
				if (type_specifier != nullptr)
				{
					compiler->addStruct(structName, type_specifier->getStructOrUnionSpecifier());
				}
				break;
			}
		}
	}
}

