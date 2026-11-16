#pragma once

#include "Token.h"
#include "TokenType.h"

using namespace std;

namespace WadeSpace
{
	class ConstantExpression;
	class ParameterTypeList;
	class Declarator;
	class ExpressionTree;

	class DirectDeclarator
	{
		shared_ptr<CToken> identifier;
		shared_ptr<CToken> token1;
		shared_ptr<CToken> token2;
		shared_ptr<Declarator> declarator;
		shared_ptr<DirectDeclarator> directDeclarator;
		shared_ptr<ExpressionTree> constantExpression;
		shared_ptr<ParameterTypeList> parameterTypeList;
		shared_ptr<vector<shared_ptr<CToken>>> vectorOfStrings;
	public:
		DirectDeclarator() = default;
		DirectDeclarator( 
			shared_ptr<CToken> identifier, 
			shared_ptr<CToken> token1,
			shared_ptr<CToken> token2,
			shared_ptr<Declarator> declarator, 
			shared_ptr<DirectDeclarator> directDeclarator,
			shared_ptr<ExpressionTree> constantExpression, 
			shared_ptr<ParameterTypeList> parameterTypeList,
			shared_ptr<vector<shared_ptr<CToken>>> vectorOfStrings
		);
		virtual ~DirectDeclarator() = default;
		DirectDeclarator(const DirectDeclarator& other) = default;
		DirectDeclarator(DirectDeclarator&& other) noexcept = default;
		DirectDeclarator& operator=(const DirectDeclarator& other) = default;
		DirectDeclarator& operator=(DirectDeclarator&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getIdentifier() const { return identifier; };
		[[nodiscard]] shared_ptr<CToken> getToken1() const { return token1; };
		[[nodiscard]] shared_ptr<CToken> getToken2() const { return token2; };
		[[nodiscard]] shared_ptr<Declarator> getDeclarator() const { return declarator; };
		[[nodiscard]] shared_ptr<DirectDeclarator> getDirectDeclarator() const { return directDeclarator; };
		[[nodiscard]] shared_ptr<ExpressionTree> getConstantExpression() const { return constantExpression; };
		[[nodiscard]] shared_ptr<ParameterTypeList> getParameterTypeList() const { return parameterTypeList; };
		[[nodiscard]] shared_ptr<vector<shared_ptr<CToken>>> getVectorOfStrings() const { return vectorOfStrings; };
		void setIdentifier(shared_ptr<CToken> identifier) { this->identifier = identifier; };
		void setToken1(shared_ptr<CToken> token1) { this->token1 = token1; };
		void setToken2(shared_ptr<CToken> token2) { this->token2 = token2; };
		void setDeclarator(shared_ptr<Declarator> declarator) { this->declarator = declarator; };
		void setDirectDeclarator(shared_ptr<DirectDeclarator> directDeclarator) { this->directDeclarator = directDeclarator; };
		void setConstantExpression(shared_ptr<ExpressionTree> constantExpression) { this->constantExpression = constantExpression; };
		void setParameterTypeList(shared_ptr<ParameterTypeList> parameterTypeList) { this->parameterTypeList = parameterTypeList; };
		void setVectorOfStrings(shared_ptr<vector<shared_ptr<CToken>>> vectorOfStrings) { this->vectorOfStrings = vectorOfStrings; };
		[[nodiscard]] bool hasIdentifier() const { return identifier != nullptr; };
		[[nodiscard]] bool hasToken1() const { return token1 != nullptr; };
		[[nodiscard]] bool hasToken2() const { return token2 != nullptr; };
		[[nodiscard]] bool hasDeclarator() const { return declarator != nullptr; };
		[[nodiscard]] bool hasDirectDeclarator() const { return directDeclarator != nullptr; };
		[[nodiscard]] bool hasConstantExpression() const { return constantExpression != nullptr; };
		[[nodiscard]] bool hasParameterTypeList() const { return parameterTypeList != nullptr; };
		[[nodiscard]] bool hasVectorOfStrings() const { return vectorOfStrings != nullptr; };
		void addString(shared_ptr<CToken> str) 
		{
			if (vectorOfStrings == nullptr) vectorOfStrings = make_shared<vector<shared_ptr<CToken>>>();
			vectorOfStrings->push_back(str);
		}
	};
}
