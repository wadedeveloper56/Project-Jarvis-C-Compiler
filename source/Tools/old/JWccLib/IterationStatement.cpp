#include "pch.h"
#include "IterationStatement.h"
#include "ExpressionTree.h"

using namespace WadeSpace;
using namespace std;

IterationStatement::IterationStatement(TokenType op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement)
	: BaseStatement(op, es1, es2, exp, statement)
{
}

IterationStatement::IterationStatement(TokenType op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<BaseStatement> statement) 
	: BaseStatement(op, es1, es2, statement)
{
}

IterationStatement::IterationStatement(TokenType op, shared_ptr<ExpressionTree> identifier, shared_ptr<BaseStatement> statement)
	: BaseStatement(op, identifier, statement)
{
}
