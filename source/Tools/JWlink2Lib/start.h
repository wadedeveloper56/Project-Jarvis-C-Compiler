#pragma once

#include <stdio.h>
#include "structs.h"

#define STDIN_HANDLE        (_fileno(stdin))
#define STDOUT_HANDLE       (_fileno(stdout))
#define STDERR_HANDLE       (_fileno(stderr))

using namespace std;

void SetSegMask(void);
void ResetCmdFile(void);
void GetCmdLine(char* buff);
void NewCommandSource(char* name, char* buff, method how);
void DoCmdFile(char* fname);
void DoLink(char* cmdline);
section* NewSection(void);
void ResetSubSystems(void);
void LinkMeBaby(void);
int Spawn(void (*fn)(void));
void LinkMainLine(char* cmds);
void InitSubSystems(void);
void FiniSubSystems(void);
int LinkerMain(int argc, char* argv[]);
