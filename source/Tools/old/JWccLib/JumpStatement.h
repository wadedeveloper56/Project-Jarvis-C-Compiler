#pragma once

#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace
{
	class ExpressionTree;

	class JumpStatement : public BaseStatement
	{
	public:
		JumpStatement() = default;       
		JumpStatement(TokenType op);
		JumpStatement(TokenType op, shared_ptr<CToken> identifier);
		JumpStatement(TokenType op, shared_ptr<ExpressionTree> identifier);
		virtual ~JumpStatement() = default;
		JumpStatement(const JumpStatement& other) = default;
		JumpStatement(JumpStatement&& other) noexcept = default;
		JumpStatement& operator=(const JumpStatement& other) = default;
		JumpStatement& operator=(JumpStatement&& other) noexcept = default;
	};
}
