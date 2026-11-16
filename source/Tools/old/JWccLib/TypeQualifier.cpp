#include "pch.h"
#include "TypeQualifier.h"

#include "Token.h"

using namespace WadeSpace;
using namespace std;

TypeQualifier::TypeQualifier(shared_ptr<CToken> type) :type(type)
{
}
