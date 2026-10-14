#pragma once

#include <string>
#include <map>
#include <vector>
#include "ProgramData.h"
#include "ExternalDeclaration.h"
#include "StructOrUnionSpecifier.h"
#include "..\Arguments\ArgumentTable.h"

namespace WadeSpace
{
	class Compiler
	{
		shared_ptr<ProgramData> programData;
		shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> typedefList;
		shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> structList;
		shared_ptr<map<string, shared_ptr<FunctionDefinition>>> functionList;
		shared_ptr<map<string, shared_ptr<EnumSpecifier>>> enumList;
	public:
		Compiler();
		virtual ~Compiler() = default;
		void compileFile(istream& inStr, ostream& out, int& exitcode);
		void compile(istream& in, ostream& out, int& exitcode, const string& filename);
		[[nodiscard]] shared_ptr<ProgramData> getProgramData() const { return programData; }
		[[nodiscard]] shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> getTypedefList() const { return typedefList; }
		[[nodiscard]] shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> getStructList() const { return structList; }
		[[nodiscard]] shared_ptr<map<string, shared_ptr<FunctionDefinition>>> getFunctionList() const { return functionList; }
		void setProgramData(shared_ptr<ProgramData> data) { programData = data; }
		void setTypedefList(shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> list) { typedefList = list; }
		void setStructList(shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> list) { structList = list; }
		void setFunctionList(shared_ptr<map<string, shared_ptr<FunctionDefinition>>> list) { functionList = list; }
		[[nodiscard]] bool hasProgramData() const { return programData != nullptr; }
		[[nodiscard]] bool hasTypedefList() const { return typedefList != nullptr; }
		[[nodiscard]] bool hasStructList() const { return structList != nullptr; }
		[[nodiscard]] bool hasFunctionList() const { return functionList != nullptr; }
		[[nodiscard]] bool hasTypedef(const string& name) const { return typedefList->find(name) != typedefList->end(); }
		[[nodiscard]] bool hasFunction(const string& name) const { return functionList->find(name) != functionList->end(); }
		[[nodiscard]] bool hasStruct(const string& name) const { return structList->find(name) != structList->end(); }
		void addTypedef(const string& name, shared_ptr<DeclarationSpecifiersNode> declaration) { typedefList->insert({ name, declaration }); }
		void addFunction(const string& name, shared_ptr<FunctionDefinition> function) { functionList->insert({ name, function }); }
		void addStruct(const string& name, shared_ptr<StructOrUnionSpecifier> structSpecifier) { structList->insert({ name, structSpecifier }); }
		void addExternalDeclaration(shared_ptr<ExternalDeclaration> externalDeclaration) { programData->addExternalDeclaration(externalDeclaration); }
		void addEnum(const string& name, shared_ptr<EnumSpecifier> enumSpecifier) { enumList->insert({ name, enumSpecifier }); }
		shared_ptr<StructOrUnionSpecifier> findStruct(const string& name) const
		{
			auto it = structList->find(name);
			if (it != structList->end())
			{
				return it->second;
			}
			return nullptr;
		}
		shared_ptr<DeclarationSpecifiersNode> findTypedef(const string& name) const
		{
			auto it = typedefList->find(name);
			if (it != typedefList->end())
			{
				return it->second;
			}
			return nullptr;
		}
		shared_ptr<FunctionDefinition> findFunction(const string& name) const
		{
			auto it = functionList->find(name);
			if (it != functionList->end())
			{
				return it->second;
			}
			return nullptr;
		}
		shared_ptr<EnumSpecifier> findEnum(const string& name) const
		{
			auto it = enumList->find(name);
			if (it != enumList->end())
			{
				return it->second;
			}
			return nullptr;
		}
	};
}
