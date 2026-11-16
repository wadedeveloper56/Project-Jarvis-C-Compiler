#pragma once

#include <string>
#include "Declarator.h"
#include "Initializer.h"

namespace WadeSpace
{
	class InitDeclarator
	{
		shared_ptr<Declarator> declarator;
		shared_ptr<Initializer> initializer;
	public:
		InitDeclarator() = default;
		InitDeclarator(shared_ptr<Declarator> declarator);
		InitDeclarator(shared_ptr<Declarator> declarator, shared_ptr<Initializer> initializer);
		virtual ~InitDeclarator() = default;
		InitDeclarator(const InitDeclarator& other) = default;
		InitDeclarator(InitDeclarator&& other) noexcept = default;
		InitDeclarator& operator=(const InitDeclarator& other) = default;
		InitDeclarator& operator=(InitDeclarator&& other) noexcept = default;
		[[nodiscard]] string getVariableName() const;
		[[nodiscard]] shared_ptr<Declarator> getDeclarator() const { return declarator; }
		[[nodiscard]] shared_ptr<Initializer> getInitializer() const { return initializer; }
		[[nodiscard]] bool hasDeclarator() const { return declarator != nullptr; }
		[[nodiscard]] bool hasInitializer() const { return initializer != nullptr; }
		void setDeclarator(shared_ptr<Declarator> const declarator) { this->declarator = declarator; }
		void setInitializer(shared_ptr<Initializer> const initializer) { this->initializer = initializer; }
	};
}
