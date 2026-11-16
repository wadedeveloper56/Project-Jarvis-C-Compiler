#pragma once

#include <string>
#include <vector>
#include <optional>

#include "Token.h"
#include "TypeQualifier.h"

using namespace std;

namespace WadeSpace
{
	class Pointer
	{
		shared_ptr<CToken> name;
		shared_ptr<vector<shared_ptr<TypeQualifier>>> typeQualifierList;
		int level;
		shared_ptr<Pointer> pointer;
	public:
		Pointer() = default;
		Pointer(const shared_ptr<CToken> name, shared_ptr<vector<shared_ptr<TypeQualifier>>> typeQualifierList);
		Pointer(const shared_ptr<CToken> name, shared_ptr<vector<shared_ptr<TypeQualifier>>> typeQualifierList, shared_ptr<Pointer> pointer);
		Pointer(const shared_ptr<CToken> name, shared_ptr<Pointer> pointer);
		Pointer(const shared_ptr<CToken> name);
		virtual ~Pointer() = default;
		Pointer(const Pointer& other) = default;
		Pointer(Pointer&& other) noexcept = default;
		Pointer& operator=(const Pointer& other) = default;
		Pointer& operator=(Pointer&& other) noexcept = default;
		void inc();
		[[nodiscard]] shared_ptr<CToken> getName() const { return name; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<TypeQualifier>>> getTypeQualifierList() const { return typeQualifierList; }
		[[nodiscard]] int getLevel() const { return level; }
		[[nodiscard]] shared_ptr<Pointer> getPointer() const { return pointer; }
		void setName(const shared_ptr<CToken> name) { this->name = name; }
		void setTypeQualifierList(shared_ptr<vector<shared_ptr<TypeQualifier>>> typeQualifierList) { this->typeQualifierList = typeQualifierList; }
		void setLevel(int level) { this->level = level; }
		void setPointer(shared_ptr<Pointer> pointer) { this->pointer = pointer; }
		[[nodiscard]] bool hasName() const { return name != nullptr; }
		[[nodiscard]] bool hasTypeQualifierList() const { return typeQualifierList != nullptr; }
		[[nodiscard]] bool hasPointer() const { return pointer != nullptr; }
		void addTypeQualifier(shared_ptr<TypeQualifier> typeQualifier)
		{
			if (typeQualifierList == nullptr)
			{
				typeQualifierList = make_shared<vector<shared_ptr<TypeQualifier>>>();
			}
			typeQualifierList->push_back(typeQualifier);
		}
	};
}
