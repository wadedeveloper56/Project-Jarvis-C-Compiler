#pragma once

typedef enum class ProcessorBitType
{
	BIT16_8086, 
	BIT16_186, 
	BIT16_286, 
	
	BIT16_386,
	BIT32_386,
	
	BIT16_486, 
	BIT32_486,
	
	BIT16_586, 
	BIT32_586,

	BIT16_686,
	BIT32_686,

	BIT64_x64
} ProcessorBitType;

extern int bits;
extern bool bit16;
extern bool bit32;
extern bool bit64;
extern bool isWindows;
extern bool isLinux;
extern string processor;
extern ProcessorBitType processorBitType;
