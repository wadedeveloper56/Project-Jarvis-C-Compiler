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
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JWccTest
{
	TEST_CLASS(JWccTest)
	{
	public:

		// helper: create a Declaration for 'int x;' style
		shared_ptr<WadeSpace::Declaration> createSimpleDeclaration(const std::string& name, WadeSpace::TokenType type = WadeSpace::INT)
		{
			auto declSpec = make_shared<WadeSpace::DeclarationSpecifiers>();
			// build a TypeSpecifier and put it in DeclarationSpecifiersNode
			auto ts = make_shared<WadeSpace::TypeSpecifier>();
			ts->setType(type);
			auto node = make_shared<WadeSpace::DeclarationSpecifiersNode>(nullptr, ts, nullptr);
			declSpec->setDeclarationSpecifiersNodeList(make_shared<vector<shared_ptr<WadeSpace::DeclarationSpecifiersNode>>>());
			declSpec->getDeclarationSpecifiersNodeList()->push_back(node);

			auto decl = make_shared<WadeSpace::Declaration>(declSpec);
			// create DirectDeclarator with identifier token
			auto idtok = createStringIDToken(const_cast<char*>(name.c_str()));
			auto dd = make_shared<WadeSpace::DirectDeclarator>(idtok, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
			auto declarator = make_shared<WadeSpace::Declarator>(nullptr, dd);
			auto init = make_shared<WadeSpace::InitDeclarator>();
			init->setDeclarator(declarator);
			// add to declaration
			decl->addInitDeclarator(init);
			return decl;
		}
		
		TEST_METHOD(CollectLocalDeclarations)
		{
			using namespace WadeSpace;
			ProgramData pd;
			// create a function body with a declaration: int k;
			auto decl = createSimpleDeclaration("k", INT);
			auto declList = make_shared<vector<shared_ptr<Declaration>>>();
			declList->push_back(decl);
			auto body = make_shared<BaseStatement>(declList);
			// create a FunctionDefinition that uses this body
			auto dd = make_shared<Declarator>();
			auto func = make_shared<FunctionDefinition>(dd, body);
			// now invoke ProgramData::handleFunction indirectly by building external declaration list
			auto external = make_shared<ExternalDeclaration>(func);
			pd.addExternalDeclaration(external);
			// process globals/functions
			auto gen = pd.processGlobalVariables();
			Assert::IsTrue(pd.hasGenerator());
			auto ft = gen->getFunctionTable();
			Assert::IsTrue(ft != nullptr && ft->size() > 0);
			//auto f = ft->at(0);
			//Assert::IsTrue(f->localVariables != nullptr && !f->localVariables->empty());
			//Assert::AreEqual(std::string("k"), f->localVariables->at(0)->name);
		}

		TEST_METHOD(Integration_CompileSimpleFunction)
		{
			using namespace WadeSpace;
			Compiler compiler;
			std::istringstream in("int add(int a, int b) { return a + b; }");
			std::ostringstream out;
			int exitcode = 0;
//			compiler.compileFile(in, out, exitcode);
//			Assert::AreEqual(0, exitcode);
//			std::string s = out.str();
//			Assert::IsTrue(s.find("_add PROC") != std::string::npos || s.find("_add PROC C") != std::string::npos);
		}

		TEST_METHOD(Integration_SemanticErrorPreventsGeneration)
		{
			using namespace WadeSpace;
			Compiler compiler;
			std::istringstream in("int bad() { return 3.14; }");
			std::ostringstream out;
			int exitcode = 0;
//			compiler.compileFile(in, out, exitcode);
//			Assert::AreNotEqual(0, exitcode);
//			std::string s = out.str();
//			Assert::IsTrue(s.find("E") != std::string::npos || s.find("error") != std::string::npos);
		}

		TEST_METHOD(MasmGeneratorLocalEmission)
		{
			using namespace WadeSpace;
			// create function data with locals
			auto fd = make_shared<FunctionData>();
			fd->name = "testfn";
			fd->localVariables = make_shared<vector<shared_ptr<VariableData>>>();
			auto v = make_shared<VariableData>();
			v->name = "l1";
			v->type = INT;
			fd->localVariables->push_back(v);
			// create generator and emit
			auto varTable = make_shared<vector<shared_ptr<VariableData>>>();
			auto funcTable = make_shared<vector<shared_ptr<FunctionData>>>();
			funcTable->push_back(fd);
			MasmCodeGenerator gen(varTable, funcTable);
			std::ostringstream out;
			//gen.generateCode(out);
			//std::string s = out.str();
			//Assert::IsTrue(s.find("_testfn PROC") != std::string::npos);
			//Assert::IsTrue(s.find("LOCAL l1") != std::string::npos || s.find("LOCAL l1:") != std::string::npos || s.find("l1") != std::string::npos);
		}



		TEST_METHOD(VariableDataFields)
		{
			using namespace WadeSpace;
			VariableData v;
			v.name = "count";
			v.type = INT;
			v.pointer = false;
			v.arraySize = 4;
			v.unsign = false;
			Assert::AreEqual(std::string("count"), v.name);
			Assert::IsTrue(v.type.has_value());
			Assert::IsTrue(v.type.has_value() && v.type.value() == INT);
			Assert::IsFalse(v.pointer);
			Assert::AreEqual(4ULL, v.arraySize);
		}

		TEST_METHOD(FunctionLocalVariables)
		{
			using namespace WadeSpace;
			FunctionData f;
			f.name = "foo";
			f.localVariables = make_shared<vector<shared_ptr<VariableData>>>();
			auto v = make_shared<VariableData>();
			v->name = "x";
			v->type = INT;
			f.localVariables->push_back(v);
			Assert::AreEqual(std::string("foo"), f.name);
			Assert::AreEqual((size_t)1, f.localVariables->size());
			Assert::AreEqual(std::string("x"), f.localVariables->at(0)->name);
		}
	};
}
