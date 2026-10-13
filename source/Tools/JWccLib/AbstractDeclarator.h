#pragma once

#include "Pointer.h"
#include "DirectAbstractDeclarator.h"

namespace WadeSpace
{
	class AbstractDeclarator
	{ 
		shared_ptr<Pointer> pointer;
		shared_ptr<DirectAbstractDeclarator> directAbstractDeclarator;
	public:
		AbstractDeclarator() = default;
		AbstractDeclarator(shared_ptr<Pointer> pointer, shared_ptr<DirectAbstractDeclarator> directAbstractDeclarator);
		virtual ~AbstractDeclarator() = default;
		AbstractDeclarator(const AbstractDeclarator& other) = default;
		AbstractDeclarator(AbstractDeclarator&& other) noexcept = default;
		AbstractDeclarator& operator=(const AbstractDeclarator& other) = default;
		AbstractDeclarator& operator=(AbstractDeclarator&& other) noexcept = default;
		[[nodiscard]] shared_ptr<Pointer> getPointer() const;
		[[nodiscard]] shared_ptr<DirectAbstractDeclarator> getDirectAbstractDeclarator() const;
		void setPointer(shared_ptr<Pointer> pointer);
		void setDirectAbstractDeclarator(shared_ptr<DirectAbstractDeclarator> directAbstractDeclarator);
		[[nodiscard]] bool hasPointer() const;
		[[nodiscard]] bool hasDirectAbstractDeclarator() const;
	};
}
