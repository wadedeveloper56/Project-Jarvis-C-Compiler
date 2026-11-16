#include "pch.h"
#include "ParameterTypeList.h"

using namespace WadeSpace;

ParameterTypeList::ParameterTypeList(shared_ptr<vector<shared_ptr<ParameterDeclaration>>> vectorParameterDeclaration, bool ellipsis) : vectorParameterDeclaration(vectorParameterDeclaration), ellipsis(ellipsis)
{
}
