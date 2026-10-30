#pragma once

#include "File.h"
#include "start.h"
#include "globals.h"

#define MAX_OPEN_FILES 12       // the maximum number of open files.
#define TOOMANY EMFILE
#define RESOURCE_MAX_SIZE       128
#define PATH_SEP '\\'
#define IS_PATH_SEP( a ) ((a)=='\\' || (a)=='/' || (a)==':')
#define CASE_PATH_SEP   case '\\': case '/': case ':'
#define PATH_LIST_SEP ';'
#define PATH_MAX FILENAME_MAX

typedef struct
{
	void* buffer;
	unsigned long       pos;
	infilelist* currfile;
} bufferedfile;

extern infilelist* CachedLibFiles;
extern infilelist* CachedFiles;

void TrapBreak(int sig_num);
void CheckBreak(void);
void SetBreak(void);
void RestoreBreak(void);
void LnkFilesInit(void);
void PrintIOError(unsigned msg, char* types, char* name);
int DoOpen(char* name, unsigned mode, bool isexe);
f_handle QOpenR(char* name);
f_handle QOpenRW(char* name);
int ResOpen(const char* path, int access, ...);
f_handle ExeCreate(char* name);
f_handle ExeOpen(char* name);
unsigned QRead(f_handle file, void* buffer, unsigned len, char* name);
unsigned QWrite(f_handle file, void* buffer, unsigned len, char* name);
void QWriteNL(f_handle file, char* name);
void QClose(f_handle file, char* name);
long QLSeek(f_handle file, long position, int start, char* name);
void QSeek(f_handle file, long position, char* name);
unsigned long QPos(f_handle file);
unsigned long QFileSize(f_handle file);
void QDelete(char* name);
bool QReadStr(f_handle file, char* dest, unsigned size, char* name);
bool QIsDevice(f_handle file);
f_handle NSOpen(char* name, unsigned mode);
f_handle QObjOpen(char* name);
f_handle TempFileOpen(char* name);
int QMakeFileName(char** pos, char* name, char* fname);
bool QHavePath(char* name);
bool QSysHelp(char** cmd_ptr);
bool QModTime(char* name, time_t* time);
time_t QFModTime(int handle);
char WaitForKey(void);
int _bgetcmd(char* buffer, int len);
char* getcmd(char* buffer);
void GetCmdLine(char* buff);
bool CleanCachedHandles(void);
f_handle SearchPath(char* name);
char* GetEnvString(char* envname);
void SetCommandFile(f_handle file, char* fname);
