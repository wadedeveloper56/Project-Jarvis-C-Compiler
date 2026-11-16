#include "pch.h"
#include "framework.h"
#include "File2.h"

errno_t OpenFile3(FileHandle* pfh, const char* filename, const char* pmode)
{
	return fopen_s(pfh, filename, pmode);
}

int CloseFile3(FileHandle handle)
{
	return fclose(handle);
}

int FlushFile3(FileHandle handle)
{
	return fflush(handle);
}

int ReadFile3(FileHandle handle, void* buffer, size_t size, size_t count)
{
	return fread(buffer, size, count, handle);
}

int WriteFile3(FileHandle handle, void* buffer, size_t size, size_t count)
{
	return fwrite(buffer, size, count, handle);
}

__int64 FileTell3(FileHandle handle)
{
	return _ftelli64(handle);
}

__int64 FileSeek3(FileHandle handle, __int64 offset, int origin)
{
	return _fseeki64(handle, offset, origin);
}

__int64 FileSize3(FileHandle handle)
{
	return _filelengthi64(_fileno(handle));
}
