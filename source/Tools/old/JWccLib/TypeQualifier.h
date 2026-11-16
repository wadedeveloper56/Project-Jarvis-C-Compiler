#pragma once

#include "Token.h"
#include "TokenType.h"

using namespace std;

namespace WadeSpace
{
	class TypeQualifier
	{
		shared_ptr<CToken> type;
	public:
		TypeQualifier() = default;
		TypeQualifier(shared_ptr<CToken> type);
		~TypeQualifier() = default;
		TypeQualifier(const TypeQualifier& other) = default;
		TypeQualifier(TypeQualifier&& other) noexcept = default;
		TypeQualifier& operator=(const TypeQualifier& other) = default;
		TypeQualifier& operator=(TypeQualifier&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getType() const { return type; }
		[[nodiscard]] bool hasType() const { return type != nullptr; }
		void setType(shared_ptr<CToken> type) { this->type = type; }
	};
}
