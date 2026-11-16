#include "pch.h"
#include "TypeName.h"

using namespace WadeSpace;

TypeName::TypeName(shared_ptr<SpecifierQualifierList> specifierQualifierList) : specifierQualifierList(specifierQualifierList), abstractDeclarator(nullptr)
{
}

TypeName::TypeName(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<AbstractDeclarator> abstractDeclarator) : specifierQualifierList(specifierQualifierList), abstractDeclarator(abstractDeclarator)
{
}
