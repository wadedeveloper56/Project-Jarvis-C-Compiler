#include "pch.h"
#include "DirectDeclarator.h"
#include "ParameterTypeList.h"
#include "Declarator.h"
#include "ExpressionTree.h"

using namespace WadeSpace;
using namespace std;

DirectDeclarator::DirectDeclarator(
	shared_ptr<CToken> identifier,
	shared_ptr<CToken> token1,
	shared_ptr<CToken> token2,
	shared_ptr<Declarator> declarator,
	shared_ptr<DirectDeclarator> directDeclarator,
	shared_ptr<ExpressionTree> constantExpression,
	shared_ptr<ParameterTypeList> parameterTypeList,
	shared_ptr<vector<shared_ptr<CToken>>> vectorOfStrings)
	: identifier(identifier),
	  token1(token1),
	  token2(token2),
	  declarator(declarator),
	  directDeclarator(directDeclarator),
	  constantExpression(constantExpression),
	  parameterTypeList(parameterTypeList),
	  vectorOfStrings(vectorOfStrings)
{
}

