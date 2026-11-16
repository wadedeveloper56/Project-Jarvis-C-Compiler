#pragma once

#include <string>
#include <vector>
#include <optional>

#include "Token.h"
#include "TokenType.h"

using namespace std;

namespace WadeSpace
{
	class StorageClassSpecifier
	{
		shared_ptr<CToken> type;
	public:
		StorageClassSpecifier() = default;
		StorageClassSpecifier(const shared_ptr<CToken> type);
		virtual ~StorageClassSpecifier() = default;
		StorageClassSpecifier(const StorageClassSpecifier& other) = default;
		StorageClassSpecifier(StorageClassSpecifier&& other) noexcept = default;
		StorageClassSpecifier& operator=(const StorageClassSpecifier& other) = default;
		StorageClassSpecifier& operator=(StorageClassSpecifier&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getType() const { return type; }
		[[nodiscard]] bool hasType() const { return type != nullptr; }
		void setType(const shared_ptr<CToken> type) { this->type = type; }
	};
}
