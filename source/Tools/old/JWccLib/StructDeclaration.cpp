#include "pch.h"
#include "StructDeclaration.h"

using namespace WadeSpace;

StructDeclaration::StructDeclaration(shared_ptr<SpecifierQualifierList> specifierQualifierList, shared_ptr<vector<shared_ptr<StructDeclarator>>> vectorStructDeclarator) : specifierQualifierList(specifierQualifierList), vectorStructDeclarator(vectorStructDeclarator)
{
}
