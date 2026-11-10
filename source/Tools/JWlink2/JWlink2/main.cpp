#include "pch.h"
#include "command_line.h"

using namespace jwlink2;
using namespace std;

int main(int argc, char** argv) 
{
	try
	{
		runLinker(parseArguments(argc, argv));
		return 0;
	}
	catch (const exception& error)
	{
		cerr << "JWlink2: error: " << error.what() << '\n';
		return 1;
	}
}
