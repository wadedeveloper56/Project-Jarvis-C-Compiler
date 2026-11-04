#pragma once
#define PE_DEFAULT_BASE (0x400000UL)
#define PE_DEFAULT_BASE_DLL (0x10000000UL) /* jwlink */
#define PE_DEF_STACK_COMMIT (0xFFFFFFFFUL)
void DoAddResource(char* name);
