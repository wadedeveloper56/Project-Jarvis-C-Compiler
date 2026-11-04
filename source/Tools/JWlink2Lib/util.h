#pragma once

#include "structs.h"

#define NLCHAR         '\n'
#define CTRLZ          '\32'
#define _LinkerPrompt "JWLINK>"

char* ChkStrDup(char* str);

char* ChkToString(void* mem, unsigned len);

char* FileName(char* buff, unsigned len, file_defext etype, bool force);

bool GetToken(sep_type req, tokcontrol ctrl);

bool GetTokenEx(sep_type req, tokcontrol ctrl, cmdfilelist* resetpoint, bool* pbreset);

#define IS_WHITESPACE(ptr) (*(ptr) == ' ' || *(ptr) =='\t' || *(ptr) == '\r')

void EatWhite(void);

void StartNewFile(void);

bool MakeToken(tokcontrol ctrl, sep_type separator);

void BackupParser(void);

void ExpandEnvVariable(void);

void OutPutPrompt(char* str);

void GetNewLine(void);

void RestoreCmdLine(void);

void RestoreParser(void);

char* tostring(void);

void MapEscapeChar(void);

unsigned MapDoubleByteChar(unsigned char c);

int ParseNumber(char* str, int radix);

bool ProcOne(parse_entry* entry, sep_type req, bool suicide);

bool HintFormat(exe_format hint);

bool HaveEquals(tokcontrol ctrl);

bool GetLong(uint32_t* addr);

ord_state getatol(uint32_t* pnt);

bool WildCard(bool (*rtn)(void), tokcontrol ctrl);

bool ProcArgListEx(bool (*rtn)(void), tokcontrol ctrl, cmdfilelist* resetpoint);

bool ProcArgList(bool (*rtn)(void), tokcontrol ctrl);

ord_state getatoi(uint16_t* pnt);
bool CheckFence(void);
uint16_t blog_32(uint32_t value);
char* totext(void);