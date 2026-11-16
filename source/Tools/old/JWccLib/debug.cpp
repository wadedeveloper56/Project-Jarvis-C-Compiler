#include "pch.h"
#include "debug.h"
#include "ExpressionTree.h"
#include "ProgramData.h"
#include "GlobalVars.h"
#include "StructOrUnionSpecifier.h"
#include "Compiler.h"

using namespace std;
using namespace WadeSpace;

/*
void handleStructDefinition(Declaration* declaration)
{
	auto declaration_specifiers = declaration->getDeclarationSpecifiers();
	if (declaration_specifiers != nullptr && declaration_specifiers->hasDeclarationSpecifiersNodeList())
	{
		for (auto node : *declaration_specifiers->getDeclarationSpecifiersNodeList())
		{
			auto type_specifier = node->typeSpecifier;
			if (type_specifier != nullptr && type_specifier->getType().has_value())
			{
				auto token = type_specifier->getType().value();
				if ((token == STRUCT || token == UNION) && type_specifier->getStructOrUnionSpecifier() != nullptr)
				{
					StructOrUnionSpecifier* temp = type_specifier->getStructOrUnionSpecifier();
					if (temp->getVectorStructDeclaration() != nullptr)
					{
						string name = temp->getName()->getSymbolName();
						map<string, StructOrUnionSpecifier*>* smap = compiler->getStructList();
						smap->insert({ name, temp });
					}
				}
			}
		}
	}
}
*/

void handleDeclaration(shared_ptr<ExternalDeclaration> externalDeclaration)
{
	bool isTypedef = externalDeclaration->isTypedef();
	shared_ptr<Declaration> declaration = externalDeclaration->getDeclaration();
	if (declaration != nullptr)
	{
		//handleStructDefinition(declaration);
		if (declaration->getVectorInitDeclarator() != nullptr)
		{
			for (shared_ptr<InitDeclarator> initDecl : *declaration->getVectorInitDeclarator())
			{
				shared_ptr<DirectDeclarator> dd = initDecl->getDeclarator()->getDirectDeclarator();
				if (dd != nullptr)
				{
					string name;
					if (dd->getIdentifier() != nullptr)
					{
						name = dd->getIdentifier()->getSymbolName();
					}
					else
					{
						name = dd->getDirectDeclarator()->getIdentifier()->getSymbolName();
					}
					/*					if (isTypedef)
										{
											compiler->getTypedefList()->insert({name, externalDeclaration});
										}
					*/
				}
			}
		}
	}
	//if (!isTypedef) compiler->addExternalDeclaration(externalDeclaration);
}

void createTranslationUnit(shared_ptr<ExternalDeclaration> externalDeclaration)
{
	compiler->addExternalDeclaration(externalDeclaration);
}

shared_ptr<vector<shared_ptr<BaseStatement>>> createStatementList(shared_ptr<BaseStatement> statement, shared_ptr<vector<shared_ptr<BaseStatement>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<BaseStatement>>>();
	}
	list->push_back(statement);
	return list;
}

shared_ptr<vector<shared_ptr<Initializer>>> createInitializerList(shared_ptr<Initializer> initializer, shared_ptr<vector<shared_ptr<Initializer>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<Initializer>>>();
	}
	list->push_back(initializer);
	return list;
}

shared_ptr<DirectAbstractDeclarator> createDirectAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator, shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list)
{
	return make_shared<DirectAbstractDeclarator>(abstractDeclarator, list);
}

shared_ptr<DirectAbstractDeclarator> createDirectAbstractDeclarator(shared_ptr<DirectAbstractDeclarator> dad, shared_ptr<AbstractDeclarator> abstractDeclarator, shared_ptr<ParameterTypeList> parameterTypeList, shared_ptr<ExpressionTree> constantExpression, TokenType type)
{
	shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list;
	if (dad != nullptr)
	{
		list = dad->getList();
	}
	else
	{
		list = make_shared<vector<shared_ptr<DirectAbstractDeclaratorNode>>>();
	}
	shared_ptr<DirectAbstractDeclaratorNode> node = make_shared<DirectAbstractDeclaratorNode>(parameterTypeList, constantExpression, type);
	list->push_back(node);
	return make_shared<DirectAbstractDeclarator>(dad->getAbstractDeclarator(), list);
}

shared_ptr<vector<shared_ptr<CToken>>> createIdentifierList(const shared_ptr<CToken> identifier, shared_ptr<vector<shared_ptr<CToken>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<CToken>>>();
	}
	list->push_back(identifier);
	return list;
}

shared_ptr<vector<shared_ptr<ParameterDeclaration>>> createParameterList(shared_ptr<ParameterDeclaration> value1, shared_ptr<vector<shared_ptr<ParameterDeclaration>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<ParameterDeclaration>>>();
	}
	list->push_back(value1);
	return list;
}

shared_ptr<vector<shared_ptr<TypeQualifier>>> createTypeQualifierList(shared_ptr<TypeQualifier> value1, shared_ptr<vector<shared_ptr<TypeQualifier>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<TypeQualifier>>>();
	}
	list->push_back(value1);
	return list;
}

shared_ptr<vector<shared_ptr<Enumerator>>> createEnumeratorList(shared_ptr<Enumerator> value1, shared_ptr<vector<shared_ptr<Enumerator>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<Enumerator>>>();
	}
	list->push_back(value1);
	return list;
}

shared_ptr<vector<shared_ptr<StructDeclarator>>> createStructDeclaratorList(shared_ptr<StructDeclarator> value1, shared_ptr<vector<shared_ptr<StructDeclarator>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<StructDeclarator>>>();
	}
	list->push_back(value1);
	return list;
}

shared_ptr<vector<shared_ptr<StructDeclaration>>> createStructDeclarationList(shared_ptr<StructDeclaration> value1, shared_ptr<vector<shared_ptr<StructDeclaration>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<StructDeclaration>>>();
	}
	list->push_back(value1);
	return list;
}

shared_ptr<InitDeclarator> createInitDeclarator(shared_ptr<Declarator> declarator, shared_ptr<Initializer> initializer)
{
	return make_shared<InitDeclarator>(declarator, initializer);
}

shared_ptr<Declaration> createDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<vector<shared_ptr<InitDeclarator>>> vectorInitDeclarator)
{
	return make_shared<Declaration>(declarationSpecifiers, vectorInitDeclarator);
}

shared_ptr<vector<shared_ptr<InitDeclarator>>> createInitDeclaratorList(shared_ptr<InitDeclarator> value1, shared_ptr<vector<shared_ptr<InitDeclarator>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<InitDeclarator>>>();
	}
	list->push_back(value1);
	return list;
}

shared_ptr<vector<shared_ptr<ExpressionTree>>> createArgumentExpressionList(shared_ptr<ExpressionTree> exp, shared_ptr<vector<shared_ptr<ExpressionTree>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<ExpressionTree>>>();
	}
	list->push_back(exp);
	return list;
}

shared_ptr<ExpressionTree> createExpression(
	NodeType type,
	shared_ptr<CToken> token1,
	shared_ptr<CToken> token2,
	shared_ptr<ExpressionTree> lexp,
	shared_ptr<ExpressionTree> exp1,
	shared_ptr<ExpressionTree> exp2,
	shared_ptr<vector<shared_ptr<ExpressionTree>>> argumentList,
	shared_ptr<CToken> identifier,
	shared_ptr<vector<shared_ptr<Initializer>>> initializerList,
	shared_ptr<TypeName> typeName,
	shared_ptr<CToken> token3,
	shared_ptr<Constant> constant,
	shared_ptr<ExpressionTree> left,
	shared_ptr<CToken> op,
	shared_ptr<ExpressionTree> right
)
{
	shared_ptr<TreeNodeData> nodeData = make_shared<TreeNodeData>(type, token1, token2, lexp, exp1, exp2, argumentList, identifier, initializerList, typeName, token3, constant, op);
	shared_ptr<TreeNode> node = make_shared<TreeNode>(nodeData, left ? left->getTree() : nullptr, right ? right->getTree() : nullptr);
	return make_shared<ExpressionTree>(node);
}

shared_ptr<Constant> createConstant(const shared_ptr<CToken> iConst, const shared_ptr<CToken> fConst, const shared_ptr<CToken> strConst, const optional<TokenType>& type)
{
	return make_shared<Constant>(iConst, fConst, strConst, type);
}

shared_ptr<Declarator> createDeclarator(shared_ptr<Pointer> pointer, shared_ptr<DirectDeclarator> directDeclarator)
{
	return make_shared<Declarator>(pointer, directDeclarator);
}

shared_ptr<DirectDeclarator> createDirectDeclarator(shared_ptr<CToken> identifier, shared_ptr<CToken> token1, shared_ptr<CToken> token2,
	shared_ptr<Declarator> declarator,
	shared_ptr<DirectDeclarator> directDeclarator,
	shared_ptr<ExpressionTree> constantExpression,
	shared_ptr<ParameterTypeList> parameterTypeList,
	shared_ptr<vector<shared_ptr<CToken>>> vectorOfStrings)
{
	return make_shared<DirectDeclarator>(identifier, token1, token2, declarator, directDeclarator, constantExpression, parameterTypeList, vectorOfStrings);
}

shared_ptr<DeclarationSpecifiers> createDeclarationSpecifiers(shared_ptr<CToken> token,
	shared_ptr<DeclarationSpecifiers> declarationSpecifiers, 
	shared_ptr<StorageClassSpecifier> storageClassSpecifier, 
	shared_ptr<TypeSpecifier> typeSpecifier, 
	shared_ptr<TypeQualifier> typeQualifier,
	shared_ptr<StructOrUnionSpecifier> structOrUnionSpecifier)
{
	auto node = make_shared<DeclarationSpecifiersNode>(storageClassSpecifier, typeSpecifier, typeQualifier);
	if (declarationSpecifiers != nullptr)
	{
		declarationSpecifiers->addDeclarationSpecifiersNode(storageClassSpecifier, typeSpecifier, typeQualifier);
		return declarationSpecifiers;
	}
	else
	{
		shared_ptr<DeclarationSpecifiers> declarationSpecifiers = make_shared<DeclarationSpecifiers>();
		declarationSpecifiers->addDeclarationSpecifiersNode(storageClassSpecifier, typeSpecifier, typeQualifier);
		return declarationSpecifiers;
	}
}

shared_ptr<StorageClassSpecifier> createStorageClassSpecifier(shared_ptr<CToken> token)
{
	return make_shared<StorageClassSpecifier>(token);
}

shared_ptr<ExpressionTree> createPrimaryExpression(const shared_ptr<CToken> identifier, shared_ptr<Constant> constant)
{
	return createExpression(NT_NONE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, identifier, constant, nullptr, nullptr, nullptr);
}

shared_ptr<vector<shared_ptr<Declaration>>> createDeclarationList(shared_ptr<Declaration> exp, shared_ptr<vector<shared_ptr<Declaration>>> list)
{
	if (list == nullptr)
	{
		list = make_shared<vector<shared_ptr<Declaration>>>();
	}
	list->push_back(exp);
	return list;
}
