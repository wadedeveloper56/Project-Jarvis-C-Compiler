#include "pch.h"
#include "SemanticAnalyzer.h"
#include "ProgramData.h"
#include "ExternalDeclaration.h"
#include "InitDeclarator.h"
#include "FunctionDefinition.h"
#include "Diagnostic.h"
#include "SymbolTable.h"
#include <unordered_set>

using namespace WadeSpace;
using namespace std;

vector<Diagnostic> SemanticAnalyzer::analyze(shared_ptr<ProgramData> programData)
{
	vector<Diagnostic> diagnostics;
	if (!programData || !programData->hasProgram()) return diagnostics;

	auto program = programData->getProgram();
	SymbolTable symbols;

	// First pass: declare typedefs and function signatures in global scope
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
					SymbolInfo info;
					info.kind = SymbolInfo::Kind::Function;
					info.name = name;
					// Type information can be attached later; leave type null for now
					if (!symbols.declare(info))
					{
						diagnostics.emplace_back(Diagnostic::Severity::Error, "Duplicate function definition: " + name);
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
							SymbolInfo info;
							info.kind = SymbolInfo::Kind::Typedef;
							info.name = varName;
							if (!symbols.declare(info))
							{
								diagnostics.emplace_back(Diagnostic::Severity::Error, "Duplicate typedef name: " + varName);
							}
						}
					}
				}
			}
		}
	}

	// Second pass: deeper per-function checks and block-level declarations
	for (auto& ext : *program)
	{
		if (ext->hasFunction())
		{
			auto fn = ext->getFunctionDefinition();
			if (!fn) continue;

			// enter function scope
			symbols.pushScope();

			// add parameters
			if (fn->hasDeclarator() && fn->getDeclarator()->hasDirectDeclarator())
			{
				auto dd = fn->getDeclarator()->getDirectDeclarator();
				if (dd->hasParameterTypeList())
				{
					auto params = dd->getParameterTypeList();
					if (params->hasVectorParameterDeclaration())
					{
						for (auto& param : *params->getVectorParameterDeclaration())
						{
							if (param->hasDeclarator() && param->getDeclarator()->hasDirectDeclarator())
							{
								auto pdd = param->getDeclarator()->getDirectDeclarator();
								if (pdd->hasIdentifier())
								{
									string pname = pdd->getIdentifier()->getSymbolName();
									SymbolInfo sinfo;
									sinfo.kind = SymbolInfo::Kind::Parameter;
									sinfo.name = pname;
									if (!symbols.declare(sinfo))
									{
										diagnostics.emplace_back(Diagnostic::Severity::Error, "Duplicate parameter name: " + pname);
									}
								}
							}
						}
					}
				}
			}

			// process local declarations in function (very basic: find declarations attached to function)
			if (fn->hasVectorDeclaration())
			{
				for (auto& decl : *fn->getVectorDeclaration())
				{
					if (decl->hasVectorInitDeclarator())
					{
						for (auto& init : *decl->getVectorInitDeclarator())
						{
							string varName = init->getVariableName();
							if (!varName.empty())
							{
								SymbolInfo info;
								info.kind = SymbolInfo::Kind::Variable;
								info.name = varName;
								if (!symbols.declare(info))
								{
									diagnostics.emplace_back(Diagnostic::Severity::Error, "Duplicate local declaration: " + varName);
								}
							}
						}
					}
				}
			}

			// TODO: walk function body AST for identifier uses and more checks

			// exit function scope
			symbols.popScope();
		}
	}

	return diagnostics;
}
