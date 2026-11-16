#pragma once

#include <vector>
#include "StructDeclaration.h"

using namespace std;

namespace WadeSpace
{
	class StructOrUnionSpecifier
	{
		shared_ptr<CToken> name;
		shared_ptr<CToken> structOrUnion;
		shared_ptr<vector<shared_ptr<StructDeclaration>>>vectorStructDeclaration;
	public:
		StructOrUnionSpecifier() = default;
		StructOrUnionSpecifier(shared_ptr<CToken> structOrUnion, shared_ptr<CToken> name, shared_ptr<vector<shared_ptr<StructDeclaration>>>vectorStructDeclaration);
		virtual ~StructOrUnionSpecifier() = default;
		StructOrUnionSpecifier(const StructOrUnionSpecifier& other) = default;
		StructOrUnionSpecifier(StructOrUnionSpecifier&& other) noexcept = default;
		StructOrUnionSpecifier& operator=(const StructOrUnionSpecifier& other) = default;
		StructOrUnionSpecifier& operator=(StructOrUnionSpecifier&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getName() const { return name; }
		[[nodiscard]] shared_ptr<CToken> getStructOrUnion() const { return structOrUnion; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<StructDeclaration>>>getVectorStructDeclaration() const { return vectorStructDeclaration; }
		[[nodiscard]] bool hasName() const { return name != nullptr; }
		[[nodiscard]] bool hasStructOrUnion() const { return structOrUnion != nullptr; }
		[[nodiscard]] bool hasVectorStructDeclaration() const { return vectorStructDeclaration != nullptr; }
		void setName(shared_ptr<CToken> name) { this->name = name; }
		void setStructOrUnion(shared_ptr<CToken> structOrUnion) { this->structOrUnion = structOrUnion; }
		void setVectorStructDeclaration(shared_ptr<vector<shared_ptr<StructDeclaration>>>vectorStructDeclaration) { this->vectorStructDeclaration = vectorStructDeclaration; }
		void addStructDeclaration(shared_ptr<StructDeclaration> structDeclaration)
		{
			if (vectorStructDeclaration == nullptr)
			{
				vectorStructDeclaration = make_shared<vector<shared_ptr<StructDeclaration>>>();
			}
			vectorStructDeclaration->push_back(structDeclaration);
		}
	};
}
