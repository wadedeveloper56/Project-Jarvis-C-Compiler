#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"

using namespace std;

class ASTPrinter : public ASTVisitor
{
public:
	void visit(ProgramNode* node) override 
	{
		cout << "--- AST PROGRAM START ---\n";
		for (auto& decl : node->externalDeclarations)
		{
			decl->accept(this);
		}
		cout << "--- AST PROGRAM END ---\n";
	}

	void visit(VarDeclNode* node) override 
	{
		cout << (node->isGlobal ? "[Global Var] " : "[Local Var] ")
			<< node->type << " " << node->name;
		if (!node->initValue.empty())
		{
			cout << " = " << node->initValue;
		}
		cout << ";\n";
	}

	void visit(FunctionDeclNode* node) override {
		cout << "[Function] " << node->returnType << " " << node->name << "(";
		for (size_t i = 0; i < node->params.size(); ++i)
		{
			cout << node->params[i].first << " " << node->params[i].second;
			if (i + 1 < node->params.size()) cout << ", ";
		}
		cout << ") {\n";
		for (auto& stmt : node->body)
		{
			cout << "  ";
			stmt->accept(this);
		}
		cout << "}\n";
	}

	void visit(ReturnStmtNode* node) override { cout << "return " << node->value << ";\n"; }
};

int main()
{
	string sourceCode = R"(int global_variable = 42;int standard_var;int main(int argc, int argv) {int local_var = 10;return local_var;})";
	try
	{
		Lexer lexer(sourceCode);
		vector tokens = lexer.tokenize();
		Parser parser(tokens);
		unique_ptr astRoot = parser.parseProgram();
		ASTPrinter printer;
		astRoot->accept(&printer);
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}
	return 0;
}