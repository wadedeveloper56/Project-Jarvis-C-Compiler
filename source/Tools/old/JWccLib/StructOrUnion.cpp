#include "pch.h"
#include "StructOrUnion.h"
#include <optional>

using namespace WadeSpace; 
using namespace std;
StructOrUnion::StructOrUnion(optional<TokenType> type): type(type)
{
}
