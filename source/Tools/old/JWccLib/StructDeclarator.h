#pragma once

#include "Declarator.h"

namespace WadeSpace
{
	class ExpressionTree;

	class StructDeclarator
	{
		shared_ptr<Declarator> declarator;
		shared_ptr<ExpressionTree> constantExpression;
	public:
		StructDeclarator() = default;
		StructDeclarator(shared_ptr<Declarator> declarator);
		StructDeclarator(shared_ptr<ExpressionTree> constantExpression);
		StructDeclarator(shared_ptr<Declarator> declarator, shared_ptr<ExpressionTree> constantExpression);
		virtual ~StructDeclarator() = default;
		StructDeclarator(const StructDeclarator& other) = default;
		StructDeclarator(StructDeclarator&& other) noexcept = default;
		StructDeclarator& operator=(const StructDeclarator& other) = default;
		StructDeclarator& operator=(StructDeclarator&& other) noexcept = default;
		[[nodiscard]] shared_ptr<Declarator> getDeclarator() const { return declarator; }
		[[nodiscard]] shared_ptr<ExpressionTree> getConstantExpression() const { return constantExpression; }
		[[nodiscard]] bool hasDeclarator() const { return declarator != nullptr; }
		[[nodiscard]] bool hasConstantExpression() const { return constantExpression != nullptr; }
		void setDeclarator(shared_ptr<Declarator> declarator) { this->declarator = declarator; }
		void setConstantExpression(shared_ptr<ExpressionTree> constantExpression) { this->constantExpression = constantExpression; }
	};
}
