#include "pch.h"
#include "framework.h"
#include "File.h"

f_handle OpenFile2(const char* filename, int oflag, int pmode)
{
	return _open(filename, oflag, pmode);
}

int CloseFile2(f_handle handle)
{
	return _close(handle);
}

int FlushFile2(f_handle handle)
{
	return _commit(handle);
}

int ReadFile2(f_handle handle, void* const buffer, unsigned const buffer_size)
{
	return _read(handle, buffer, buffer_size);
}

int WriteFile2(f_handle handle, void* const buffer, unsigned const buffer_size)
{
	return _write(handle, buffer, buffer_size);
}

__int64 FileTell2(f_handle handle)
{
	return _telli64(handle);
}

__int64 FileSeek2(f_handle handle, __int64 offset, int origin)
{
	return _lseeki64(handle, offset, origin);
}

__int64 FileSize2(f_handle handle)
{
	return _filelengthi64(handle);
}
