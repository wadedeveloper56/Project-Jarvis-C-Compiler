#include "pch.h"
#include "framework.h"
#include "BinaryFormat.h"

using namespace std;

PSTR PszLongnames = 0;

DWORD ConvertBigEndian(DWORD bigEndian)
{
	DWORD temp = 0;
	temp |= bigEndian >> 24;
	temp |= ((bigEndian & 0x00FF0000) >> 8);
	temp |= ((bigEndian & 0x0000FF00) << 8);
	temp |= ((bigEndian & 0x000000FF) << 24);
	return temp;
}

void DumpFirstLinkerMember(vector<LIBFileLinkerMembersPtr> *first, PVOID p)
{
	DWORD cSymbols = *(PDWORD)p;
	PDWORD pMemberOffsets = MakePtr(PDWORD, p, 4);
	PSTR pSymbolName;
	unsigned i;

	cSymbols = ConvertBigEndian(cSymbols);
	pSymbolName = MakePtr(PSTR, pMemberOffsets, 4 * cSymbols);
	for (i = 0; i < cSymbols; i++)
	{
		LIBFileLinkerMembersPtr ptr = new LIBFileLinkerMembers;
		DWORD offset = ConvertBigEndian(*pMemberOffsets);
		ptr->offset = offset;
		ptr->pSymbolName = pSymbolName;
		pMemberOffsets++;
		pSymbolName += strlen(pSymbolName) + 1;
		first->push_back(ptr);
	}
}

void DumpSecondLinkerMember(vector<LIBFileLinkerMembersPtr>* second, PVOID p)
{
	DWORD cArchiveMembers = *(PDWORD)p;
	PDWORD pMemberOffsets = MakePtr(PDWORD, p, 4);
	DWORD cSymbols;
	PSTR pSymbolName;
	PWORD pIndices;
	unsigned i;

	cArchiveMembers = cArchiveMembers;
	cSymbols = pMemberOffsets[cArchiveMembers];
	pIndices = MakePtr(PWORD, p, 4 + cArchiveMembers * sizeof(DWORD) + 4);
	pSymbolName = MakePtr(PSTR, pIndices, cSymbols * sizeof(WORD));
	for (i = 0; i < cSymbols; i++)
	{
		LIBFileLinkerMembersPtr ptr = new LIBFileLinkerMembers;
		ptr->offset = pMemberOffsets[pIndices[i] - 1];
		ptr->pSymbolName = pSymbolName;
		pSymbolName += strlen(pSymbolName) + 1;
		second->push_back(ptr);
	}
}

void DumpLongnamesMember(vector<LIBFileLinkerMembersPtr>* longnames, PVOID p, DWORD len)
{
	PSTR pszName = (PSTR)p;
	DWORD offset = 0;
	PszLongnames = (PSTR)p;
	while (offset < len)
	{
		LIBFileLinkerMembersPtr ptr = new LIBFileLinkerMembers;
		unsigned cbString = lstrlenA(pszName) + 1;
		ptr->offset = offset;
		ptr->pSymbolName = pszName;
		offset += cbString;
		pszName += cbString;
		longnames->push_back(ptr);
	}
}

LIBFilePtr loadLibFile(FileType fileType, char* lpFileBase, LONGLONG fileSize)
{
	LIBFilePtr ptr = new LIBFile;
	PIMAGE_ARCHIVE_MEMBER_HEADER pArchHeader;
	BOOL fSawFirstLinkerMember = FALSE;
	BOOL fSawSecondLinkerMember = FALSE;
	BOOL fBreak = FALSE;

	if (strncmp(lpFileBase, IMAGE_ARCHIVE_START, IMAGE_ARCHIVE_START_SIZE))
	{
		printf("Not a valid .LIB file - signature not found\n");
		return NULL;
	}
	pArchHeader = MakePtr(PIMAGE_ARCHIVE_MEMBER_HEADER, lpFileBase, IMAGE_ARCHIVE_START_SIZE);
	while (pArchHeader)
	{
		LIBFileEntryPtr entry = new LIBFileEntry;
		for (int i = 0; i < 16; i++) entry->header.Name[i] = pArchHeader->Name[i];
		for (int i = 0; i < 12; i++) entry->header.Date[i] = pArchHeader->Date[i];
		for (int i = 0; i < 6; i++) entry->header.UserID[i] = pArchHeader->UserID[i];
		for (int i = 0; i < 6; i++) entry->header.GroupID[i] = pArchHeader->GroupID[i];
		for (int i = 0; i < 8; i++) entry->header.Mode[i] = pArchHeader->Mode[i];
		for (int i = 0; i < 10; i++) entry->header.Size[i] = pArchHeader->Size[i];
		for (int i = 0; i < 2; i++) entry->header.EndHeader[i] = pArchHeader->EndHeader[i];

		DWORD thisMemberSize;
		if (!strncmp((char*)pArchHeader->Name, IMAGE_ARCHIVE_LINKER_MEMBER, 16))
		{
			if (!fSawFirstLinkerMember)
			{
				DumpFirstLinkerMember(&entry->first,(PVOID)(pArchHeader + 1));
				fSawFirstLinkerMember = TRUE;
			}
			else if (!fSawSecondLinkerMember)
			{
				DumpSecondLinkerMember(&entry->second, (PVOID)(pArchHeader + 1));
				fSawSecondLinkerMember = TRUE;
			}
		}
		else if (!strncmp((char*)pArchHeader->Name, IMAGE_ARCHIVE_LONGNAMES_MEMBER, 16))
		{
			DumpLongnamesMember(&entry->longNames, (PVOID)(pArchHeader + 1), atoi((char*)pArchHeader->Size));
		}
		else
		{
			char *buffer = (char *)(pArchHeader + 1);
			OBJFilePtr obj = loadWin3264ObjFile(fileType, buffer, 0);
			entry->objFile = obj;
		}
		thisMemberSize = atoi((char*)pArchHeader->Size) + IMAGE_SIZEOF_ARCHIVE_MEMBER_HDR;
		thisMemberSize = (thisMemberSize + 1) & ~1;
		ptr->entrys.push_back(entry);
		pArchHeader = MakePtr(PIMAGE_ARCHIVE_MEMBER_HEADER, pArchHeader, thisMemberSize);
		if (strncmp((char*)pArchHeader->EndHeader, IMAGE_ARCHIVE_END, 2))
			break;
	}
	return ptr;
}