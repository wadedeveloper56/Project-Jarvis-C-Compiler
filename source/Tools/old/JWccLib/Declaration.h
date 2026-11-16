#pragma once

#include <string>
#include <vector>
#include <optional>
#include "DeclarationSpecifiers.h"
#include "InitDeclarator.h"

using namespace std;

namespace WadeSpace
{
	class Declaration
	{
		shared_ptr<DeclarationSpecifiers> declarationSpecifiers;
		shared_ptr<vector<shared_ptr<InitDeclarator>>> vectorInitDeclarator;
	public:
		Declaration() = default;
		Declaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers);
		Declaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<vector<shared_ptr<InitDeclarator>>> vectorInitDeclarator);
		virtual ~Declaration() = default;
		[[nodiscard]] shared_ptr<DeclarationSpecifiers> getDeclarationSpecifiers() const { return declarationSpecifiers; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<InitDeclarator>>> getVectorInitDeclarator() const { return vectorInitDeclarator; }
		[[nodiscard]] bool hasDeclarationSpecifiers() const { return declarationSpecifiers != nullptr; }
		[[nodiscard]] bool hasVectorInitDeclarator() const { return vectorInitDeclarator != nullptr; }
		void setDeclarationSpecifiers(shared_ptr<DeclarationSpecifiers> declarationSpecifiers) { this->declarationSpecifiers = declarationSpecifiers; }
		void setVectorInitDeclarator(shared_ptr<vector<shared_ptr<InitDeclarator>>> vectorInitDeclarator) { this->vectorInitDeclarator = vectorInitDeclarator; }
		void addInitDeclarator(shared_ptr<InitDeclarator> initDeclarator) { 
			if (vectorInitDeclarator == nullptr) 
			{
				vectorInitDeclarator = make_shared<vector<shared_ptr<InitDeclarator>>>();
			}
			vectorInitDeclarator->push_back(initDeclarator); 
		}
	};
}
