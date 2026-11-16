#pragma once

#include <string>
#include <vector>
#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace
{
	class SelectionStatement : public BaseStatement
	{
	public:
		SelectionStatement() = default;
		SelectionStatement(TokenType op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement);
		SelectionStatement(TokenType op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> ifStatement, shared_ptr<BaseStatement> elseStatement);
		virtual ~SelectionStatement() = default;
		SelectionStatement(const SelectionStatement& other) = default;
		SelectionStatement(SelectionStatement&& other) noexcept = default;
		SelectionStatement& operator=(const SelectionStatement& other) = default;
		SelectionStatement& operator=(SelectionStatement&& other) noexcept = default;
	};
}
