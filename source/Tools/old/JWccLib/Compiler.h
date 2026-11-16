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
		bool compileFile(istream& inStr, ostream& out);
		bool StartChildProcessAndWait(char*  commandLine);
		bool StartPreProcessor(char  *filename, const char* name);
		bool compile(istream& in, ostream& out, const string& filename);

		[[nodiscard]] shared_ptr<ProgramData> getProgramData() const;
		[[nodiscard]] shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> getTypedefList() const;
		[[nodiscard]] shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> getStructList() const;
		[[nodiscard]] shared_ptr<map<string, shared_ptr<FunctionDefinition>>> getFunctionList() const;
		void setProgramData(shared_ptr<ProgramData> data);
		void setTypedefList(shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> list);
		void setStructList(shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> list);
		void setFunctionList(shared_ptr<map<string, shared_ptr<FunctionDefinition>>> list);
		[[nodiscard]] bool hasProgramData() const;
		[[nodiscard]] bool hasTypedefList() const;
		[[nodiscard]] bool hasStructList() const;
		[[nodiscard]] bool hasFunctionList() const;
		[[nodiscard]] bool hasTypedef(const string& name) const;
		[[nodiscard]] bool hasFunction(const string& name) const;
		[[nodiscard]] bool hasStruct(const string& name) const;

		void addTypedef(const string& name, shared_ptr<DeclarationSpecifiersNode> declaration);
		void addFunction(const string& name, shared_ptr<FunctionDefinition> function);
		void addStruct(const string& name, shared_ptr<StructOrUnionSpecifier> structSpecifier);
		void addExternalDeclaration(shared_ptr<ExternalDeclaration> externalDeclaration);
		void addEnum(const string& name, shared_ptr<EnumSpecifier> enumSpecifier);
		shared_ptr<StructOrUnionSpecifier> findStruct(const string& name) const;
		shared_ptr<DeclarationSpecifiersNode> findTypedef(const string& name) const;
		shared_ptr<FunctionDefinition> findFunction(const string& name) const;
		shared_ptr<EnumSpecifier> findEnum(const string& name) const;
	};
}
