#pragma once

#include "structs.h"

using namespace std;

extern int _argc;
extern char** _argv;
extern void* SpawnStack;
extern char* ArgSave;
extern file_defext Extension;
extern file_list** CurrFList;
extern tok Token;
extern commandflag CmdFlags;
extern char* Name;
extern sysblock* SysBlocks;
extern sysblock* LinkCommands;
extern path_entry* Path;           /* path for object files                */
extern struct fmt_data FmtData;       /* format specific data */
extern section* Root;           /* pointer to root information          */
extern section* CurrSect;       /* current section for file processing  */
extern ::byte DBIFlag;
extern int OvlNum;         /* current overlay number               */
extern stateflag      LinkState;      /* flags to indicate linker state */

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
