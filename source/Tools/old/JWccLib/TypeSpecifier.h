#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "EnumSpecifier.h"

namespace WadeSpace
{
	class ExternalDeclaration;
	class StructOrUnionSpecifier;

	class TypeSpecifier
	{
		shared_ptr<EnumSpecifier> enumSpec;
		optional<TokenType> type;
		shared_ptr<CToken> typePtr;
		shared_ptr<StructOrUnionSpecifier> structOrUnionSpecifier;
		shared_ptr<ExternalDeclaration> typedefInfo;
	public:
		TypeSpecifier() = default;
		TypeSpecifier(TokenType type, shared_ptr<CToken> token, shared_ptr<StructOrUnionSpecifier> structOrUnion, shared_ptr<EnumSpecifier> enumSpec, shared_ptr<ExternalDeclaration> extDecl);
		virtual ~TypeSpecifier() = default;
		TypeSpecifier(const TypeSpecifier& other) = default;
		TypeSpecifier(TypeSpecifier&& other) noexcept = default;
		TypeSpecifier& operator=(const TypeSpecifier& other)	= default;
		TypeSpecifier& operator=(TypeSpecifier&& other) noexcept = default;
		[[nodiscard]] optional<TokenType> getType() const { return type; }
		[[nodiscard]] shared_ptr<CToken> getTypePtr() const { return typePtr; }
		[[nodiscard]] shared_ptr<EnumSpecifier> getEnumSpec() const { return enumSpec; }
		[[nodiscard]] shared_ptr<StructOrUnionSpecifier> getStructOrUnionSpecifier() const { return structOrUnionSpecifier; }
		[[nodiscard]] shared_ptr<ExternalDeclaration> getTypedefInfo() const { return typedefInfo; }
		[[nodiscard]] bool hasType() const { return type.has_value(); }
		[[nodiscard]] bool hasTypePtr() const { return typePtr != nullptr; }
		[[nodiscard]] bool hasEnumSpec() const { return enumSpec != nullptr; }
		[[nodiscard]] bool hasStructOrUnionSpecifier() const { return structOrUnionSpecifier != nullptr; }
		[[nodiscard]] bool hasTypedefInfo() const { return typedefInfo != nullptr; }
		void setType(optional<TokenType> type) { this->type = type; }
		void setTypePtr(shared_ptr<CToken> typePtr) { this->typePtr = typePtr; }
		void setEnumSpec(shared_ptr<EnumSpecifier> enumSpec) { this->enumSpec = enumSpec; }
		void setStructOrUnionSpecifier(shared_ptr<StructOrUnionSpecifier> structOrUnionSpecifier) { this->structOrUnionSpecifier = structOrUnionSpecifier; }
		void setTypedefInfo(shared_ptr<ExternalDeclaration> typedefInfo) { this->typedefInfo = typedefInfo; }
	};
}
