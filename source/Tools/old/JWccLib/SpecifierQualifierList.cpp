#include "pch.h"
#include "SpecifierQualifierList.h"

using namespace WadeSpace;

SpecifierQualifierList::SpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<TypeSpecifier> typeSpecifier) : specifierQualifierList(specifierQualifierList), typeSpecifier(typeSpecifier), typeQualifier(nullptr)
{
}

SpecifierQualifierList::SpecifierQualifierList(shared_ptr<TypeSpecifier> typeSpecifier) : specifierQualifierList(nullptr), typeSpecifier(typeSpecifier), typeQualifier(nullptr)
{
}

SpecifierQualifierList::SpecifierQualifierList(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<TypeQualifier> typeQualifier) : specifierQualifierList(specifierQualifierList), typeSpecifier(nullptr), typeQualifier(typeQualifier)
{
}

SpecifierQualifierList::SpecifierQualifierList(shared_ptr<TypeQualifier> typeQualifier) : specifierQualifierList(nullptr), typeSpecifier(nullptr), typeQualifier(typeQualifier)
{
}

