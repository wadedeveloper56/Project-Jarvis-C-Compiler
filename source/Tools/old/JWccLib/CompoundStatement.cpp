#include "pch.h"
#include "CompoundStatement.h"

using namespace WadeSpace;
using namespace std;

CompoundStatement::CompoundStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> list) : BaseStatement(list)
{
} 

CompoundStatement::CompoundStatement(shared_ptr<vector<shared_ptr<Declaration>>> list) : BaseStatement(list)
{
}

CompoundStatement::CompoundStatement(shared_ptr<vector<shared_ptr<BaseStatement>>> sList, shared_ptr<vector<shared_ptr<Declaration>>> dList) : BaseStatement(sList, dList)
{
}
