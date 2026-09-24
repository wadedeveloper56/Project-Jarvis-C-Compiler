#include "pch.h"
#include "Lexer.h"
#include "Parser.h"
#include "ArgumentTable.h"
#include "SemanticAnalyzer.h"
#include "JWasmGenerator.h"
#include "ASTPrinter.h"
#include "types.h"

using namespace std;


// defaults
int bits = 32;
bool bit16 = false;
bool bit32 = true; // default to 32-bit
bool bit64 = false;
bool isWindows = true; // default to Windows calling convention for 64-bit
bool isLinux = false;
string processor = "386"; // default to 16/32-bit 80386
ProcessorBitType processorBitType = ProcessorBitType::BIT32_386;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	ifstream in;
	ofstream out;
	ArgIntPtr bitsize = argInt0("bB", "bits", NULL, "define bit size to be 16, 32 or 64 bits (default is 32)");
	ArgStrPtr proc = argStr0("cC", "cpu", NULL, "define the processor 086, 186, 286, 386, 486, 586, 686, or x64 (default 16/32 bit is \"386\")");
	ArgStrPtr platform = argStr0("pP", "os", NULL, "define platform windows or linux (default is \"windows\")");
	ArgFilePtr outfile = argFile0("oO", "out", "<file>", "output file (default is \"-\")");
	ArgLitPtr help = argLit0("hH", "help", "print this help and exit");
	ArgLitPtr version = argLit0("vV", "version", "print version information and exit");
	ArgFilePtr infiles = argFileN(NULL, NULL, NULL, 1, argc + 2, "input file(s)");
	ArgEndPtr end = argEnd(20);
	void* argtable[] = { bitsize, proc, platform, outfile, help, version, infiles, end };
	const char* progname = "JWcc2";
	int exitcode = 0, nerrors = 0;
	string src = "";
	ostringstream ss;

	printf("%s c compiler (x86/x64 hosted) v1.0 (C) Copyright 2025-2026 Christopher D. Wade.\n", progname);
	printf("All Rights Reserved\n");

	if (argNullCheck(argtable) != 0)
	{
		printf("%s: insufficient memory\n", progname);
		exitcode = 1;
		goto exit;
	}
	bitsize->ival[0] = 32;
	proc->sval[0] = "386";
	outfile->filename[0] = "-";
	platform->sval[0] = "windows";
	processorBitType = ProcessorBitType::BIT32_386;

	nerrors = argParse(argc, argv, argtable);

	bits = bitsize->ival[0];
	processor = proc->sval[0];
	bit16 = bits == 16;
	bit32 = bits == 32;
	bit64 = bits == 64;
	isWindows = (platform->sval[0] == "windows");
	isLinux = (platform->sval[0] == "linux");

	cout << "Bits: " << bits << ", Processor: " << processor << ", Platform: " << (isWindows ? "Windows" : (isLinux ? "Linux" : "Unknown")) << endl;

	if (bit16)
	{
		if (processor == "086") processorBitType = ProcessorBitType::BIT16_8086;
		else if (processor == "186") processorBitType = ProcessorBitType::BIT16_186;
		else if (processor == "286") processorBitType = ProcessorBitType::BIT16_286;
		else if (processor == "386") processorBitType = ProcessorBitType::BIT16_386;
		else if (processor == "486") processorBitType = ProcessorBitType::BIT16_486;
		else if (processor == "586") processorBitType = ProcessorBitType::BIT16_586;
		else if (processor == "686") processorBitType = ProcessorBitType::BIT16_686;
		else
		{
			printf("Invalid processor for 16-bit: %s\n", processor.c_str());
			exitcode = 1;
			goto exit;
		}
	}
	else if (bit32)
	{
		if (processor == "386") processorBitType = ProcessorBitType::BIT32_386;
		else if (processor == "486") processorBitType = ProcessorBitType::BIT32_486;
		else if (processor == "586") processorBitType = ProcessorBitType::BIT32_586;
		else if (processor == "686") processorBitType = ProcessorBitType::BIT32_686;
		else
		{
			printf("Invalid processor for 32-bit: %s\n", processor.c_str());
			exitcode = 1;
			goto exit;
		}
	}
	else if (bit64)
	{
		if (processor == "x64") processorBitType = ProcessorBitType::BIT64_x64;
		else
		{
			printf("Invalid processor for 64-bit: %s\n", processor.c_str());
			exitcode = 1;
			goto exit;
		}
	}

	if (help->count > 0)
	{
		printf("Usage: %s", progname);
		argPrintSyntax(stdout, argtable, "\n");
		printf("Compiler that coverts c code to 16,32 or 64 bit JWasm v2.12pre assembler\n");
		argPrintGlossary(stdout, argtable, "  %-25s %s\n");
		exitcode = 0;
		goto exit;
	}

	if (version->count > 0)
	{
		printf("'%s' c code to JWasm v2.12pre assembler compiler.\n", progname);
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

	if (in.is_open() && out.is_open())
	{
		ss.clear();
		ss << in.rdbuf();
		src = ss.str();

		try
		{
			Parser p(src, in);
			auto prog = p.parseProgram();
			SemanticAnalyzer sema;
			bool ok = sema.analyze(*prog);
			if (!ok)
			{
				for (auto& e : sema.errors()) cerr << "Semantic error: " << e << "\n";
				return 1;
			}
			JWasmGenerator gen(out, bits, isWindows);
			gen.generate(*prog);
			//ASTPrinter printer;
			//printer.visit(*prog);
		}
		catch (const exception& ex)
		{
			cerr << "Error: " << ex.what() << "\n";
			return 1;
		}
	}
	else
	{
		cerr << "Error opening file " << infiles->filename[0] << endl;
		exitcode = -1;
	}
exit:
	argFreeTable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	in.close();
	out.close();
	return exitcode;
}