// JWcc2Lib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "..\JWcc2\types.h"
#include <string>

using namespace std;
// defaults
int bits = 32;
bool bit16 = false;
bool bit32 = true; // default to 32-bit
bool bit64 = false;
bool isWindows = true; // default to Windows calling convention for 64-bit
bool isLinux = false;
std::string processor = "386"; // default to 16/32-bit 80386
ProcessorBitType processorBitType = ProcessorBitType::BIT32_386;

// TODO: This is an example of a library function
void fnJWcc2Lib()
{
}
