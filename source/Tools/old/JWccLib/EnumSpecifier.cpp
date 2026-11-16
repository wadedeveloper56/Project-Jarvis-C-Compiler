#include "pch.h"
#include "EnumSpecifier.h"

#include "Token.h"

using namespace WadeSpace;

EnumSpecifier::EnumSpecifier(shared_ptr<CToken> nameStr, shared_ptr<vector<shared_ptr<Enumerator>>> vectorEnumerator) : nameStr(nameStr),vectorEnumerator(vectorEnumerator)
{
}
