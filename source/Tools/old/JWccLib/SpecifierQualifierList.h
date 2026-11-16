#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TypeSpecifier.h"
#include "TypeQualifier.h"

namespace WadeSpace
{
	class SpecifierQualifierList
	{
		shared_ptr<SpecifierQualifierList> specifierQualifierList;
		shared_ptr<TypeSpecifier> typeSpecifier;
		shared_ptr<TypeQualifier> typeQualifier;
	public:
		SpecifierQualifierList() = default;
		SpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<TypeSpecifier> typeSpecifier);
		SpecifierQualifierList(shared_ptr<TypeSpecifier> typeSpecifier);
		SpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<TypeQualifier> typeQualifier);
		SpecifierQualifierList(shared_ptr<TypeQualifier> typeQualifier);
		virtual ~SpecifierQualifierList() = default;
		SpecifierQualifierList(const SpecifierQualifierList& other) = default;
		SpecifierQualifierList(SpecifierQualifierList&& other) noexcept = default;
		SpecifierQualifierList& operator=(const SpecifierQualifierList& other) = default;
		SpecifierQualifierList& operator=(SpecifierQualifierList&& other) noexcept = default;
		[[nodiscard]] shared_ptr<SpecifierQualifierList> getSpecifierQualifierList() const { return specifierQualifierList; }
		[[nodiscard]] shared_ptr<TypeSpecifier> getTypeSpecifier() const { return typeSpecifier; }
		[[nodiscard]] shared_ptr<TypeQualifier> getTypeQualifier() const { return typeQualifier; }
		void setSpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList) { this->specifierQualifierList = specifierQualifierList; }
		void setTypeSpecifier(shared_ptr<TypeSpecifier> typeSpecifier) { this->typeSpecifier = typeSpecifier; }
		void setTypeQualifier(shared_ptr<TypeQualifier> typeQualifier) { this->typeQualifier = typeQualifier; }
		bool hasSpecifierQualifierList() const { return specifierQualifierList != nullptr; }
		bool hasTypeSpecifier() const { return typeSpecifier != nullptr; }
		bool hasTypeQualifier() const { return typeQualifier != nullptr; }
	};
}
