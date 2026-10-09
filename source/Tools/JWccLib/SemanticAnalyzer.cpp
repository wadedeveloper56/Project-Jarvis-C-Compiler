#include "pch.h"
#include "SemanticAnalyzer.h"
#include "ProgramData.h"
#include "ExternalDeclaration.h"
#include "InitDeclarator.h"
#include "FunctionDefinition.h"
#include <unordered_set>

using namespace WadeSpace;
using namespace std;

vector<string> SemanticAnalyzer::analyze(shared_ptr<ProgramData> programData)
{
	vector<string> diagnostics;
	if (!programData || !programData->hasProgram()) return diagnostics;

	auto program = programData->getProgram();
	unordered_set<string> functionNames;
	unordered_set<string> typedefNames;

	for (auto& ext : *program)
	{
		if (ext->hasFunction())
		{
			auto fn = ext->getFunctionDefinition();
			if (fn && fn->hasDeclarator() && fn->getDeclarator()->hasDirectDeclarator())
			{
				auto dd = fn->getDeclarator()->getDirectDeclarator();
				if (dd->hasIdentifier())
				{
					string name = dd->getIdentifier()->getSymbolName();
					if (functionNames.find(name) != functionNames.end())
					{
						diagnostics.push_back("Duplicate function definition: " + name);
					}
					else
					{
						functionNames.insert(name);
					}
				}
			}
		}
		else if (ext->hasDeclaration())
		{
			if (ext->isTypedef())
			{
				auto decl = ext->getDeclaration();
				if (decl->hasVectorInitDeclarator())
				{
					for (auto& init : *decl->getVectorInitDeclarator())
					{
						string varName = init->getVariableName();
						if (!varName.empty())
						{
							if (typedefNames.find(varName) != typedefNames.end())
							{
								diagnostics.push_back("Duplicate typedef name: " + varName);
							}
							else
							{
								typedefNames.insert(varName);
							}
						}
					}
				}
			}
		}
	}

	return diagnostics;
}
