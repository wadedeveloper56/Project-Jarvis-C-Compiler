#pragma once

#include "TokenType.h"
#include "FunctionDefinition.h"
#include "Declaration.h"

using namespace std;

namespace WadeSpace
{
	class ExternalDeclaration
	{
		shared_ptr<FunctionDefinition> functionDefinition;
		shared_ptr<Declaration> declaration;
	public:
		ExternalDeclaration() = default;
		ExternalDeclaration(shared_ptr<FunctionDefinition> functionDefinition);
		ExternalDeclaration(shared_ptr<Declaration> declaration);
		virtual ~ExternalDeclaration() = default;
		ExternalDeclaration(const ExternalDeclaration& other) = default;
		ExternalDeclaration(ExternalDeclaration&& other) noexcept = default;
		ExternalDeclaration& operator=(const ExternalDeclaration& other) = default;
		ExternalDeclaration& operator=(ExternalDeclaration&& other) noexcept = default;
		[[nodiscard]] shared_ptr<FunctionDefinition> getFunctionDefinition() const { return functionDefinition; };
		[[nodiscard]] shared_ptr<Declaration> getDeclaration() const { return declaration; };
		[[nodiscard]] bool hasFunction() const { return functionDefinition != nullptr; };
		[[nodiscard]] bool hasDeclaration() const { return declaration != nullptr; };
		void setFunctionDefinition(shared_ptr<FunctionDefinition> functionDefinition) { this->functionDefinition = functionDefinition; };
		void setDeclaration(shared_ptr<Declaration> declaration) { this->declaration = declaration; };
		shared_ptr<TypeSpecifier> findType(shared_ptr<Declaration> decl)
		{
			shared_ptr<DeclarationSpecifiers> specifiers = decl->getDeclarationSpecifiers();
			shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> list = specifiers->getDeclarationSpecifiersNodeList();
			for (shared_ptr<DeclarationSpecifiersNode> node : *list)
			{
				if (node->getTypeSpecifier() != nullptr)
				{
					return node->getTypeSpecifier();
				}
			}
			return nullptr;
		}
		shared_ptr<StorageClassSpecifier> findStorageSpecifier(shared_ptr<Declaration> decl)
		{
			shared_ptr<DeclarationSpecifiers> specifiers = decl->getDeclarationSpecifiers();
			shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> list = specifiers->getDeclarationSpecifiersNodeList();
			for (shared_ptr<DeclarationSpecifiersNode> node : *list)
			{
				if (node->getStorageClassSpecifier() != nullptr)
				{
					return node->getStorageClassSpecifier();
				}
			}
			return nullptr;
		}
		bool isTypedef()
		{
			if (declaration != nullptr)
			{
				shared_ptr<StorageClassSpecifier> temp = findStorageSpecifier(declaration);
				if (temp != nullptr && temp->getType()->getKeywordName() == "typedef")
				{
					return true;
				}
			}
			return false;
		}

	};
}
