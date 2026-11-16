#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace 
{
	class ExpressionStatement : public BaseStatement
	{
	public:
		ExpressionStatement() = default;
		ExpressionStatement(shared_ptr<ExpressionTree> exp) : BaseStatement(TokenType::expression_statement, exp) {}
		virtual ~ExpressionStatement() = default;
		ExpressionStatement(const ExpressionStatement& other) = default;
		ExpressionStatement(ExpressionStatement&& other) noexcept = default;
		ExpressionStatement& operator=(const ExpressionStatement& other) = default;
		ExpressionStatement& operator=(ExpressionStatement&& other) noexcept = default;
	};
}
