#pragma once

#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)
#pragma warning(disable : 4333)
#include <windows.h>

namespace BinaryFormat {
	class BINARYFORMATS_API MemoryMappedFile
	{
		HANDLE hFile;
		HANDLE hMap;
		LPVOID lpBasePtr;
		LARGE_INTEGER fileSize;
		char* buffer;
	public:
		MemoryMappedFile(char* filename);
		~MemoryMappedFile();
		char* getBuffer() const;
		LONGLONG getFileSize() const;
	};
};
