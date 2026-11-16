#pragma once

#include <string>
#include <vector>
#include <memory>

namespace WadeSpace
{
	class ProgramData;
	struct Diagnostic;

	class SemanticAnalyzer
	{
	public:
		SemanticAnalyzer() = default;
		virtual ~SemanticAnalyzer() = default;

		// Analyze the program data and return a list of diagnostics.
		std::vector<Diagnostic> analyze(std::shared_ptr<ProgramData> programData);
	};
}
