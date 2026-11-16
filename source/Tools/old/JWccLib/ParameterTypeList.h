#pragma once

#include <string>
#include <vector>
#include <optional>
#include "ParameterDeclaration.h"

using namespace std;

namespace WadeSpace
{
	class ParameterTypeList
	{
		shared_ptr<vector<shared_ptr<ParameterDeclaration>>> vectorParameterDeclaration;
		bool ellipsis;
	public:
		ParameterTypeList() = default;
		ParameterTypeList(shared_ptr<vector<shared_ptr<ParameterDeclaration>>> vectorParameterDeclaration, bool ellipsis);
		virtual ~ParameterTypeList() = default;
		ParameterTypeList(const ParameterTypeList& other) = default;
		ParameterTypeList(ParameterTypeList&& other) noexcept = default;
		ParameterTypeList& operator=(const ParameterTypeList& other) = default;
		ParameterTypeList& operator=(ParameterTypeList&& other) noexcept = default;
		void setVectorParameterDeclaration(shared_ptr<vector<shared_ptr<ParameterDeclaration>>> vectorParameterDeclaration) { this->vectorParameterDeclaration = vectorParameterDeclaration; }
		void setEllipsis(bool ellipsis) { this->ellipsis = ellipsis; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<ParameterDeclaration>>> getVectorParameterDeclaration() const { return vectorParameterDeclaration; }
		[[nodiscard]] bool hasVectorParameterDeclaration() const { return vectorParameterDeclaration != nullptr; }
		[[nodiscard]] bool hasEllipsis() const { return ellipsis; }
		void addParameterDeclaration(shared_ptr<ParameterDeclaration> parameterDeclaration) 
		{
			if (vectorParameterDeclaration == nullptr) vectorParameterDeclaration = make_shared<vector<shared_ptr<ParameterDeclaration>>>();
			vectorParameterDeclaration->push_back(parameterDeclaration); 
		}
	};
}
