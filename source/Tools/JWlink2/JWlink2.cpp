#include "pch.h"
#include "JWlink2.h"
#include "CommandLine.h"

using namespace std;
int g_LinkStatus;

int main(int argc, char* argv[]) {
    CommandLine* cmdLine = new CommandLine(argc, argv);
    int status = cmdLine->ProcessCommandLine();
    cout << "Structured Group Output:\n";
    cout << "========================================\n";
    cout << "Parsed Multi-map Layout (Even with errors):\n";
    for (const auto& [directive, arguments] : cmdLine->GetConfigMap())
    {
        cout << " [" << directive << "]\n";
        for (const auto& arg : arguments)
        {
            cout << "  -> " << arg << "\n";
        }
    }
    delete cmdLine;
    return status;
}

