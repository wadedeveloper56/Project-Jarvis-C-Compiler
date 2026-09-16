#include "pch.h"
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "ArgumentTable.h"

using namespace std;
bool bit16 = false;
bool bit32 = false;
bool bit64 = false;
int main(int argc, char* argv[])
{
	string src;
	
	printf("Compiler (x86/x64) v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
	printf("All Rights Reserved\n");

	try
	{
	  src = "int x = 3;\n"
			"int add(int a, int b) {\n"
			"  int c = a + b * 2;\n"
			"  return c;\n"
			"}\n"
			"int main() {\n"
			"  int y = add(x, 4);\n"
			"  return y;\n"
			"}\n";

		Parser p(src);
		auto prog = p.parseProgram();
		ASTPrinter printer;
		prog->accept(printer);
	}
	catch (const exception& ex)
	{
		cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
	return 0;
}