#pragma once

using namespace std;

namespace WadeSpace
{
	class ExpressionTree;

	class Initializer
	{
		shared_ptr<ExpressionTree> assignmentExpression;
		shared_ptr<vector<shared_ptr<Initializer>>> initializerList;
	public:
		Initializer() = default;
		Initializer(shared_ptr<ExpressionTree> assignmentExpression);
		Initializer(shared_ptr<vector<shared_ptr<Initializer>>> initializerList);
		virtual ~Initializer() = default;
		Initializer(Initializer&& other) noexcept = default;
		Initializer(const Initializer& other) = default;
		Initializer& operator=(const Initializer& other) = default;
		Initializer& operator=(Initializer&& other) noexcept = default;
		[[nodiscard]] shared_ptr<ExpressionTree> getAssignmentExpression() const { return assignmentExpression; }
		[[nodiscard]] bool hasAssignmentExpression() const { return assignmentExpression != nullptr; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<Initializer>>> getInitializerList() const { return initializerList; }
		[[nodiscard]] bool hasInitializerList() const { return initializerList != nullptr; }
		void setAssignmentExpression(shared_ptr<ExpressionTree> assignmentExpression) { this->assignmentExpression = assignmentExpression; }
		void setInitializerList(shared_ptr<vector<shared_ptr<Initializer>>> initializerList) { this->initializerList = initializerList; }
		void addInitializer(shared_ptr<Initializer> initializer)
		{
			if (!initializerList)
			{
				initializerList = make_shared<vector<shared_ptr<Initializer>>>();
			}
			initializerList->push_back(initializer);
		}
	};
}
