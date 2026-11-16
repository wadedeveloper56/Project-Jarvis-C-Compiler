#include "pch.h"
#include "Initializer.h"
#include "ExpressionTree.h"

using namespace WadeSpace;

Initializer::Initializer(shared_ptr<ExpressionTree> assignmentExpression) : assignmentExpression(assignmentExpression), initializerList(nullptr)
{
}

Initializer::Initializer(shared_ptr<vector<shared_ptr<Initializer>>> initializerList) : assignmentExpression(nullptr), initializerList(initializerList)
{
}
