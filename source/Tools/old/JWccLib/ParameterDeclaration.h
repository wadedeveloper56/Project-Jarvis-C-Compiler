#pragma once

#include <string>
#include <vector>
#include <optional>
#include "DeclarationSpecifiers.h"
#include "Declarator.h"
#include "AbstractDeclarator.h"

namespace WadeSpace
{
	class ParameterDeclaration
	{
		shared_ptr<DeclarationSpecifiers> declarationSpecifiers;
		shared_ptr<Declarator> declarator;
		shared_ptr<AbstractDeclarator> abstractDeclarator;
	public:
		ParameterDeclaration() = default;
		ParameterDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers);
		ParameterDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<AbstractDeclarator> abstractDeclarator);
		ParameterDeclaration(shared_ptr<DeclarationSpecifiers> declarationSpecifiers, shared_ptr<Declarator> declarator);
		virtual ~ParameterDeclaration() = default;
		ParameterDeclaration(const ParameterDeclaration& other) = default;
		ParameterDeclaration(ParameterDeclaration&& other) noexcept = default;
		ParameterDeclaration& operator=(const ParameterDeclaration& other) = default;
		ParameterDeclaration& operator=(ParameterDeclaration&& other) noexcept = default;
		[[nodiscard]] shared_ptr<DeclarationSpecifiers> getDeclarationSpecifiers() const { return declarationSpecifiers; }
		[[nodiscard]] shared_ptr<Declarator> getDeclarator() const { return declarator; }
		[[nodiscard]] shared_ptr<AbstractDeclarator> getAbstractDeclarator() const { return abstractDeclarator; }
		void setDeclarationSpecifiers(shared_ptr<DeclarationSpecifiers> declarationSpecifiers) { this->declarationSpecifiers = declarationSpecifiers; }
		void setDeclarator(shared_ptr<Declarator> declarator) { this->declarator = declarator; }
		void setAbstractDeclarator(shared_ptr<AbstractDeclarator> abstractDeclarator) { this->abstractDeclarator = abstractDeclarator; }
		[[nodiscard]] bool hasDeclarationSpecifiers() const { return declarationSpecifiers != nullptr; }
		[[nodiscard]] bool hasDeclarator() const { return declarator != nullptr; }
		[[nodiscard]] bool hasAbstractDeclarator() const { return abstractDeclarator != nullptr; }
	};
}
