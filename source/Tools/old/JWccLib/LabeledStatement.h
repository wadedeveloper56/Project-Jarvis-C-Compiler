#pragma once

#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace
{
	class LabeledStatement : public BaseStatement
	{
	public:
		LabeledStatement() = default;
		LabeledStatement(shared_ptr<CToken> identifier, shared_ptr<BaseStatement> statement);
		LabeledStatement(TokenType op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement);
		LabeledStatement(TokenType op, shared_ptr<BaseStatement> statement);
		virtual ~LabeledStatement() = default;
		LabeledStatement(const LabeledStatement& other) = default;
		LabeledStatement(LabeledStatement&& other) noexcept = default;
		LabeledStatement& operator=(const LabeledStatement& other) = default;
		LabeledStatement& operator=(LabeledStatement&& other) noexcept = default;
	};
}
