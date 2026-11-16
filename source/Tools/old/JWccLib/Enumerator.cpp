#include "pch.h"
#include "Enumerator.h"
#include "ExpressionTree.h"

using namespace WadeSpace;

Enumerator::Enumerator(shared_ptr<CToken> identifier, shared_ptr<ExpressionTree> constantExpression) : identifier(identifier), constantExpression(constantExpression)
{
}
