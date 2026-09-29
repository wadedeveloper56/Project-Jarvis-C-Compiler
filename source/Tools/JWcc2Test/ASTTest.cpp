#include "pch.h"
#include "CppUnitTest.h"
#include "../JWcc2/Lexer.h"
#include "../JWcc2/Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JWcc2Test
{
	TEST_CLASS(ASTBasicTest)
	{
	public:
		
		TEST_METHOD(ParserTestBasic1Method0Globals)
		{
			ostringstream ss;
			ss << "int main() { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			Assert::IsNotNull(program.get());
			Assert::AreEqual(1, (int)program->declarations.size());
			auto decl = dynamic_cast<FunctionDeclaration*>(program->declarations[0].get());
			Assert::IsNotNull(decl);
			Assert::AreEqual(string("int"), decl->retType);
			Assert::AreEqual(string("main"), decl->name);
			Assert::AreEqual(0, (int)decl->params.size());
			Assert::IsNotNull(decl->body.get());
			auto body = decl->body.get();
			Assert::IsNotNull(body);
			Assert::AreEqual(0, (int)body->localDeclarations.size());
			Assert::AreEqual(1, (int)body->statements.size());
			auto ret = dynamic_cast<ReturnStatement*>(body->statements[0].get());
			Assert::IsNotNull(ret);
			auto numExpr = dynamic_cast<NumberExpression*>(ret->expr.get());
			Assert::IsNotNull(numExpr);
			Assert::AreEqual(0, (int)numExpr->value);
		}

		TEST_METHOD(ParserTestBasic1Method1Globals)
		{
			ostringstream ss;
			ss << "int global1; int main() { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			Assert::IsNotNull(program.get());
			Assert::AreEqual(2, (int)program->declarations.size());

			auto decl0 = dynamic_cast<VariableDeclaration*>(program->declarations[0].get());
			Assert::IsNotNull(decl0);
			Assert::AreEqual(string("int"), decl0->type);
			Assert::AreEqual(string("global1"), decl0->name);

			auto decl1 = dynamic_cast<FunctionDeclaration*>(program->declarations[1].get());
			Assert::IsNotNull(decl1);
			Assert::AreEqual(string("int"), decl1->retType);
			Assert::AreEqual(string("main"), decl1->name);
			Assert::AreEqual(0, (int)decl1->params.size());

			Assert::IsNotNull(decl1->body.get());
			auto body = decl1->body.get();
			Assert::IsNotNull(body);
			Assert::AreEqual(0, (int)body->localDeclarations.size());
			Assert::AreEqual(1, (int)body->statements.size());
			
			auto ret = dynamic_cast<ReturnStatement*>(body->statements[0].get());
			Assert::IsNotNull(ret);
			
			auto numExpr = dynamic_cast<NumberExpression*>(ret->expr.get());
			Assert::IsNotNull(numExpr);
			Assert::AreEqual(0, (int)numExpr->value);
		}

		TEST_METHOD(ParserTestBasic1Method1Parameter0Globals)
		{
			ostringstream ss;
			ss << "int main(int argc) { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			Assert::IsNotNull(program.get());
			Assert::AreEqual(1, (int)program->declarations.size());
			auto decl = dynamic_cast<FunctionDeclaration*>(program->declarations[0].get());
			Assert::IsNotNull(decl);
			Assert::AreEqual(string("int"), decl->retType);
			Assert::AreEqual(string("main"), decl->name);

			auto param = decl->params;
			Assert::AreEqual(1, (int)param.size());
			auto pair = param[0];
			Assert::AreEqual(string("int"), pair.first);
			Assert::AreEqual(string("argc"), pair.second);

			Assert::IsNotNull(decl->body.get());
			auto body = decl->body.get();
			Assert::IsNotNull(body);
			Assert::AreEqual(0, (int)body->localDeclarations.size());
			Assert::AreEqual(1, (int)body->statements.size());

			auto ret = dynamic_cast<ReturnStatement*>(body->statements[0].get());
			Assert::IsNotNull(ret);

			auto numExpr = dynamic_cast<NumberExpression*>(ret->expr.get());
			Assert::IsNotNull(numExpr);
			Assert::AreEqual(0, (int)numExpr->value);
		}

		TEST_METHOD(ParserTestBasic1Method1Parameter1Globals)
		{
			ostringstream ss;
			ss << "unsigned int global1; int main(int argc) { return 0; }";
			Parser parser(ss.str());
			auto program = parser.parseProgram();
			Assert::IsNotNull(program.get());
			Assert::AreEqual(2, (int)program->declarations.size());

			auto decl1 = dynamic_cast<VariableDeclaration*>(program->declarations[0].get());
			Assert::IsNotNull(decl1);
			Assert::AreEqual(string("unsigned int"), decl1->type);
			Assert::AreEqual(string("global1"), decl1->name);

			auto decl2 = dynamic_cast<FunctionDeclaration*>(program->declarations[1].get());
			Assert::IsNotNull(decl2);
			Assert::AreEqual(string("int"), decl2->retType);
			Assert::AreEqual(string("main"), decl2->name);

			auto param = decl2->params;
			Assert::AreEqual(1, (int)param.size());
			auto pair = param[0];
			Assert::AreEqual(string("int"), pair.first);
			Assert::AreEqual(string("argc"), pair.second);

			Assert::IsNotNull(decl2->body.get());
			auto body = decl2->body.get();
			Assert::IsNotNull(body);
			Assert::AreEqual(0, (int)body->localDeclarations.size());
			Assert::AreEqual(1, (int)body->statements.size());

			auto ret = dynamic_cast<ReturnStatement*>(body->statements[0].get());
			Assert::IsNotNull(ret);

			auto numExpr = dynamic_cast<NumberExpression*>(ret->expr.get());
			Assert::IsNotNull(numExpr);
			Assert::AreEqual(0, (int)numExpr->value);
		}
	};
}
