#pragma once

#include <string>

namespace WadeSpace
{
	struct Diagnostic
	{
		enum class Severity { Info, Warning, Error };

		std::string file;
		int line = 0;
		int column = 0;
		Severity severity = Severity::Error;
		std::string code; // machine-readable code like E001, W201
		std::string message;

		Diagnostic() = default;
		Diagnostic(Severity sev, const std::string& msg, const std::string& file = "", int line = 0, int column = 0)
			: file(file), line(line), column(column), severity(sev), message(msg) {}

		Diagnostic(Severity sev, const std::string& msg, const std::string& code, const std::string& file = "", int line = 0, int column = 0)
			: file(file), line(line), column(column), severity(sev), code(code), message(msg) {}

		std::string toString() const;
		std::string toIDEString() const;
	};
}
