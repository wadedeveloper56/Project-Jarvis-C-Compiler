#include "pch.h"
#include "globals.h"

using namespace std;

int main(int argc, char** argv)
{
    cout << "Hello World!\n";
    return((LinkState & LINK_ERROR) ? 1 : 0);
}
