#pragma once

#include "StorageClassSpecifier.h"
#include "TypeSpecifier.h"
#include "TypeQualifier.h"

namespace WadeSpace
{
	class DeclarationSpecifiersNode
	{
		shared_ptr<StorageClassSpecifier> storageClassSpecifier;
		shared_ptr<TypeSpecifier> typeSpecifier;
		shared_ptr<TypeQualifier> typeQualifier;
	public:
		DeclarationSpecifiersNode() = default;
		DeclarationSpecifiersNode(shared_ptr<StorageClassSpecifier> storageClassSpecifier, shared_ptr<TypeSpecifier> typeSpecifier, shared_ptr<TypeQualifier> const typeQualifier);
		virtual ~DeclarationSpecifiersNode() = default;
		DeclarationSpecifiersNode(const DeclarationSpecifiersNode& other) = default;
		DeclarationSpecifiersNode(DeclarationSpecifiersNode&& other) noexcept = default;
		DeclarationSpecifiersNode& operator=(const DeclarationSpecifiersNode& other) = default;
		DeclarationSpecifiersNode& operator=(DeclarationSpecifiersNode&& other) noexcept = default;
		[[nodiscard]] shared_ptr<StorageClassSpecifier> getStorageClassSpecifier() const { return storageClassSpecifier; }
		[[nodiscard]] shared_ptr<TypeSpecifier> getTypeSpecifier() const { return typeSpecifier; }
		[[nodiscard]] shared_ptr<TypeQualifier> getTypeQualifier() const { return typeQualifier; }
		[[nodiscard]] bool hasStorageClassSpecifier() const { return storageClassSpecifier != nullptr; }
		[[nodiscard]] bool hasTypeSpecifier() const { return typeSpecifier != nullptr; }
		[[nodiscard]] bool hasTypeQualifier() const { return typeQualifier != nullptr; }
		void setStorageClassSpecifier(shared_ptr<StorageClassSpecifier> storageClassSpecifier) { this->storageClassSpecifier = storageClassSpecifier; }
		void setTypeSpecifier(shared_ptr<TypeSpecifier> typeSpecifier) { this->typeSpecifier = typeSpecifier; }
		void setTypeQualifier(shared_ptr<TypeQualifier> typeQualifier) { this->typeQualifier = typeQualifier; }
	};

	class DeclarationSpecifiers
	{
		shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> declarationSpecifiersNodeList;
	public:
		DeclarationSpecifiers() = default;
		virtual ~DeclarationSpecifiers() = default;
		void addDeclarationSpecifiersNode(shared_ptr<StorageClassSpecifier> storageClassSpecifier, shared_ptr<TypeSpecifier> typeSpecifier, shared_ptr<TypeQualifier> typeQualifier);
		DeclarationSpecifiers(const DeclarationSpecifiers& other) = default;
		DeclarationSpecifiers(DeclarationSpecifiers&& other) noexcept = default;
		DeclarationSpecifiers& operator=(const DeclarationSpecifiers& other) = default;
		DeclarationSpecifiers& operator=(DeclarationSpecifiers&& other) noexcept = default;
		[[nodiscard]] shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> getDeclarationSpecifiersNodeList() const { return declarationSpecifiersNodeList; }
		void setDeclarationSpecifiersNodeList(shared_ptr<vector<shared_ptr<DeclarationSpecifiersNode>>> declarationSpecifiersNodeList) { this->declarationSpecifiersNodeList = declarationSpecifiersNodeList; }
		[[nodiscard]] bool hasDeclarationSpecifiersNodeList() const { return declarationSpecifiersNodeList != nullptr; }
	};
}
