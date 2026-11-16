#include "pch.h"

#include "Compiler.h"
#include "Messages.h"
#include "GlobalVars.h"

using namespace std;

namespace WadeSpace
{
	shared_ptr<Compiler> compiler = nullptr;
	Messages messages;
	bool bit16 = false;
	bool bit32 = false;
	bool bit64 = false;
	char asmFileName[_MAX_PATH];
}

