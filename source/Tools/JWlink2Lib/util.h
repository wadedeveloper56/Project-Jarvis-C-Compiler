#pragma once

#include "structs.h"

#define NLCHAR         '\n'
#define CTRLZ          '\32'
#define _LinkerPrompt "JWLINK>"

char* ChkStrDup(char* str);
char* tostring(void);
char* FileName(char* buff, unsigned len, file_defext etype, bool force);
char* ChkToString(void* mem, unsigned len);
bool GetToken(sep_type req, tokcontrol ctrl);
bool GetTokenEx(sep_type req, tokcontrol ctrl, cmdfilelist* resetpoint, bool* pbreset);
void EatWhite(void);
void StartNewFile(void);
bool MakeToken(tokcontrol ctrl, sep_type separator);
void BackupParser(void);
void ExpandEnvVariable(void);
void GetNewLine(void);
void RestoreCmdLine(void);
void RestoreParser(void);
void MapEscapeChar(void);
unsigned MapDoubleByteChar(unsigned char c);
int ParseNumber(char* str, int radix);
bool ProcOne(parse_entry* entry, sep_type req, bool suicide);
bool HintFormat(exe_format hint);