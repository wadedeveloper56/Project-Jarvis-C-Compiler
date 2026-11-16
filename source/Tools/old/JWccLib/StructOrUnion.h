#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"

using namespace std;

namespace WadeSpace
{
	class StructOrUnion
	{
		optional<TokenType> type;
	public:
		StructOrUnion() = default;
		StructOrUnion(optional<TokenType> type);
		virtual ~StructOrUnion() = default;
		StructOrUnion(const StructOrUnion& other) = default;
		StructOrUnion(StructOrUnion&& other) noexcept = default;
		StructOrUnion& operator=(const StructOrUnion& other) = default;
		StructOrUnion& operator=(StructOrUnion&& other) noexcept = default;
		[[nodiscard]] optional<TokenType> getType() const { return type; }
		[[nodiscard]] bool hasType() const { return type.has_value(); }
		void setType(TokenType type) { this->type = type; }
	};
}
