#include "pch.h"
#include "ExpressionTree.h"
#include "TypeName.h"

using namespace WadeSpace;

shared_ptr<TreeNode> ExpressionTree::evaluate(ostream& out, void (*process)(ostream& out, shared_ptr<TreeNodeData> left, shared_ptr<TreeNodeData> right, shared_ptr<TreeNodeData> current))
{
	return postOrderTraversal(out, tree, process);
}

shared_ptr<TreeNode> ExpressionTree::postOrderTraversal(ostream& out, shared_ptr<TreeNode> node, void (*process)(ostream& out, shared_ptr<TreeNodeData> left, shared_ptr<TreeNodeData> right, shared_ptr<TreeNodeData> current))
{
	if (node == nullptr)
		return nullptr;
	if (node->getLeft() == nullptr && node->getRight() == nullptr)
	{
		if (process)
		{
			(*process)(out, nullptr, nullptr, node->getData());
		}
		return node;
	}
	shared_ptr<TreeNode> left = postOrderTraversal(out, node->getLeft(), process);
	shared_ptr<TreeNode> right = postOrderTraversal(out, node->getRight(), process);
	shared_ptr<TreeNodeData> data = node->getData();
	if (data && process)
	{
		(*process)(out, left ? left->getData() : nullptr, right ? right->getData() : nullptr, data);
	}
	return node;
}

TreeNodeData::TreeNodeData(
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
)
{
	this->type = type;
	this->token1 = token1;
	this->token2 = token2;
	this->lexp = lexp;
	this->exp1 = exp1;
	this->exp2 = exp2;
	this->argumentList = argumentList;
	this->identifier = identifier;
	this->initializerList = initializerList;
	this->typeName = typeName;
	this->token3 = token3;
	this->constant = constant;
	this->op = op;
}

TreeNode::TreeNode(shared_ptr<TreeNodeData> data, shared_ptr<TreeNode> left, shared_ptr<TreeNode> right)
{
	this->data = data;
	this->left = left;
	this->right = right;
}

ExpressionTree::ExpressionTree(shared_ptr<TreeNode> data)
{
	stak = new stack<shared_ptr<TreeNode>>();
	tree = data;
}
