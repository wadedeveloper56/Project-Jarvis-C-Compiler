#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Pointer.h"
#include "DirectDeclarator.h"

using namespace std;

namespace WadeSpace
{
	class Declarator
	{
		shared_ptr<DirectDeclarator> directDeclarator;
		shared_ptr<Pointer> pointer;
	public:
		Declarator() = default;
		Declarator(shared_ptr<Pointer> pointer, shared_ptr<DirectDeclarator> directDeclarator);
		virtual ~Declarator() = default;
		[[nodiscard]] shared_ptr<DirectDeclarator> getDirectDeclarator() const { return directDeclarator; }
		[[nodiscard]] shared_ptr<Pointer> getPointer() const { return pointer; }
		void setDirectDeclarator(shared_ptr<DirectDeclarator> directDeclarator) { this->directDeclarator = directDeclarator; }
		void setPointer(shared_ptr<Pointer> pointer) { this->pointer = pointer; }
		bool hasDirectDeclarator() const { return directDeclarator != nullptr; }
		bool hasPointer() const { return pointer != nullptr; }
	};
}
