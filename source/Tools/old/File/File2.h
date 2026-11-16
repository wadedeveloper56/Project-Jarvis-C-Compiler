#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef FILE *FileHandle;
#define NIL_HANDLE -1

errno_t OpenFile3(FileHandle* pfh, const char* filename, const char* pmode);
int CloseFile3(FileHandle handle);
int FlushFile3(FileHandle handle);
int ReadFile3(FileHandle handle, void* buffer, size_t size, size_t count);
int WriteFile3(FileHandle handle, void* buffer, size_t size, size_t count);
__int64 FileTell3(FileHandle handle);
__int64 FileSeek3(FileHandle handle, __int64 offset, int origin);
__int64 FileSize3(FileHandle handle);

#ifdef __cplusplus
}
#endif