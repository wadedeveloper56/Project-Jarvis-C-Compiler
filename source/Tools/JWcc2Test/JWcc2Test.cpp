#include "pch.h"
#include "CppUnitTest.h"
#include "../JWcc2/Lexer.h"
#include "../JWcc2/Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JWcc2Test
{
	TEST_CLASS(JWcc2Test)
	{
	public:
		
		TEST_METHOD(ParserTestBasic)
		{
			ostringstream ss;
			ss << "int main() { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			Assert::IsNotNull(program.get());
		}
	};
}
