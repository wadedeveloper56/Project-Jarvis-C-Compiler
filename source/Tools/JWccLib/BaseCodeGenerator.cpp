#include "pch.h"
#include "BaseCodeGenerator.h"

using namespace WadeSpace;

BaseCodeGenerator::BaseCodeGenerator(shared_ptr<vector<shared_ptr<VariableData>>> variable_table, shared_ptr<vector<shared_ptr<FunctionData>>> function_table) 
	: variableTable(variable_table),	functionTable(function_table)
{
}
