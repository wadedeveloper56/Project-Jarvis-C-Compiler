#pragma once

#include <string>
#include <vector>
#include <optional>
#include "SpecifierQualifierList.h"
#include "StructDeclarator.h"

using namespace std;

namespace WadeSpace
{
	class StructDeclaration
	{
		shared_ptr<SpecifierQualifierList> specifierQualifierList;
		shared_ptr<vector<shared_ptr<StructDeclarator>>> vectorStructDeclarator;
	public:
		StructDeclaration() = default;
		StructDeclaration(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<vector<shared_ptr<StructDeclarator>>> vectorStructDeclarator);
		virtual ~StructDeclaration() = default;
		StructDeclaration(const StructDeclaration& other) = default;
		StructDeclaration(StructDeclaration&& other) noexcept = default;
		StructDeclaration& operator=(const StructDeclaration& other) = default;
		StructDeclaration& operator=(StructDeclaration&& other) noexcept = default;
		[[nodiscard]] shared_ptr<SpecifierQualifierList> getSpecifierQualifierList() const { return specifierQualifierList; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<StructDeclarator>>> getVectorStructDeclarator() const { return vectorStructDeclarator; }
		void setSpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList) { this->specifierQualifierList = specifierQualifierList; }
		void setVectorStructDeclarator(shared_ptr<vector<shared_ptr<StructDeclarator>>> vectorStructDeclarator) { this->vectorStructDeclarator = vectorStructDeclarator; }
		bool hasSpecifierQualifierList() const { return specifierQualifierList != nullptr; }
		bool hasVectorStructDeclarator() const { return vectorStructDeclarator != nullptr; }
		void addStructDeclarator(shared_ptr<StructDeclarator> structDeclarator)
		{
			if (vectorStructDeclarator == nullptr)
			{
				vectorStructDeclarator = make_shared<vector<shared_ptr<StructDeclarator>>>();
			}
			vectorStructDeclarator->push_back(structDeclarator);
		}
	};
}
