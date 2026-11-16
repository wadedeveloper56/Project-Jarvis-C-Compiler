#include "pch.h"
#include "SelectionStatement.h"

using namespace WadeSpace;
using namespace std;

SelectionStatement::SelectionStatement(TokenType op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> ifStatement, shared_ptr<BaseStatement> elseStatement) 
	: BaseStatement(op, exp, ifStatement, elseStatement)
{
}

SelectionStatement::SelectionStatement(TokenType op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement) : BaseStatement(op, exp, statement)
{
}
