#pragma once

#include "SpecifierQualifierList.h"
#include "AbstractDeclarator.h"

namespace WadeSpace
{
	class TypeName
	{
		shared_ptr<SpecifierQualifierList> specifierQualifierList;
		shared_ptr<AbstractDeclarator> abstractDeclarator;
	public:
		TypeName() = default;
		TypeName(shared_ptr<SpecifierQualifierList> specifierQualifierList);
		TypeName(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<AbstractDeclarator> abstractDeclarator);
		virtual ~TypeName() = default;
		TypeName(const TypeName& other) = default;
		TypeName(TypeName&& other) noexcept = default;
		TypeName& operator=(const TypeName& other) = default;
		TypeName& operator=(TypeName&& other) noexcept = default;
		[[nodiscard]] shared_ptr<SpecifierQualifierList> getSpecifierQualifierList() const { return specifierQualifierList; }
		[[nodiscard]] shared_ptr<AbstractDeclarator> getAbstractDeclarator() const { return abstractDeclarator; }
		[[nodiscard]] bool hasSpecifierQualifierList() const { return specifierQualifierList != nullptr; }
		[[nodiscard]] bool hasAbstractDeclarator() const { return abstractDeclarator != nullptr; }
		void setSpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList) { this->specifierQualifierList = specifierQualifierList; }
		void setAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator) { this->abstractDeclarator = abstractDeclarator; }

	};
}
