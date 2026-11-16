#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace
{
	class IterationStatement : public BaseStatement
	{
	public:
		IterationStatement() = default;
		IterationStatement(TokenType op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement);
		IterationStatement(TokenType op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<BaseStatement> statement);
		IterationStatement(TokenType op, shared_ptr<ExpressionTree> identifier, shared_ptr<BaseStatement> statement);
		virtual ~IterationStatement() = default;
		IterationStatement(const IterationStatement& other) = default;
		IterationStatement(IterationStatement&& other) noexcept = default;
		IterationStatement& operator=(const IterationStatement& other) = default;
		IterationStatement& operator=(IterationStatement&& other) noexcept = default;
	};
}
