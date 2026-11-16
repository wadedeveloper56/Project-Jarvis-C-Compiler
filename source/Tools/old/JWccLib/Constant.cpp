#include "pch.h"
#include "Constant.h"

using namespace WadeSpace;
using namespace std;

Constant::Constant(const shared_ptr<CToken> iConst, const shared_ptr<CToken> fConst, const shared_ptr<CToken> strConst, const optional<TokenType>& type) 
	: iConst(iConst),
	fConst(fConst),
	strConst(strConst),
	type(type)
{
}
