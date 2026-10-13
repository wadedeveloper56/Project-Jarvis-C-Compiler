#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include "ProgramData.h"
#include "BaseCodeGenerator.h"

using namespace std;

namespace WadeSpace
{
	class MasmCodeGenerator : public BaseCodeGenerator
	{
	public:
		MasmCodeGenerator() = default;
		MasmCodeGenerator(shared_ptr<vector<shared_ptr<VariableData>>> variableTable, shared_ptr<vector<shared_ptr<FunctionData>>> functionTable);
		virtual ~MasmCodeGenerator() = default;

		virtual void generateCode(ostream& out);

		MasmCodeGenerator(const MasmCodeGenerator& other);
		MasmCodeGenerator(MasmCodeGenerator&& other) noexcept;
		MasmCodeGenerator& operator=(const MasmCodeGenerator& other);
		MasmCodeGenerator& operator=(MasmCodeGenerator&& other) noexcept;

	private:
		// thread-local pointers used during expression evaluation to resolve identifiers
		static thread_local shared_ptr<vector<shared_ptr<VariableData>>> g_current_locals;
		static thread_local shared_ptr<vector<shared_ptr<VariableData>>> g_current_params;

		void handleIndividualFunction(ostream& out, shared_ptr<FunctionData> ptr);
		void handleInitializedVariable(ostream& out, shared_ptr<VariableData> ptr);
		void handleUUninitializedVariable(ostream& out, shared_ptr<VariableData> ptr);
		void handleVariableTable(ostream& out);
		void handleFunctionTable(ostream& out);
		void handleStructs(ostream& out);
		void outputVariable(ostream& out, shared_ptr<VariableData> ptr);
		void handleFunctionWithParameters(ostream& out, string name, shared_ptr<vector<shared_ptr<VariableData>>> list);
		void handleIndividualFunctionStatements(ostream& out, TokenType returnType, shared_ptr<BaseStatement> statements);
		string vectorToCommaSeparatedList(const vector<string>& vec);
		string getAsmType(TokenType type, bool isPointer, bool isUnsigned);
		void handlePrototype(ostream& out);
		string convertToAsmType(bool isUnsigned, bool isPointer, TokenType type);
		void handleFunctionTablePrototypes(ostream& out);
	};
}

