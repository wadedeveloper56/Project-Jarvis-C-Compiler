#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace
{
	class Statement : public BaseStatement
	{
	public:
		Statement() = default;
		Statement(TokenType op, shared_ptr<BaseStatement> statement);
		virtual ~Statement() = default;
		Statement(const Statement& other) = default;
		Statement(Statement&& other) noexcept = default;
		Statement& operator=(const Statement& other) = default;
		Statement& operator=(Statement&& other) noexcept = default;
	};
}
