#include "pch.h"
#include "BinaryFormat.h"

using namespace BinaryFormat;

MemoryMappedFile::MemoryMappedFile(char* filename) :buffer(nullptr), hFile(0), hMap(0), lpBasePtr(0)
{
	hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileSizeEx(hFile, &fileSize);
		hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hMap == NULL) {
			CloseHandle(hFile);
		}
		else {
			lpBasePtr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
			if (lpBasePtr == NULL) {
				CloseHandle(hMap);
				CloseHandle(hFile);
			}
			else {
				buffer = static_cast<char*>(lpBasePtr);
			}
		}
	}
}

MemoryMappedFile::~MemoryMappedFile()
{
	UnmapViewOfFile(lpBasePtr);
	CloseHandle(hMap);
	CloseHandle(hFile);
}

char* MemoryMappedFile::getBuffer() const
{
	return buffer;
}

LONGLONG MemoryMappedFile::getFileSize() const
{
	return fileSize.QuadPart;
}