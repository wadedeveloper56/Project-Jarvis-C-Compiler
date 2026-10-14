#include "pch.h"
#include "Compiler.h"
#include "scanner.h"
#include "parser.hpp"
#include "interpreter.h"
#include "ArgumentTable.h"
#include "GlobalVars.h"
#include "BaseCodeGenerator.h"
#include "MasmCodeGenerator.h"
#include "output.h"
#include "SemanticAnalyzer.h"
#include "Diagnostic.h"
//#include "ppMain.h"

using namespace WadeSpace;
using namespace std;

Compiler::Compiler()
{
	programData = make_shared<ProgramData>();
	typedefList = make_shared<map<string, shared_ptr<DeclarationSpecifiersNode>>>();
	structList = make_shared<map<string, shared_ptr<StructOrUnionSpecifier>>>();
	functionList = make_shared<map<string, shared_ptr<FunctionDefinition>>>();
	enumList = make_shared<map<string, shared_ptr<EnumSpecifier>>>();
}

void Compiler::compileFile(istream& inStr, ostream& out, int& exitcode)
{
	Interpreter i;
	i.setStreams(&inStr, &out);
	exitcode = i.parse();
	// Run semantic analysis on the parsed program
	SemanticAnalyzer analyzer;
	auto diagnostics = analyzer.analyze(programData);
	bool hasError = false;
	for (const auto& d : diagnostics)
	{
		out << d.toIDEString() << endl;
		if (d.severity == Diagnostic::Severity::Error) hasError = true;
	}

	if (hasError)
	{
		exitcode = 1;
		return;
	}

	shared_ptr<BaseCodeGenerator> generator = programData->processGlobalVariables();
	generator->generateCode(out);
}

void Compiler::compile(istream& in, ostream& out, int& exitcode)
{
	compileFile(in, out, exitcode);
}
