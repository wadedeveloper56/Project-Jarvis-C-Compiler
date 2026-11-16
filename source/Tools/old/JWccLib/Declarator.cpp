#include "pch.h"
#include "Declarator.h"
#include "GlobalVars.h"

using namespace WadeSpace;
using namespace std;

Declarator::Declarator(shared_ptr<Pointer> pointer, shared_ptr<DirectDeclarator> directDeclarator) : directDeclarator(directDeclarator), pointer(pointer)
{
}
