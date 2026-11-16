#include "pch.h"
#include "StructOrUnionSpecifier.h"

using namespace WadeSpace;

StructOrUnionSpecifier::StructOrUnionSpecifier(shared_ptr<CToken> structOrUnion, shared_ptr<CToken> name, shared_ptr<vector<shared_ptr<StructDeclaration>>>vectorStructDeclaration) : 
structOrUnion(structOrUnion), name(name), vectorStructDeclaration(vectorStructDeclaration)
{
}
