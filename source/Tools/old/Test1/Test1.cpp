// Test1.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "Test1.h"


// This is an example of an exported variable
TEST1_API int nTest1=0;

// This is an example of an exported function.
TEST1_API int fnTest1(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CTest1::CTest1()
{
    return;
}
