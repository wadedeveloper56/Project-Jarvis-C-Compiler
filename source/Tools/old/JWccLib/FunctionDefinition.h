#pragma once

#include <string>
#include <vector>
#include <optional>
#include "TokenType.h"
#include "DeclarationSpecifiers.h"
#include "Declarator.h"
#include "Declaration.h"

using namespace std;

namespace WadeSpace
{
	class BaseStatement;

	class FunctionDefinition
	{
		shared_ptr<DeclarationSpecifiers> declarationSpecifiers;
		shared_ptr<Declarator> declarator;
		shared_ptr<vector<shared_ptr<Declaration>>> vectorDeclaration;
		shared_ptr<BaseStatement> baseStatement;
	public:
		FunctionDefinition(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<Declarator> declarator, shared_ptr<vector<shared_ptr<Declaration>>> vectorDeclaration, shared_ptr<BaseStatement> baseStatement);
		FunctionDefinition(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<Declarator> declarator, shared_ptr<BaseStatement> baseStatement);
		FunctionDefinition(shared_ptr<Declarator> declarator, shared_ptr<vector<shared_ptr<Declaration>>>, shared_ptr<BaseStatement> baseStatement);
		FunctionDefinition(shared_ptr<Declarator> declarator, shared_ptr<BaseStatement> baseStatement);
		FunctionDefinition() = default;
		virtual ~FunctionDefinition() = default;
		FunctionDefinition(const FunctionDefinition& other) = default;
		FunctionDefinition(FunctionDefinition&& other) noexcept = default;
		FunctionDefinition& operator=(const FunctionDefinition& other) = default;
		FunctionDefinition& operator=(FunctionDefinition&& other) noexcept = default;
		[[nodiscard]] shared_ptr<DeclarationSpecifiers> getDeclarationSpecifiers() const { return declarationSpecifiers; }
		[[nodiscard]] shared_ptr<Declarator> getDeclarator() const { return declarator; }
		[[nodiscard]] shared_ptr<vector<shared_ptr<Declaration>>> getVectorDeclaration() const { return vectorDeclaration; }
		[[nodiscard]] shared_ptr<BaseStatement> getBaseStatement() const { return baseStatement; }
		[[nodiscard]] bool hasDeclarationSpecifiers() const { return declarationSpecifiers != nullptr; }
		[[nodiscard]] bool hasDeclarator() const { return declarator != nullptr; }
		[[nodiscard]] bool hasVectorDeclaration() const { return vectorDeclaration != nullptr; }
		[[nodiscard]] bool hasBaseStatement() const { return baseStatement != nullptr; }
		void setDeclarationSpecifiers(shared_ptr<DeclarationSpecifiers> declarationSpecifiers) { this->declarationSpecifiers = declarationSpecifiers; }
		void setDeclarator(shared_ptr<Declarator> declarator) { this->declarator = declarator; }
		void setVectorDeclaration(shared_ptr<vector<shared_ptr<Declaration>>> vectorDeclaration) { this->vectorDeclaration = vectorDeclaration; }
		void setBaseStatement(shared_ptr<BaseStatement> baseStatement) { this->baseStatement = baseStatement; }
		void addDeclaration(shared_ptr<Declaration> declaration)
		{
			if (vectorDeclaration == nullptr)
			{
				vectorDeclaration = make_shared<vector<shared_ptr<Declaration>>>();
			}
			vectorDeclaration->push_back(declaration);
		}
	};
}
