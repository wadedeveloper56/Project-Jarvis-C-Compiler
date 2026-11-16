#pragma once

#include "BaseStatement.h"
#include "Declaration.h"
#include "Constant.h"
#include "Token.h"
#include "ExpressionTree.h"
#include "Pointer.h"
#include "DeclarationSpecifiers.h"
#include "Declarator.h"
#include "ExternalDeclaration.h"
#include "ParameterDeclaration.h"
#include "StructDeclaration.h"

using namespace std;
using namespace WadeSpace;

void handleDeclaration(shared_ptr<ExternalDeclaration> externalDeclaration);
void createTranslationUnit(shared_ptr<ExternalDeclaration> externalDeclaration);
shared_ptr<vector<shared_ptr<BaseStatement>>> createStatementList(shared_ptr<BaseStatement> statement, shared_ptr<vector<shared_ptr<BaseStatement>>> list);
shared_ptr<vector<shared_ptr<Initializer>>> createInitializerList(shared_ptr<Initializer> initializer, shared_ptr<vector<shared_ptr<Initializer>>> list);
shared_ptr<DirectAbstractDeclarator> createDirectAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator, shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list);
shared_ptr<DirectAbstractDeclarator> createDirectAbstractDeclarator(shared_ptr<DirectAbstractDeclarator> dad, shared_ptr<AbstractDeclarator> abstractDeclarator, shared_ptr<ParameterTypeList> parameterTypeList, shared_ptr<ExpressionTree> constantExpression, WadeSpace::TokenType type);
shared_ptr<vector<shared_ptr<CToken>>> createIdentifierList(const shared_ptr<CToken> identifier, shared_ptr<vector<shared_ptr<CToken>>> list);
shared_ptr<vector<shared_ptr<ParameterDeclaration>>> createParameterList(shared_ptr<ParameterDeclaration> value1, shared_ptr<vector<shared_ptr<ParameterDeclaration>>> list);
shared_ptr<vector<shared_ptr<TypeQualifier>>> createTypeQualifierList(shared_ptr<TypeQualifier> value1, shared_ptr<vector<shared_ptr<TypeQualifier>>> list);
shared_ptr<vector<shared_ptr<Enumerator>>> createEnumeratorList(shared_ptr<Enumerator> value1, shared_ptr<vector<shared_ptr<Enumerator>>> list);
shared_ptr<vector<shared_ptr<StructDeclarator>>> createStructDeclaratorList(shared_ptr<StructDeclarator> value1, shared_ptr<vector<shared_ptr<StructDeclarator>>> list);
shared_ptr<vector<shared_ptr<StructDeclaration>>> createStructDeclarationList(shared_ptr<StructDeclaration> value1, shared_ptr<vector<shared_ptr<StructDeclaration>>> list);
shared_ptr<InitDeclarator> createInitDeclarator(shared_ptr<Declarator> declarator, shared_ptr<Initializer> initializer);
shared_ptr<Declaration> createDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<vector<shared_ptr<InitDeclarator>>> vectorInitDeclarator);
shared_ptr<vector<shared_ptr<InitDeclarator>>> createInitDeclaratorList(shared_ptr<InitDeclarator> value1, shared_ptr<vector<shared_ptr<InitDeclarator>>> list);
shared_ptr<vector<shared_ptr<ExpressionTree>>> createArgumentExpressionList(shared_ptr<ExpressionTree> exp, shared_ptr<vector<shared_ptr<ExpressionTree>>> list);
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
);
shared_ptr<Constant> createConstant(const shared_ptr<CToken> iConst, const shared_ptr<CToken> fConst, const shared_ptr<CToken> strConst, const std::optional<WadeSpace::TokenType>& type);
shared_ptr<Declarator> createDeclarator(shared_ptr<Pointer> pointer, shared_ptr<DirectDeclarator> directDeclarator);
shared_ptr<DirectDeclarator> createDirectDeclarator(shared_ptr<CToken> identifier, shared_ptr<CToken> token1, shared_ptr<CToken> token2,
	shared_ptr<Declarator> declarator,
	shared_ptr<DirectDeclarator> directDeclarator,
	shared_ptr<ExpressionTree> constantExpression,
	shared_ptr<ParameterTypeList> parameterTypeList,
	shared_ptr<vector<shared_ptr<CToken>>> vectorOfStrings);
shared_ptr<DeclarationSpecifiers> createDeclarationSpecifiers(
	shared_ptr<CToken> token,
	shared_ptr<DeclarationSpecifiers> declarationSpecifiers,
	shared_ptr<StorageClassSpecifier> storageClassSpecifier,
	shared_ptr<TypeSpecifier> typeSpecifier,
	shared_ptr<TypeQualifier> typeQualifier,
	shared_ptr<StructOrUnionSpecifier> structOrUnionSpecifier);
shared_ptr<StorageClassSpecifier> createStorageClassSpecifier(shared_ptr<CToken> token);
shared_ptr<ExpressionTree> createPrimaryExpression(const shared_ptr<CToken> identifier, shared_ptr<Constant> constant);
shared_ptr<vector<shared_ptr<Declaration>>> createDeclarationList(shared_ptr<Declaration> exp, shared_ptr<vector<shared_ptr<Declaration>>> list);
