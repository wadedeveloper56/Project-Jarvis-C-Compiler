#pragma once

#include <vector>
#include "Enumerator.h"
#include "Token.h"

using namespace std;

namespace WadeSpace
{
	class EnumSpecifier
	{
		shared_ptr<CToken>  nameStr;
		shared_ptr<vector<shared_ptr<Enumerator>>> vectorEnumerator;
	public:
		EnumSpecifier() = default;
		EnumSpecifier(shared_ptr<CToken> nameStr, shared_ptr<vector<shared_ptr<Enumerator>>> vectorEnumerator);
		virtual ~EnumSpecifier() = default;
		EnumSpecifier(const EnumSpecifier& other) = default;
		EnumSpecifier(EnumSpecifier&& other) noexcept = default;
		EnumSpecifier& operator=(const EnumSpecifier& other) = default;
		EnumSpecifier& operator=(EnumSpecifier&& other) noexcept = default;
		[[nodiscard]] shared_ptr<CToken> getNameStr() const { return nameStr; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<Enumerator>>> getVectorEnumerator() const { return vectorEnumerator; }
		[[nodiscard]] bool hasNameStr() const { return nameStr != nullptr; }
		[[nodiscard]] bool hasVectorEnumerator() const { return vectorEnumerator != nullptr; }
		void setNameStr(shared_ptr<CToken> nameStr) { this->nameStr = nameStr; }
		void setVectorEnumerator(shared_ptr<vector<shared_ptr<Enumerator>>> vectorEnumerator) { this->vectorEnumerator = vectorEnumerator; }
		void addEnumerator(shared_ptr<Enumerator> enumerator)
		{
			if (vectorEnumerator == nullptr)
			{
				vectorEnumerator = make_shared<vector<shared_ptr<Enumerator>>>();
			}
			vectorEnumerator->push_back(enumerator);
		}
	};
}
