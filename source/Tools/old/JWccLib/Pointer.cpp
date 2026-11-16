#include "pch.h"
#include "Pointer.h"

using namespace WadeSpace;

Pointer::Pointer(const shared_ptr<CToken> name, shared_ptr<vector<shared_ptr<TypeQualifier>>> typeQualifierList) : name(name), typeQualifierList(typeQualifierList), level(1), pointer(nullptr)
{
}

Pointer::Pointer(const shared_ptr<CToken> name, shared_ptr<vector<shared_ptr<TypeQualifier>>> typeQualifierList, shared_ptr<Pointer> pointer) : name(name), typeQualifierList(typeQualifierList), level(1), pointer(pointer)
{
}

Pointer::Pointer(const shared_ptr<CToken> name, shared_ptr<Pointer> pointer) : name(name), typeQualifierList(nullptr), level(1), pointer(pointer)
{
}

Pointer::Pointer(const shared_ptr<CToken> name) : name(name), typeQualifierList(nullptr), level(1), pointer(nullptr)
{
}

void Pointer::inc() { level++; }

