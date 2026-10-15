#include "pch.h"
#include <Windows.h>
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

bool Compiler::compileFile(istream& inStr, ostream& out)
{
	Interpreter i;
	i.setStreams(&inStr, &out);
	int pareseerror = i.parse();
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
	    return true;
	}

	shared_ptr<BaseCodeGenerator> generator = programData->processGlobalVariables();
	generator->generateCode(out);
	return false;
}

bool WadeSpace::Compiler::StartChildProcessAndWait(char*  commandLine)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    // Start the child process
    if (!CreateProcessA(
        nullptr,    // Application name (use command line instead)
        commandLine,// Command line arguments
        nullptr,    // Process handle not inheritable
        nullptr,    // Thread handle not inheritable
        FALSE,      // Set handle inheritance to FALSE
        0,          // No creation flags
        nullptr,    // Use parent's environment block
        nullptr,    // Use parent's starting directory 
        &si,        // Pointer to STARTUPINFO structure
        &pi         // Pointer to PROCESS_INFORMATION structure
    ))
    {
        cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        return true;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return false;
}

bool Compiler::StartPreProcessor(char  *filename, const char* name)
{
    char commandLine[_MAX_PATH + 50];
    _snprintf_s(commandLine, _MAX_PATH + 50, _MAX_PATH + 50, "JWcpp.exe -o%s %s", filename, name);

    return StartChildProcessAndWait(commandLine);
}

bool Compiler::compile(istream& in, ostream& out, const string& originalFilename)
{

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char filename[_MAX_PATH];

    cout << "Compiling " << originalFilename << endl;
    const char* name = originalFilename.c_str();
    errno_t err = _splitpath_s(name, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    _makepath_s(filename, _MAX_PATH, drive, dir, fname, "i");

    bool preprocessorResult = StartPreProcessor(filename, name);
    if (preprocessorResult) return true;

	ifstream newin;
    newin.open(filename, ifstream::in);
	bool compileResult = compileFile(newin, out);
	newin.close();
	return compileResult;
}
