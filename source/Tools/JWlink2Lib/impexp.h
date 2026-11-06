#pragma once

#include <stdint.h>
#include "structs.h"

#define NOT_IMP_BY_ORDINAL 0xFFFFFFFF

typedef struct entry_export
{
    struct entry_export* next;
    char* name;
    uint32_t             ordinal;
    uint16_t             iopl_words;
    uint16_t             isresident : 1;
    uint16_t             isexported : 1;
    uint16_t             ismovable : 1;
    uint16_t             isanonymous : 1;
    uint16_t             isfree : 1;
    uint16_t             isprivate : 1;
    symbol* sym;
    char* impname;
    targ_addr               addr;
} entry_export;

entry_export* AllocExport(char* name, unsigned len);
void AddToExportList(entry_export* exp);
void MSImportKeyword(symbol* sym, length_name* modname, length_name* extname, unsigned long ordinal);
