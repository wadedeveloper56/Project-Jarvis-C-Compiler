#include "pch.h"
#include "objpass1.h"
#include "objfree.h"

void FreeSegFlags(seg_flags* curr)
{
	seg_flags* next;

	while (curr != NULL)
	{
		next = curr->next;
		free(curr->name);
		free(curr);
		curr = next;
	}
}