#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void* AllocateMemory(size_t size);
void FreeMemory(void* memoryBlock);
void* ReallocateMemory(void* memblock, size_t size);
void* ExpandMemory(void* memblock, size_t size);

#ifdef __cplusplus
} // extern "C"
#endif