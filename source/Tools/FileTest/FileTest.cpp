#include "pch.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include "../File/File.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace FileTest
{
	TEST_CLASS(FileTest)
	{
	public:

		TEST_METHOD(OpenFile2_NotNull)
		{
			f_handle fh = OpenFile2("test.txt", _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE);
			Assert::AreNotEqual((f_handle) - 1, fh);
			CloseFile2(fh);
			DeleteFileA("test.txt");
		}
	};
}
