#include "pch.h"
#include "framework.h"
#include "BinaryFormat.h"

DOSOBJFilePtr loadDOSWin16ObjFile(FileType fileType, char* buffer, LONGLONG fileSize)
{
	bool done = false;
	int offset = 0;
	unsigned char rectype;
	unsigned long reclength;

	DOSOBJFilePtr ptr = new DOSOBJFile();
	while (!done)
	{
		DOSOBJRecordPtr record = new DOSOBJRecord();
		rectype = buffer[offset];
		reclength = ((unsigned char)buffer[offset + 1]) + (256 * (unsigned char)buffer[offset + 2]);
		record->rectype = rectype;
		record->reclength = reclength;
		record->buffer = new unsigned char[reclength];
		memset(record->buffer, 0, reclength);
		for (int i = 0; i < reclength; i++) record->buffer[i] = buffer[offset + 3 + i];
		offset += 3 + reclength;
		if (rectype == (unsigned char)MODEND || rectype == (unsigned char)MODEND32) done = true;
		ptr->records.push_back(record);
	}
	return ptr;
}

WIN16EXEFilePtr loadWin16ExeFile(FileType fileType, char* buffer, LONGLONG fileSize)
{
	WIN16EXEFilePtr ptr = new WIN16EXEFile();
	DosHeaderPtr dosHeader = (DosHeaderPtr)buffer;
	NEHeaderPtr neHeader = MakePtr(NEHeaderPtr, dosHeader, dosHeader->e_lfanew);
	//segment list
	uint16_t offset = neHeader->SegTableOffset;
	uint16_t count = neHeader->SegCount;
	for (int i = 0; i < count; i++)
	{
		NESegmentTableEntryPtr segmentTableEntry = MakePtr(NESegmentTableEntryPtr, neHeader, offset);
		ptr->segmentTable.push_back(segmentTableEntry);
		offset += sizeof(NESegmentTableEntry);
	}
	//resource list
	ptr->resourceTable = new NEResourceTable();
	offset = neHeader->ResTableOffset;
	NEResourceTableHeaderPtr resourceTable = MakePtr(NEResourceTableHeaderPtr, neHeader, offset);
	ptr->resourceTable->header = resourceTable;
	offset += sizeof(NEResourceTableHeader);
	while (true)
	{
		NEResourceTypeEntryPtr resourceTypeList = new NEResourceTypeEntry();
		NEResourceTypeHeaderPtr resourceType = MakePtr(NEResourceTypeHeaderPtr, neHeader, offset);
		if (resourceType->TypeID == 0x0000) break;
		resourceTypeList->header = resourceType;
		offset += sizeof(NEResourceTypeHeader);
		for (int i = 0; i < resourceType->ResourceCount; i++)
		{
			NEResourcePtr resource = MakePtr(NEResourcePtr, neHeader, offset);
			NEResourceEntryPtr resourceEntry = new NEResourceEntry();
			resourceEntry->resource = resource;
			resourceTypeList->entries.push_back(resourceEntry);
			offset += sizeof(NEResource);
		}
		ptr->resourceTable->entries.push_back(resourceTypeList);
	}
	return ptr;
}
