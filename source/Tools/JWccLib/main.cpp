#include "pch.h"
#include <fstream>
#include <sstream>
#include "scanner.h"
#include "parser.hpp"
#include "interpreter.h"
#include "ArgumentTable.h"
#include "GlobalVars.h"
//#include "simplecpp.h"
//#include "preprocessor.h"
#include "BaseCodeGenerator.h"
#include "MasmCodeGenerator.h"
#include "output.h"
#include "Compiler.h"

using namespace WadeSpace;
using namespace std;
//using namespace simplecpp;
//using namespace WadeSpace::PreProcessor;

int main(int argc, char* argv[])
{
	ifstream in;
	ofstream out;
	ArgIntPtr bitsize = argInt0("bB", "bitsize", NULL, "define bit size to be 16, 32 or 64 bits (default is 32)");
	ArgFilePtr outfile = argFile0("Oo", "output", "<file>", "output file (default is \"-\")");
	ArgLitPtr help = argLit0("hH", "help", "print this help and exit");
	ArgLitPtr version = argLit0("vV", "version", "print version information and exit");
	ArgFilePtr infiles = argFileN(NULL, NULL, NULL, 1, argc + 2, "input file(s)");
	ArgEndPtr end = argEnd(20);
	void* argtable[] = {bitsize, outfile, help, version, infiles, end};
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

	nerrors = argParse(argc, argv, argtable);

	if (bitsize->ival[0] == 16)
		bit16 = true;
	else if (bitsize->ival[0] == 32)
		bit32 = true;
	else
		bit64 = true;

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

	in.open(infiles->filename[0], ifstream::in);
	out.open(outfile->filename[0], ofstream::out);

	if (!in.is_open())
	{
		cerr << "Failed to open input '" << infiles->filename[0] << "' ";
		int errNum = errno;
		char errMsg[256];

		// Use MSVC-preferred strerror_s instead of strerror
		if (strerror_s(errMsg, sizeof(errMsg), errNum) == 0)
		{
			cerr << "Error Code (" << errNum << "): '" << errMsg << "'" << endl;
		}
		else
		{
			cerr << "Error Code: " << errNum << " (Unknown error description)" << endl;
		}
		exitcode = -1;
	}
	else if (!out.is_open())
	{
		cerr << "Failed to open output '" << outfile->filename[0] << "' ";
		int errNum = errno;
		char errMsg[256];

		if (strerror_s(errMsg, sizeof(errMsg), errNum) == 0)
		{
			cerr << "Error Code (" << errNum << "): '" << errMsg << "'" << endl;
		}
		else
		{
			cerr << "Error Code: " << errNum << " (Unknown error description)" << endl;
		}
		exitcode = -1;
	}
	else
	{
		debug_printf("Parse starting");
		compiler = make_shared<Compiler>();
		compiler->compile(in, out, exitcode);
		if (exitcode == 0)
		{
			cout << "Parse successful" << endl;
		}
		else
		{
			cout << "Parse failed. Result = " << exitcode << endl;
		}
	}

exit:
	argFreeTable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	in.close();
	out.close();
	return exitcode;
}
