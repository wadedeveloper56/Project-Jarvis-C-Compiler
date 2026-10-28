#include "pch.h"
#include "CppUnitTest.h"
#include "CommandLine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JWlink2Test
{
	TEST_CLASS(JWlink2Test)
	{
	public:
		
		TEST_METHOD(CommandLineTest1)
		{
			int argc = 14;
			char* argv[] = { "JWlink2.exe", "format","win","pe","option","map=C:\\projects\\OSDev\\Project-Jarvis-C-Compiler\\test\\bin\\windows\\64bit\\test1.map","file","C:\\projects\\OSDev\\Project-Jarvis-C-Compiler\\test\\bin\\windows\\64bit\\test1.obj,C:\\projects\\OSDev\\Project-Jarvis-C-Compiler\\test\\bin\\windows\\64bit\\c064.obj","lib","\"C:\\Program Files(x86)\\Windows Kits\\10\\Lib\\10.0.28000.0\\um\\x64\\shell32.lib\"","\"C:\\Program Files(x86)\\Windows Kits\\10\\Lib\\10.0.28000.0\\um\\x64\\kernel32.lib\"","\"C:\\Program Files\\Microsoft Visual Studio\\18\\Insiders\\VC\\Tools\\MSVC\\14.53.36911\\lib\\x64\\msvcrt.lib\"","name","C:\\projects\\OSDev\\Project-Jarvis-C-Compiler\\test\\bin\\windows\\64bit\\test1.exe"};
			CommandLine cmdLine(argc, argv);
			cmdLine.ProcessCommandLine();
			auto& configMap = cmdLine.GetConfigMap();
			Assert::IsTrue(configMap.find("format") != configMap.end());
			Assert::IsTrue(configMap.find("option") != configMap.end());
			Assert::IsTrue(configMap.find("file") != configMap.end());
			Assert::IsTrue(configMap.find("lib") != configMap.end());
			Assert::IsTrue(configMap.find("name") != configMap.end());
			const auto& formatValues = configMap["format"];
			Assert::AreEqual(2, (int)formatValues.size());

			const auto& optionValues = configMap["option"];
			Assert::AreEqual(1, (int)optionValues.size());

			const auto& fileValues = configMap["file"];
			Assert::AreEqual(2, (int)fileValues.size());

			const auto& libValues = configMap["lib"];
			Assert::AreEqual(3, (int)libValues.size());

			const auto& nameValues = configMap["name"];
			Assert::AreEqual(1, (int)nameValues.size());
		}
	};
}
