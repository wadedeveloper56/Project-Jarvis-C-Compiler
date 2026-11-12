#include "pch.h"
#include "command_line.h"
#include "linker.h"

using namespace std;

int main(int argc, char** argv) 
{
	try
	{
		JWlink2::runLinker(JWlink2::commandline::parseArguments(argc, argv));
		return 0;
	}
	catch (const exception& error)
	{
		cerr << "JWlink2: error: " << error.what() << '\n';
		return 1;
	}
}
