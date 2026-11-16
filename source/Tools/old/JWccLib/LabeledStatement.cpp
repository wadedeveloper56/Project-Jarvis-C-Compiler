#include "pch.h"
#include "LabeledStatement.h"

using namespace WadeSpace;
using namespace std;

LabeledStatement::LabeledStatement(shared_ptr<CToken> identifier, shared_ptr<BaseStatement> statement) : BaseStatement(identifier, statement)
{
}

LabeledStatement::LabeledStatement(TokenType op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement) : BaseStatement(op, exp, statement)
{
}

LabeledStatement::LabeledStatement(TokenType op, shared_ptr<BaseStatement> statement) : BaseStatement(op, statement)
{
}
