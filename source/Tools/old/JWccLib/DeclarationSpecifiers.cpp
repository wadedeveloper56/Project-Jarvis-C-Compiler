#include "pch.h"
#include "DeclarationSpecifiers.h"

using namespace WadeSpace;
using namespace std;

DeclarationSpecifiersNode::DeclarationSpecifiersNode(shared_ptr<StorageClassSpecifier> storageClassSpecifier, shared_ptr<TypeSpecifier> typeSpecifier, shared_ptr<TypeQualifier> const typeQualifier)
	: storageClassSpecifier(storageClassSpecifier), typeSpecifier(typeSpecifier), typeQualifier(typeQualifier) 
{
}

void DeclarationSpecifiers::addDeclarationSpecifiersNode(shared_ptr<StorageClassSpecifier> storageClassSpecifier, shared_ptr<TypeSpecifier> typeSpecifier, shared_ptr<TypeQualifier> typeQualifier)
{
	if (declarationSpecifiersNodeList == nullptr)
	{
		declarationSpecifiersNodeList = make_shared<vector<shared_ptr<DeclarationSpecifiersNode>>>();
	}
	declarationSpecifiersNodeList->push_back(make_shared<DeclarationSpecifiersNode>(storageClassSpecifier, typeSpecifier, typeQualifier));
}
