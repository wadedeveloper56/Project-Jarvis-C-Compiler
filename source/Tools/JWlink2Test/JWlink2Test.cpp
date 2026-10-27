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
			int argc = 12;
			char* argv[] = { "JWlink2.exe", "format","win","pe","option","map=%CD%\\bin\\windows\\64bit\\test1.map","file","%CD%\\bin\\windows\\64bit\\test1.obj,%CD%\\bin\\windows\\64bit\\c064.obj","lib","%LIB64%,%KLIB64%,%LLIB64%","name","%CD%\\bin\\windows\\64bit\\test1.exe" };
			CommandLine cmdLine(argc, argv);
			cmdLine.ProcessCommandLine();
			auto configMap = cmdLine.GetConfigMap();
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
