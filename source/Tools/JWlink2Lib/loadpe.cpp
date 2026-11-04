#include "pch.h"
#include "loadpe.h"
#include "structs.h"
#include "globals.h"

void DoAddResource(char* name)
{
	list_of_names* info;
	unsigned            len;

	len = strlen(name);
	info = (list_of_names*)malloc(sizeof(list_of_names) + len);
	memcpy(info->name, name, len + 1);
	info->next_name = FmtData.u.pe.resources;
	FmtData.u.pe.resources = info;
}