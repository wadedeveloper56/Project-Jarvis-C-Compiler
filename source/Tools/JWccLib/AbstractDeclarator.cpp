#include "pch.h"
#include "AbstractDeclarator.h"

using namespace WadeSpace;

AbstractDeclarator::AbstractDeclarator(shared_ptr<Pointer> pointer, shared_ptr<DirectAbstractDeclarator> directAbstractDeclarator)
{
	this->pointer = pointer;
	this->directAbstractDeclarator = directAbstractDeclarator;
}

shared_ptr<Pointer> AbstractDeclarator::getPointer() const { return pointer; }
shared_ptr<DirectAbstractDeclarator> AbstractDeclarator::getDirectAbstractDeclarator() const { return directAbstractDeclarator; }
void AbstractDeclarator::setPointer(shared_ptr<Pointer> pointer) { this->pointer = pointer; }
void AbstractDeclarator::setDirectAbstractDeclarator(shared_ptr<DirectAbstractDeclarator> directAbstractDeclarator) { this->directAbstractDeclarator = directAbstractDeclarator; }
bool AbstractDeclarator::hasPointer() const { return pointer != nullptr; }
bool AbstractDeclarator::hasDirectAbstractDeclarator() const { return directAbstractDeclarator != nullptr; }
