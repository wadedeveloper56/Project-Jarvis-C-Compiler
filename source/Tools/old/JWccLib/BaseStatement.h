#pragma once

#include <memory>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "Declaration.h"
#include "Token.h"
#include "ExpressionTree.h"

using namespace std;

namespace WadeSpace
{
	class ExpressionTree;

	class BaseStatement
	{
	protected:
		optional<TokenType> op;
		shared_ptr<CToken> identifier;
		shared_ptr<ExpressionTree> exp;
		shared_ptr<ExpressionTree> contExp;
		shared_ptr<BaseStatement> statement;
		shared_ptr<BaseStatement> es1;
		shared_ptr<BaseStatement> es2;
		shared_ptr<BaseStatement> statement2;
		shared_ptr<vector<shared_ptr<BaseStatement>>> statementList;
		shared_ptr<vector<shared_ptr<Declaration>>> declarationList;
	public:
		BaseStatement(optional<TokenType>, shared_ptr<CToken> identifier, shared_ptr<ExpressionTree> const exp, shared_ptr<ExpressionTree> const contExp,
			shared_ptr<BaseStatement> const statement, shared_ptr<BaseStatement> const es1, shared_ptr<BaseStatement> const es2,
			shared_ptr<BaseStatement> const statement2, vector<shared_ptr<BaseStatement>>* const statementList,
			shared_ptr<vector<shared_ptr<Declaration>>>	const declarationList);
		BaseStatement(optional<TokenType> op);
		BaseStatement(optional<TokenType> op, shared_ptr<CToken> identifier);
		BaseStatement(optional<TokenType> op, shared_ptr<ExpressionTree> exp);
		BaseStatement(optional<TokenType> op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement);
		BaseStatement(optional<TokenType> op, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> ifStatement, shared_ptr<BaseStatement> elseStatement);
		BaseStatement(optional<TokenType> op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<BaseStatement> statement);
		BaseStatement(optional<TokenType> op, shared_ptr<BaseStatement> es1, shared_ptr<BaseStatement> es2, shared_ptr<ExpressionTree> exp, shared_ptr<BaseStatement> statement);
		BaseStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> statementList);
		BaseStatement(shared_ptr<vector<shared_ptr<Declaration>>> declarationList);
		BaseStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> statementList, shared_ptr<vector<shared_ptr<Declaration>>> declarationList);
		BaseStatement(shared_ptr<CToken> identifier, shared_ptr<BaseStatement> statement);
		BaseStatement(optional<TokenType> op, shared_ptr<BaseStatement> statement);
		BaseStatement() = default;
		virtual ~BaseStatement() = default;
		BaseStatement(const BaseStatement& other) = default;
		BaseStatement(BaseStatement&& other) noexcept = default;
		BaseStatement& operator=(const BaseStatement& other) = default;
		BaseStatement& operator=(BaseStatement&& other) noexcept = default;
		[[nodiscard]] optional<TokenType> getOp() const { return op; }
		[[nodiscard]] shared_ptr<CToken> getIdentifier() const { return identifier; }
		[[nodiscard]] shared_ptr<ExpressionTree> getExp() const { return exp; }
		[[nodiscard]] shared_ptr<ExpressionTree> getContExp() const { return contExp; }
		[[nodiscard]] shared_ptr<BaseStatement> getStatement() const { return statement; }
		[[nodiscard]] shared_ptr<BaseStatement> getEs1() const { return es1; }
		[[nodiscard]] shared_ptr<BaseStatement> getEs2() const { return es2; }
		[[nodiscard]] shared_ptr<BaseStatement> getStatement2() const { return statement2; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<BaseStatement>>> getStatementList() const { return statementList; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<Declaration>>> getDeclarationList() const { return declarationList; }
		void setOp(TokenType op) { this->op = op; }
		void setIdentifier(shared_ptr<CToken> identifier) { this->identifier = identifier; }
		void setExp(shared_ptr<ExpressionTree> exp) { this->exp = exp; }
		void setContExp(shared_ptr<ExpressionTree> contExp) { this->contExp = contExp; }
		void setStatement(shared_ptr<BaseStatement> statement) { this->statement = statement; }
		void setEs1(shared_ptr<BaseStatement> es1) { this->es1 = es1; }
		void setEs2(shared_ptr<BaseStatement> es2) { this->es2 = es2; }
		void setStatement2(shared_ptr<BaseStatement> statement2) { this->statement2 = statement2; }
		void setStatementList(shared_ptr<vector<shared_ptr<BaseStatement>>> statementList) { this->statementList = statementList; }
		void setDeclarationList(shared_ptr<vector<shared_ptr<Declaration>>> declarationList) { this->declarationList = declarationList; }
		[[nodiscard]] bool hasIdentifier() const { return identifier != nullptr; }
		[[nodiscard]] bool hasExp() const { return exp != nullptr; }
		[[nodiscard]] bool hasContExp() const { return contExp != nullptr; }
		[[nodiscard]] bool hasStatement() const { return statement != nullptr; }
		[[nodiscard]] bool hasEs1() const { return es1 != nullptr; }
		[[nodiscard]] bool hasEs2() const { return es2 != nullptr; }
		[[nodiscard]] bool hasStatement2() const { return statement2 != nullptr; }
		[[nodiscard]] bool hasStatementList() const { return statementList != nullptr; }
		[[nodiscard]] bool hasDeclarationList() const { return declarationList != nullptr; }
		void addStatement(shared_ptr<BaseStatement> statement)
		{
			if (statementList == nullptr)
			{
				statementList = make_shared<vector<shared_ptr<BaseStatement>>>();
			}
			statementList->push_back(statement);
		}
		void addDeclaration(shared_ptr<Declaration> declaration)
		{
			if (declarationList == nullptr)
			{
				declarationList = make_shared<vector<shared_ptr<Declaration>>>();
			}
			declarationList->push_back(declaration);
		}
	};
}
