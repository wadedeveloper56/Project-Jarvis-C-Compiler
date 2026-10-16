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

shared_ptr<ProgramData> Compiler::getProgramData() const { return programData; }
shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> Compiler::getTypedefList() const { return typedefList; }
shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> Compiler::getStructList() const { return structList; }
shared_ptr<map<string, shared_ptr<FunctionDefinition>>> Compiler::getFunctionList() const { return functionList; }
void Compiler::setProgramData(shared_ptr<ProgramData> data) { programData = data; }
void Compiler::setTypedefList(shared_ptr<map<string, shared_ptr<DeclarationSpecifiersNode>>> list) { typedefList = list; }
void Compiler::setStructList(shared_ptr<map<string, shared_ptr<StructOrUnionSpecifier>>> list) { structList = list; }
void Compiler::setFunctionList(shared_ptr<map<string, shared_ptr<FunctionDefinition>>> list) { functionList = list; }
[[nodiscard]] bool Compiler::hasProgramData() const { return programData != nullptr; }
[[nodiscard]] bool Compiler::hasTypedefList() const { return typedefList != nullptr; }
[[nodiscard]] bool Compiler::hasStructList() const { return structList != nullptr; }
[[nodiscard]] bool Compiler::hasFunctionList() const { return functionList != nullptr; }
[[nodiscard]] bool Compiler::hasTypedef(const string& name) const { return typedefList->find(name) != typedefList->end(); }
[[nodiscard]] bool Compiler::hasFunction(const string& name) const { return functionList->find(name) != functionList->end(); }
[[nodiscard]] bool Compiler::hasStruct(const string& name) const { return structList->find(name) != structList->end(); }
void Compiler::addTypedef(const string& name, shared_ptr<DeclarationSpecifiersNode> declaration) { typedefList->insert({ name, declaration }); }
void Compiler::addFunction(const string& name, shared_ptr<FunctionDefinition> function) { functionList->insert({ name, function }); }
void Compiler::addStruct(const string& name, shared_ptr<StructOrUnionSpecifier> structSpecifier) { structList->insert({ name, structSpecifier }); }
void Compiler::addExternalDeclaration(shared_ptr<ExternalDeclaration> externalDeclaration) { programData->addExternalDeclaration(externalDeclaration); }
void Compiler::addEnum(const string& name, shared_ptr<EnumSpecifier> enumSpecifier) { enumList->insert({ name, enumSpecifier }); }
shared_ptr<StructOrUnionSpecifier> Compiler::findStruct(const string& name) const
{
	auto it = structList->find(name);
	if (it != structList->end())
	{
		return it->second;
	}
	return nullptr;
}
shared_ptr<DeclarationSpecifiersNode> Compiler::findTypedef(const string& name) const
{
	auto it = typedefList->find(name);
	if (it != typedefList->end())
	{
		return it->second;
	}
	return nullptr;
}
shared_ptr<FunctionDefinition> Compiler::findFunction(const string& name) const
{
	auto it = functionList->find(name);
	if (it != functionList->end())
	{
		return it->second;
	}
	return nullptr;
}
shared_ptr<EnumSpecifier> Compiler::findEnum(const string& name) const
{
	auto it = enumList->find(name);
	if (it != enumList->end())
	{
		return it->second;
	}
	return nullptr;
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
		return false;
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
