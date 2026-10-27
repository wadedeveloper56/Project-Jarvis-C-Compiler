#include "pch.h"
#include "CppUnitTest.h"
#include "CommandLine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JWlink2Test
{
	TEST_CLASS(JWlink2Test)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			int argc = 3;
			char* argv[] = { "JWlink2.exe", "-o", "output.obj" };
			CommandLine cmdLine(argc, argv);
		}
	};
}
