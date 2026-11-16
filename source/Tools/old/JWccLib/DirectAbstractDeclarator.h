#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"

using namespace std;

namespace WadeSpace
{
	class DirectAbstractDeclarator;
	class AbstractDeclarator;
	class ConstantExpression;
	class ParameterTypeList;
	class ExpressionTree;

	typedef struct DirectAbstractDeclaratorNode
	{
		shared_ptr<ParameterTypeList> parameterTypeList;
		shared_ptr<ExpressionTree> constantExpression;
		optional<TokenType> type;

		DirectAbstractDeclaratorNode() = default;
		DirectAbstractDeclaratorNode(shared_ptr<ParameterTypeList> parameterTypeList, shared_ptr<ExpressionTree> constantExpression, optional<TokenType> type);
		~DirectAbstractDeclaratorNode() = default;
		DirectAbstractDeclaratorNode(const DirectAbstractDeclaratorNode& other) = default;
		DirectAbstractDeclaratorNode(DirectAbstractDeclaratorNode&& other) noexcept = default;
		DirectAbstractDeclaratorNode& operator=(const DirectAbstractDeclaratorNode& other) = default;
		DirectAbstractDeclaratorNode& operator=(DirectAbstractDeclaratorNode&& other) noexcept = default;
		[[nodiscard]] shared_ptr<ParameterTypeList> getParameterTypeList() const { return parameterTypeList; }
		[[nodiscard]] shared_ptr<ExpressionTree> getConstantExpression() const { return constantExpression; }
		[[nodiscard]] optional<TokenType> getType() const { return type; }
		[[nodiscard]] bool hasParameterTypeList() const { return parameterTypeList != nullptr; }
		[[nodiscard]] bool hasConstantExpression() const { return constantExpression != nullptr; }
		[[nodiscard]] bool hasType() const { return type.has_value(); }
		void setParameterTypeList(shared_ptr<ParameterTypeList> parameterTypeList) { this->parameterTypeList = parameterTypeList; }
		void setConstantExpression(shared_ptr<ExpressionTree> constantExpression) { this->constantExpression = constantExpression; }
		void setType(optional<TokenType> type) { this->type = type; }
	}* DirectAbstractDeclaratorNodePtr;

	class DirectAbstractDeclarator
	{
		shared_ptr<AbstractDeclarator> abstractDeclarator;
		shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list;
	public:
		DirectAbstractDeclarator() = default;
		DirectAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator, shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list);
		virtual ~DirectAbstractDeclarator() = default;
		DirectAbstractDeclarator(const DirectAbstractDeclarator& other) = default;
		DirectAbstractDeclarator(DirectAbstractDeclarator&& other) noexcept = default;
		DirectAbstractDeclarator& operator=(const DirectAbstractDeclarator& other) = default;
		DirectAbstractDeclarator& operator=(DirectAbstractDeclarator&& other) noexcept = default;
		[[nodiscard]] shared_ptr<AbstractDeclarator> getAbstractDeclarator() const { return abstractDeclarator; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> getList() const { return list; }
		[[nodiscard]] bool hasAbstractDeclarator() const { return abstractDeclarator != nullptr; }
		[[nodiscard]] bool hasList() const { return list != nullptr; }
		void setAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator) { this->abstractDeclarator = abstractDeclarator; }
		void setList(shared_ptr<vector<shared_ptr<DirectAbstractDeclaratorNode>>> list) { this->list = list; }
		void addDirectAbstractDeclaratorNode(shared_ptr<DirectAbstractDeclaratorNode> node) 
		{
			if (list == nullptr) list = make_shared<vector<shared_ptr<DirectAbstractDeclaratorNode>>>();
			list->push_back(node);
		}
	};
}
