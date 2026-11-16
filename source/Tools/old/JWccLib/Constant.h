#pragma once

#include <optional>
#include "Token.h"
#include "TokenType.h"

using namespace std;

namespace WadeSpace
{
	class Constant
	{
		shared_ptr<CToken> iConst;
		shared_ptr<CToken> fConst;
		shared_ptr<CToken> strConst;
		optional<TokenType> type;
	public:
		Constant() = default;
		Constant(const shared_ptr<CToken> iConst, const shared_ptr<CToken> fConst, const shared_ptr<CToken> strConst, const optional<TokenType>& type);
		Constant(const Constant& other) = default;
		virtual ~Constant() = default;
		Constant(Constant&& other) noexcept = default;
		Constant& operator=(const Constant& other) = default;
		Constant& operator=(Constant&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getIConst() const { return iConst; }
		[[nodiscard]] shared_ptr<CToken> getFConst() const { return fConst; }
		[[nodiscard]] shared_ptr<CToken> getStrConst() const { return strConst; }
		[[nodiscard]] optional<TokenType> getType() const { return type; }
		void setIConst(const shared_ptr<CToken> iConst) { this->iConst = iConst; }
		void setFConst(const shared_ptr<CToken> fConst) { this->fConst = fConst; }
		void setStrConst(const shared_ptr<CToken> strConst) { this->strConst = strConst; }
		void setType(const optional<TokenType>& type) { this->type = type; }
		[[nodiscard]] bool hasIConst() const { return iConst != nullptr; }
		[[nodiscard]] bool hasFConst() const { return fConst != nullptr; }
		[[nodiscard]] bool hasStrConst() const { return strConst != nullptr; }
		[[nodiscard]] bool hasType() const { return type.has_value(); }
	};
}
