#include "pch.h"
#include "framework.h"
#include "Memory.h"

void* AllocateMemory(size_t size)
{
	return calloc(1, size);
}

void FreeMemory(void* memoryBlock)
{
	free(memoryBlock);
}

void* ReallocateMemory(void* memblock, size_t size)
{
	return _recalloc(memblock, 1, size);
}

void* ExpandMemory(void* memblock, size_t size)
{
	return _expand(memblock, size);
}