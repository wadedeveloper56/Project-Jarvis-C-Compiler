#include "pch.h"
#include "interpreter.h"

#include <sstream>

using namespace WadeSpace;

Interpreter::Interpreter() :
	m_scanner(*this),
	m_parser(m_scanner, *this),
	m_location(0)
{
}

int Interpreter::parse()
{
	m_location = 0;
	return m_parser.parse();
}

void Interpreter::clear()
{
	m_location = 0;
}

void Interpreter::setStreams(std::istream* is, std::ostream* os)
{
	m_scanner.switch_streams(is, os);
}

void Interpreter::increaseLocation(unsigned int loc)
{
	m_location += loc;
	//cout << "increaseLocation(): " << loc << ", total = " << m_location << endl;
}

unsigned int Interpreter::location() const
{
	return m_location;
}
