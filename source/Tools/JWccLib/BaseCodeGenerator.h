#pragma once

#include "BaseStatement.h"
#include "GlobalVars.h"
#include "Initializer.h"
#include "TokenType.h"
#include "StructOrUnionSpecifier.h"
#include "ParameterTypeList.h"

#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace WadeSpace
{
	class VariableData
	{
	public:
		string name;
		optional<TokenType> type;
		bool pointer;
		unsigned long long arraySize;
		shared_ptr<Initializer> initializer;
		string structName;
		shared_ptr<StructOrUnionSpecifier> suSpec;
		bool unsign;
		shared_ptr<ParameterTypeList> plist;

		VariableData() = default;
		virtual ~VariableData() = default;
		VariableData(const VariableData& other) = default;
		VariableData(VariableData&& other) noexcept = default;
		VariableData& operator=(const VariableData& other) = default;
		VariableData& operator=(VariableData&& other) noexcept = default;
	};

	class FunctionData
	{
	public:
		string name;
		optional<TokenType> type;
		shared_ptr<vector<shared_ptr<VariableData>>> parameters;
		shared_ptr<BaseStatement> statements;
		// local variables declared inside the function body
		shared_ptr<vector<shared_ptr<VariableData>>> localVariables;

		FunctionData() = default;
		virtual ~FunctionData() = default;
		FunctionData(const FunctionData& other) = default;
		FunctionData(FunctionData&& other) noexcept = default;
		FunctionData& operator=(const FunctionData& other) = default;
		FunctionData& operator=(FunctionData&& other) noexcept = default;
	};

	class BaseCodeGenerator
	{
	protected:
		shared_ptr<vector<shared_ptr<VariableData>>> variableTable;
		shared_ptr<vector<shared_ptr<FunctionData>>> functionTable;
	public:
		BaseCodeGenerator() = default;
		BaseCodeGenerator(shared_ptr<vector<shared_ptr<VariableData>>> variable_table, shared_ptr<vector<shared_ptr<FunctionData>>> function_table);
		virtual ~BaseCodeGenerator() = default;

		BaseCodeGenerator(const BaseCodeGenerator& other) = default;
		BaseCodeGenerator(BaseCodeGenerator&& other) noexcept = default;
		BaseCodeGenerator& operator=(const BaseCodeGenerator& other) = default;
		BaseCodeGenerator& operator=(BaseCodeGenerator&& other) noexcept = default;

		virtual void generateCode(ostream& out) = 0;

		[[nodiscard]] shared_ptr<vector<shared_ptr<VariableData>>> getVariableTable() const {	return variableTable; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<FunctionData>>> getFunctionTable() const { return functionTable; }
		void setVariableTable(shared_ptr<vector<shared_ptr<VariableData>>> variable_table) { variableTable = variable_table; }
		void setFunctionTable(shared_ptr<vector<shared_ptr<FunctionData>>> function_table) { functionTable = function_table; }
		[[nodiscard]] bool hasVariableTable() const { return variableTable != nullptr; }
		[[nodiscard]] bool hasFunctionTable() const { return functionTable != nullptr; }
	};
}
