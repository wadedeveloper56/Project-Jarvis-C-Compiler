#include "pch.h"
#include "TypeSpecifier.h"
#include "GlobalVars.h"
#include "StructOrUnionSpecifier.h"
#include "ExternalDeclaration.h"
#include "Compiler.h"

using namespace WadeSpace;

TypeSpecifier::TypeSpecifier(TokenType type, shared_ptr<CToken> token, shared_ptr<StructOrUnionSpecifier> structOrUnion, shared_ptr<EnumSpecifier> enumSpec, shared_ptr<ExternalDeclaration> extDecl)
	: type(type), enumSpec(enumSpec), structOrUnionSpecifier(structOrUnion), typePtr(token), typedefInfo(extDecl)
{
	if (structOrUnion != nullptr)
	{
		auto struct_or_union = structOrUnion->getStructOrUnion();
		this->setType(struct_or_union->getKeywordName() == "struct" ? STRUCT : UNION);
	}
}
