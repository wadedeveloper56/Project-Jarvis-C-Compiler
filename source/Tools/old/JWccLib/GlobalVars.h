#pragma once

#include "Messages.h"

using namespace std;

namespace WadeSpace
{
	class ProgramData;
	class ExternalDeclaration;
	class StructOrUnionSpecifier;
	class Compiler;

	extern shared_ptr<Compiler> compiler;
	extern Messages messages;
	extern bool bit16;
	extern bool bit32;
	extern bool bit64;
	extern char asmFileName[_MAX_PATH];
	
}