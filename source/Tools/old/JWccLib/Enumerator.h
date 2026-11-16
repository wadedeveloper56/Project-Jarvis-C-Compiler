#pragma once

#include <string>
#include <vector>
#include <optional>

#include "Token.h"

using namespace std;

namespace WadeSpace
{
	class ExpressionTree;

	class Enumerator
	{
		shared_ptr<CToken> identifier;
		shared_ptr<ExpressionTree> constantExpression;
	public:
		Enumerator() = default;
		Enumerator(shared_ptr<CToken> identifier, shared_ptr<ExpressionTree> constantExpression);
		virtual ~Enumerator() = default;
		Enumerator(const Enumerator& other) = default;
		Enumerator(Enumerator&& other) noexcept = default;
		Enumerator& operator=(const Enumerator& other) = default;
		Enumerator& operator=(Enumerator&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getIdentifier() const { return identifier; }
		[[nodiscard]] shared_ptr<ExpressionTree> getConstantExpression() const { return constantExpression; }
		void setIdentifier(shared_ptr<CToken> identifier) { this->identifier = identifier; }
		void setConstantExpression(shared_ptr<ExpressionTree> constantExpression) { this->constantExpression = constantExpression; }
		bool hasConstantExpression() const { return constantExpression != nullptr; }
		bool hasIdentifier() const { return identifier != nullptr; }
	};
}
