#include "pch.h"
#include "DirectAbstractDeclarator.h"
#include "AbstractDeclarator.h"
#include "ParameterTypeList.h"
#include "ExpressionTree.h"

using namespace WadeSpace;

DirectAbstractDeclaratorNode::DirectAbstractDeclaratorNode(shared_ptr<ParameterTypeList> parameterTypeList, shared_ptr<ExpressionTree> constantExpression, optional<TokenType> type)
	: parameterTypeList(parameterTypeList),
	  constantExpression(constantExpression),
	  type(type)
{
}

DirectAbstractDeclarator::DirectAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator, shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list) :
	abstractDeclarator(abstractDeclarator), list(list)
{
}

