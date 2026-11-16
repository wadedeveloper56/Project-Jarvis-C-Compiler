#include "pch.h"
#include "JumpStatement.h"
#include "ExpressionTree.h"

using namespace WadeSpace;
using namespace std;

JumpStatement::JumpStatement(TokenType op, shared_ptr<ExpressionTree> exp) : BaseStatement(op, exp)
{
}

JumpStatement::JumpStatement(TokenType op) : BaseStatement(op)
{
}

JumpStatement::JumpStatement(TokenType op, shared_ptr<CToken> identifier) : BaseStatement(op, identifier)
{
}
