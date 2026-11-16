#include "pch.h"
#include "Statement.h"

using namespace WadeSpace;
using namespace std;

Statement::Statement(TokenType op, shared_ptr<BaseStatement> statement) : BaseStatement(op, statement)
{
}
