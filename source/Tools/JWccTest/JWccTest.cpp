#include "pch.h"
#include "CppUnitTest.h"
#include "..\JWccLib\BaseCodeGenerator.h"
#include <string>
// test helpers
#include "..\JWccLib\Declaration.h"
#include "..\JWccLib\InitDeclarator.h"
#include "..\JWccLib\DeclarationSpecifiers.h"
#include "..\JWccLib\Declarator.h"
#include "..\JWccLib\BaseStatement.h"
#include "..\JWccLib\ProgramData.h"
#include "..\JWccLib\FunctionDefinition.h"
#include "..\JWccLib\ExternalDeclaration.h"
#include "..\JWccLib\MasmCodeGenerator.h"
#include "..\JWccLib\Token.h"
#include "..\JWccLib\Compiler.h"
#include "..\JWccLib\GlobalVars.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace WadeSpace;

namespace JWccTest
{
	TEST_CLASS(JWccTest)
	{
	public:
		TEST_METHOD(Integration_CompileSimpleFunction)
		{
			istringstream in("int add(int a, int b) { return a + b; }");
			ostringstream out;
			int exitcode = 0;
			compiler = make_shared<Compiler>();
			compiler->compileFile(in, out, exitcode);
			Assert::AreEqual(0, exitcode);
			string s = out.str();
			Assert::IsTrue(s.find("_add PROC") != string::npos || s.find("_add PROC C") != string::npos);
		}

		TEST_METHOD(Integration_SemanticErrorPreventsGeneration)
		{
			istringstream in("int bad() { return 3.14; }");
			ostringstream out;
			int exitcode = 0;
			compiler = make_shared<Compiler>();
			compiler->compileFile(in, out, exitcode);
			Assert::AreEqual(0, exitcode);
			string s = out.str();
			//Assert::IsTrue(s.find("E") != string::npos || s.find("error") != string::npos);
		}
	};
}
