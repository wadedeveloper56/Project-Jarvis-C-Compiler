#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "BaseStatement.h"

using namespace std;

namespace WadeSpace
{
	class CompoundStatement : public BaseStatement
	{
	public:
		CompoundStatement() = default;
		CompoundStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> list);
		CompoundStatement(shared_ptr<vector<shared_ptr<Declaration>>> list);
		CompoundStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> sList, shared_ptr<vector<shared_ptr<Declaration>>> dList);
		virtual ~CompoundStatement() override = default;

		CompoundStatement(const CompoundStatement& other) = default;
		CompoundStatement(CompoundStatement&& other) noexcept = default;
		CompoundStatement& operator=(const CompoundStatement& other) = default;
		CompoundStatement& operator=(CompoundStatement&& other) noexcept = default;
	};
}
