#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "ExternalDeclaration.h"
#include "BaseCodeGenerator.h"

using namespace std;

namespace WadeSpace
{
	class BaseCodeGenerator;

	class ProgramData
	{
		shared_ptr<vector<shared_ptr<ExternalDeclaration>>> program;
		shared_ptr<BaseCodeGenerator> generator;
	public:
		ProgramData();
		virtual ~ProgramData() = default;
		void addExternalDeclaration(shared_ptr<ExternalDeclaration> data);
		shared_ptr<BaseCodeGenerator> processGlobalVariables();
		[[nodiscard]] shared_ptr<BaseCodeGenerator> getGenerator() const { return generator; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<ExternalDeclaration>>> getProgram() const { return program; }
		[[nodiscard]] bool hasProgram() const { return program != nullptr; }
		[[nodiscard]] bool hasGenerator() const { return generator != nullptr; }
		void setGenerator(shared_ptr<BaseCodeGenerator> generator) { this->generator = generator; }
		void setProgram(shared_ptr<vector<shared_ptr<ExternalDeclaration>>> program) { this->program = program; }

	private:
		void handleDeclaration(shared_ptr<Declaration> declaration, shared_ptr<vector<shared_ptr<VariableData>>> variableTable);
		void handleFunction(shared_ptr<FunctionDefinition> declaration, shared_ptr < vector < shared_ptr<FunctionData>>> functionTable);
		void collectLocalDeclarations(shared_ptr<BaseStatement> stmt, shared_ptr<vector<shared_ptr<VariableData>>> locals);
		shared_ptr<ParameterTypeList> getDeclarationParameterList(shared_ptr<vector<shared_ptr<InitDeclarator>>> initDeclaratorsList);
		TokenType getFunctionParameterType(shared_ptr<ParameterDeclaration> parameterDeclaration);
		string getParameterDeclarationName(shared_ptr<ParameterDeclaration> parameterDeclaration);
		TokenType getDeclarationSpecifiersType(shared_ptr<DeclarationSpecifiers> declaration_specifiers);
		shared_ptr<TypeSpecifier> findType(shared_ptr<Declaration> decl);
		shared_ptr<StorageClassSpecifier> findStorageSpecifier(shared_ptr<Declaration> decl);

		void handleStruct(shared_ptr<Declaration> declaration, shared_ptr<vector<shared_ptr<VariableData>>> variableTable);
		void handleUnion(shared_ptr<Declaration> declaration, shared_ptr<vector<shared_ptr<VariableData>>> variableTable);
		void handleTypedef(shared_ptr<Declaration> declaration, shared_ptr<vector<shared_ptr<VariableData>>> variableTable);
		void handleEnum(shared_ptr<Declaration> declaration, shared_ptr<vector<shared_ptr<VariableData>>> variableTable);
	};
}
