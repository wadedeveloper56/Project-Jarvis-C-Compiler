#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

// ============================================================================
// SYSTEM RETURN STATUS CODE MACROS (jwlink / Watcom Architecture Specifications)
// ============================================================================
#define LNK_OK         0  // Mapping parity with EXIT_SUCCESS
#define LNK_WARNINGS   1  // Execution succeeded but non-fatal layout anomalies exist
#define LNK_ERRORS     2  // Linking pipeline terminated due to logical/structural faults
#define LNK_FATAL      3  // Critical system failure (Missing IO streams, Out of memory)

enum class TokenType
{
    Directive,
    Argument,
    ListElement,
    Delimiter
};

struct LinkerParam
{
    string value;
    TokenType type;
};

using LinkerConfig = map<string, vector<string>>;
using SubOptionMap = map<string, string>;
extern int g_LinkStatus;

class CommandLine
{
    int m_argc;
    char** m_argv;
    LinkerConfig configMap;
public:
    CommandLine(int argc, char** argv);
    ~CommandLine();
    int ProcessCommandLine();
	LinkerConfig GetConfigMap() const { return configMap; }
private:
    string ExpandResponseFiles(const string& inputStr);
    vector<LinkerParam> TokenizeJwlinkString(const string& cmdLine);
    LinkerConfig GroupDirectives(const vector<LinkerParam>& tokens);
    void convert(int argc, char* argv[], string& flattenedCmdLine);
};

