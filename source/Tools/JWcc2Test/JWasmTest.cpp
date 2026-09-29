#include "pch.h"
#include "CppUnitTest.h"
#include "../JWcc2/Lexer.h"
#include "../JWcc2/Parser.h"
#include "../JWcc2/JWasmGenerator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JWcc2Test
{
	TEST_CLASS(JWasmBasicTest)
	{
	public:
		TEST_METHOD(JWasmTestBasic16bitWindows)
		{
			ostringstream ss;
			ss << "int main() { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			ostringstream out;
			JWasmGenerator gen(out, 16, true);
			gen.generate(*program);
			auto str = out.str();
			cout << str << endl;
			Assert::IsTrue(str.find(".386") != std::string::npos);
			Assert::IsTrue(str.find("option segment:use16") != std::string::npos);
			Assert::IsTrue(str.find(".model small, c") != std::string::npos);
			Assert::IsTrue(str.find("option casemap : none") != std::string::npos);
			Assert::IsTrue(str.find(".data?") != std::string::npos);
			Assert::IsTrue(str.find(".data") != std::string::npos);
			Assert::IsTrue(str.find(".code") != std::string::npos);
			Assert::IsTrue(str.find("_main PROC") != std::string::npos);
			Assert::IsTrue(str.find("_main ENDP") != std::string::npos);
		}

		TEST_METHOD(JWasmTestBasic32bitWindows)
		{
			ostringstream ss;
			ss << "int main() { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			ostringstream out;
			JWasmGenerator gen(out, 32, true);
			gen.generate(*program);
			auto str = out.str();
			cout << str << endl;
			Assert::IsTrue(str.find(".386") != std::string::npos);
			Assert::IsTrue(str.find("option segment:use32") != std::string::npos);
			Assert::IsTrue(str.find(".model flat, c") != std::string::npos);
			Assert::IsTrue(str.find("option casemap : none") != std::string::npos);
			Assert::IsTrue(str.find(".data?") != std::string::npos);
			Assert::IsTrue(str.find(".data") != std::string::npos);
			Assert::IsTrue(str.find(".code") != std::string::npos);
			Assert::IsTrue(str.find("_main PROC") != std::string::npos);
			Assert::IsTrue(str.find("_main ENDP") != std::string::npos);
		}

		TEST_METHOD(JWasmTestBasic64bitWindows)
		{
			ostringstream ss;
			ss << "int main() { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			ostringstream out;
			JWasmGenerator gen(out, 64, true);
			gen.generate(*program);
			auto str = out.str();
			cout << str << endl;
			Assert::IsTrue(str.find(".x64p") != std::string::npos);
			Assert::IsTrue(str.find("option casemap : none") != std::string::npos);
			Assert::IsTrue(str.find(".data?") != std::string::npos);
			Assert::IsTrue(str.find(".data") != std::string::npos);
			Assert::IsTrue(str.find(".code") != std::string::npos);
			Assert::IsTrue(str.find("_main PROC") != std::string::npos);
			Assert::IsTrue(str.find("_main ENDP") != std::string::npos);
		}
	};
}
