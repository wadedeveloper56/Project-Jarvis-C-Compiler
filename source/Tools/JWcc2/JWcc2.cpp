#include "pch.h"
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"

using namespace std;

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