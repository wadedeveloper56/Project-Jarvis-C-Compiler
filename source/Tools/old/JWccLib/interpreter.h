#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include "scanner.h"
#include "parser.hpp"

namespace WadeSpace
{
	class Interpreter
	{
	public:
		Interpreter();
		int parse();
		void clear();
		string str() const;
		void setStreams(std::istream* is, std::ostream* os);
		friend class Parser;
		friend class Scanner;

	private:
		void increaseLocation(unsigned int loc);
		unsigned int location() const;

	private:
		Scanner m_scanner;
		Parser m_parser;
		unsigned int m_location;
	};
}

#endif
