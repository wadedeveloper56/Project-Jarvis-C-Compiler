#include "pch.h"
#include "Lexer.h"
#include "Parser.h"
#include "ArgumentTable.h"
#include "SemanticAnalyzer.h"
#include "JWasmGenerator.h"

using namespace std;

// defaults
int bits = 32;
bool bit16 = false;
bool bit32 = true; // default to 32-bit
bool bit64 = false;
bool isWindows = true; // default to Windows calling convention for 64-bit
bool isLinux = false;
string src;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	ifstream in;
	ofstream out;
	ArgIntPtr bitsize = argInt0("bB", "bitsize", NULL, "define bit size to be 16, 32 or 64 bits (default is 32)");
	ArgStrPtr platform = argStr0("pP", "platform", NULL, "define platform windows or linux (default is \"windows\")");
	ArgFilePtr outfile = argFile0("Oo", "output", "<file>", "output file (default is \"-\")");
	ArgLitPtr help = argLit0("hH", "help", "print this help and exit");
	ArgLitPtr version = argLit0("vV", "version", "print version information and exit");
	ArgFilePtr infiles = argFileN(NULL, NULL, NULL, 1, argc + 2, "input file(s)");
	ArgEndPtr end = argEnd(20);
	void* argtable[] = { bitsize, platform, outfile, help, version, infiles, end };
	const char* progname = "Compiler";
	int exitcode = 0, nerrors = 0;

	printf("Compiler (x86/x64) v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n");
	printf("All Rights Reserved\n");

	if (argNullCheck(argtable) != 0)
	{
		printf("%s: insufficient memory\n", progname);
		exitcode = 1;
		goto exit;
	}
	bitsize->ival[0] = 32;
	outfile->filename[0] = "-";
	platform->sval[0] = "windows";

	nerrors = argParse(argc, argv, argtable);

	bits = bitsize->ival[0];
	bit16 = bits == 16;
	bit32 = bits == 32;
	bit64 = bits == 64;
	isWindows = (platform->sval[0] == "windows");
	isLinux = (platform->sval[0] == "linux");

	if (help->count > 0)
	{
		printf("Usage: %s", progname);
		argPrintSyntax(stdout, argtable, "\n");
		printf("Compiler that coverts c code to 16,32 or 64 bit assembler\n");
		argPrintGlossary(stdout, argtable, "  %-25s %s\n");
		exitcode = 0;
		goto exit;
	}

	if (version->count > 0)
	{
		printf("'%s' c code to assembler compiler.\n", progname);
		printf("June 2025-2026, (c) Christopher D. Wade\n");
		exitcode = 0;
		goto exit;
	}

	if (nerrors > 0)
	{
		argPrintErrors(stdout, end, progname);
		printf("Try '%s --help' for more information.\n", progname);
		exitcode = 1;
		goto exit;
	}

	if (argc == 1)
	{
		printf("Try '%s --help' for more information.\n", progname);
		exitcode = 0;
		goto exit;
	}

	if (infiles->count == 0)
	{
		printf("No input file specified.\n");
		exitcode = 1;
		goto exit;
	}

	if (outfile->count == 0)
	{
		printf("No output file specified.\n");
		exitcode = 1;
		goto exit;
	}
	src = "int x = 3;\n"
		"int m;\n"
		"int add(int a, int b) {\n"
		"  int c = a + b * 2;\n"
		"  return c;\n"
		"}\n"
		"int main() {\n"
		"  int y = add(x, 4);\n"
		"  return y;\n"
		"}\n";
	try
	{

		Parser p(src);
		auto prog = p.parseProgram();
		SemanticAnalyzer sema;
		bool ok = sema.analyze(*prog);
		if (!ok)
		{
			for (auto& e : sema.errors()) cerr << "Semantic error: " << e << "\n";
			return 1;
		}
		// On success, generate MASM-like asm to stdout using selected bit size and ABI
		JWasmGenerator gen(cout, bits, isWindows);
		gen.generate(*prog);
	}
	catch (const exception& ex)
	{
		cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

exit:
	argFreeTable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	in.close();
	out.close();
	return exitcode;
}