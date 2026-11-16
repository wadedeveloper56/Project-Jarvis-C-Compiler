#pragma once

#include <vector>
#include <stack>
#include "Token.h"
#include "Constant.h"
#include "NodeType.h"

using namespace std;

namespace WadeSpace
{
	class ExpressionTree;
	class AssignmentExpression;
	class Initializer;
	class TypeName;

	class TreeNodeData
	{
		NodeType type;
		shared_ptr<CToken> token1;
		shared_ptr<CToken> token2;
		shared_ptr<ExpressionTree> lexp;
		shared_ptr<ExpressionTree> exp1;
		shared_ptr<ExpressionTree> exp2;
		shared_ptr<vector<shared_ptr<ExpressionTree>>> argumentList;
		shared_ptr<CToken> identifier;
		shared_ptr<vector<shared_ptr<Initializer>>> initializerList;
		shared_ptr<TypeName> typeName;
		shared_ptr<CToken> token3;
		shared_ptr<Constant> constant;
		shared_ptr<CToken> op;
	public:
		TreeNodeData() = default;
		TreeNodeData(
			const NodeType type,
			const shared_ptr<CToken> token1,
			const shared_ptr<CToken> token2,
			shared_ptr<ExpressionTree> const lexp,
			shared_ptr<ExpressionTree> const exp1,
			shared_ptr<ExpressionTree> const exp2,
			shared_ptr<vector<shared_ptr<ExpressionTree>>> argumentList,
			shared_ptr<CToken> identifier,
			shared_ptr<vector<shared_ptr<Initializer>>> initializerList,
			shared_ptr<TypeName> typeName,
			shared_ptr<CToken> token3,
			shared_ptr<Constant> constant,
			shared_ptr<CToken> op
		);
		virtual ~TreeNodeData() = default;
		TreeNodeData(const TreeNodeData& other) = default;
		TreeNodeData(TreeNodeData&& other) noexcept = default;
		TreeNodeData& operator=(const TreeNodeData& other) = default;
		TreeNodeData& operator=(TreeNodeData&& other) noexcept;
		[[nodiscard]] NodeType getType() const { return type; }
		[[nodiscard]] shared_ptr<CToken> getToken1() const { return token1; }
		[[nodiscard]] shared_ptr<CToken> getToken2() const { return token2; }
		[[nodiscard]] shared_ptr<ExpressionTree> getLExp() const { return lexp; }
		[[nodiscard]] shared_ptr<ExpressionTree> getExp1() const { return exp1; }
		[[nodiscard]] shared_ptr<ExpressionTree> getExp2() const { return exp2; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<ExpressionTree>>> getArgumentList() const { return argumentList; }
		[[nodiscard]] shared_ptr<CToken> getIdentifier() const { return identifier; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<Initializer>>> getInitializerList() const { return initializerList; }
		[[nodiscard]] shared_ptr<TypeName> getTypeName() const { return typeName; }
		[[nodiscard]] shared_ptr<CToken> getToken3() const { return token3; }
		[[nodiscard]] shared_ptr<Constant> getConstant() const { return constant; }
		[[nodiscard]] shared_ptr<CToken> getOp() const { return op; }
		void setType(const NodeType type) { this->type = type; }
		void setToken1(const shared_ptr<CToken> token1) { this->token1 = token1; }
		void setToken2(const shared_ptr<CToken> token2) { this->token2 = token2; }
		void setLExp(shared_ptr<ExpressionTree> lexp) { this->lexp = lexp; }
		void setExp1(shared_ptr<ExpressionTree> exp1) { this->exp1 = exp1; }
		void setExp2(shared_ptr<ExpressionTree> exp2) { this->exp2 = exp2; }
		void setArgumentList(shared_ptr<vector<shared_ptr<ExpressionTree>>> argumentList) { this->argumentList = argumentList; }
		void setIdentifier(shared_ptr<CToken> identifier) { this->identifier = identifier; }
		void setInitializerList(shared_ptr<vector<shared_ptr<Initializer>>> initializerList) { this->initializerList = initializerList; }
		void setTypeName(shared_ptr<TypeName> typeName) { this->typeName = typeName; }
		void setToken3(shared_ptr<CToken> token3) { this->token3 = token3; }
		void setConstant(shared_ptr<Constant> constant) { this->constant = constant; }
		void setOp(shared_ptr<CToken> op) { this->op = op; }
		bool hasToken1() const { return token1 != nullptr; }
		bool hasToken2() const { return token2 != nullptr; }
		bool hasLExp() const { return lexp != nullptr; }
		bool hasExp1() const { return exp1 != nullptr; }
		bool hasExp2() const { return exp2 != nullptr; }
		bool hasArgumentList() const { return argumentList != nullptr; }
		bool hasIdentifier() const { return identifier != nullptr; }
		bool hasInitializerList() const { return initializerList != nullptr; }
		bool hasTypeName() const { return typeName != nullptr; }
		bool hasToken3() const { return token3 != nullptr; }
		bool hasConstant() const { return constant != nullptr; }
		bool hasOp() const { return op != nullptr; }
		[[nodiscard]] shared_ptr<CToken> getIConst() const { return constant ? constant->getIConst() : nullptr; }
		[[nodiscard]] shared_ptr<CToken> getFConst() const { return constant ? constant->getFConst() : nullptr; }
		[[nodiscard]] shared_ptr<CToken> getStrConst() const { return constant ? constant->getStrConst() : nullptr; }
		[[nodiscard]] optional<TokenType> getTokenType() const { return constant ? constant->getType() : nullopt; }
		void addArgument(shared_ptr<ExpressionTree> argument)
		{
			if (!argumentList)
			{
				argumentList = make_shared<vector<shared_ptr<ExpressionTree>>>();
			}
			argumentList->push_back(argument);
		}
	};

	class TreeNode
	{
		shared_ptr<TreeNodeData> data;
		shared_ptr<TreeNode> left;
		shared_ptr<TreeNode> right;
	public:
		TreeNode() = default;
		TreeNode(shared_ptr<TreeNodeData> data, shared_ptr<TreeNode> left = nullptr, shared_ptr<TreeNode> right = nullptr);
		virtual ~TreeNode() = default;
		TreeNode(const TreeNode& other) = default;
		TreeNode(TreeNode&& other) noexcept = default;
		TreeNode& operator=(const TreeNode& other) = default;
		TreeNode& operator=(TreeNode&& other) noexcept = default;
		[[nodiscard]] shared_ptr<TreeNodeData> getData() const { return data; }
		[[nodiscard]] shared_ptr<TreeNode> getLeft() const { return left; }
		[[nodiscard]] shared_ptr<TreeNode> getRight() const { return right; }
		void setData(shared_ptr<TreeNodeData> data) { this->data = data; }
		void setLeft(shared_ptr<TreeNode> left) { this->left = left; }
		void setRight(shared_ptr<TreeNode> right) { this->right = right; }
		bool hasData() const { return data != nullptr; }
		bool hasLeft() const { return left != nullptr; }
		bool hasRight() const { return right != nullptr; }
	};

	class ExpressionTree
	{
		stack<shared_ptr<TreeNode>>* stak;
		shared_ptr<TreeNode> tree;
	public:
		ExpressionTree() = default;
		ExpressionTree(shared_ptr<TreeNode> data);
		virtual ~ExpressionTree() = default;
		ExpressionTree(const ExpressionTree& other) = default;
		ExpressionTree(ExpressionTree&& other) noexcept = default;
		ExpressionTree& operator=(const ExpressionTree& other) = default;
		ExpressionTree& operator=(ExpressionTree&& other) noexcept = default;
		shared_ptr<TreeNode> evaluate(ostream& out, void (*process)(ostream& out, shared_ptr<TreeNodeData> left, shared_ptr<TreeNodeData> right, shared_ptr<TreeNodeData> current));
		shared_ptr<TreeNode> postOrderTraversal(ostream& out, shared_ptr<TreeNode> node, void (*process)(ostream& out, shared_ptr<TreeNodeData> left, shared_ptr<TreeNodeData> right, shared_ptr<TreeNodeData> current));
		[[nodiscard]] shared_ptr<TreeNode> getTree() const { return tree; }
		[[nodiscard]] stack<shared_ptr<TreeNode>>* getStak() const { return stak; }
		void setTree(shared_ptr<TreeNode> tree) { this->tree = tree; }
		void setStak(stack<shared_ptr<TreeNode>>* stak) { this->stak = stak; }
		bool hasTree() const { return tree != nullptr; }
		bool hasStak() const { return stak != nullptr; }
		[[nodiscard]] shared_ptr<TreeNode> getLeft() const { return tree ? tree->getLeft() : nullptr; }
		[[nodiscard]] shared_ptr<TreeNode> getRight() const { return tree ? tree->getRight() : nullptr; }
		[[nodiscard]] shared_ptr<TreeNodeData> getData() const { return tree ? tree->getData() : nullptr; }
	};
}
