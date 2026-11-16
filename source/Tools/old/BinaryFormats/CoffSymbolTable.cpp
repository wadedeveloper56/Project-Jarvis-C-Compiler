#include "pch.h"
#include "framework.h"
#include "BinaryFormat.h"

using namespace std;

COFFSymbol::COFFSymbol(NTSymbolPtr pSymbolData, PSTR pStringTable, DWORD index)
{
	memset(&m_szTypeName, 0, sizeof(0));
	m_pSymbolData = pSymbolData;
	m_pStringTable = pStringTable;
	m_index = index;
	m_pszShortString = 0;
}

COFFSymbol::~COFFSymbol()
{
	CleanUp();
}

DWORD COFFSymbol::GetIndex()
{
	return m_index;
}

DWORD COFFSymbol::GetValue()
{
	return m_pSymbolData ? m_pSymbolData->Value : 0;
}

SHORT COFFSymbol::GetSectionNumber()
{
	return m_pSymbolData ? m_pSymbolData->SectionNumber : 0;
}

WORD COFFSymbol::GetType()
{
	return m_pSymbolData ? m_pSymbolData->Type : 0;
}

BYTE COFFSymbol::GetStorageClass()
{
	return m_pSymbolData ? m_pSymbolData->StorageClass : 0;
}

BOOL COFFSymbol::GetNumberOfAuxSymbols()
{
	return m_pSymbolData ? m_pSymbolData->NumberOfAuxSymbols : 0;
}

void COFFSymbol::CleanUp(void)
{
	if (m_pszShortString)
	{
		free(m_pszShortString);
	}

	m_pszShortString = 0;
}

NTSymbolPtr COFFSymbol::GetSymbol()
{
	return m_pSymbolData;
}

PSTR COFFSymbol::GetName()
{
	if (!m_pSymbolData)
	{
		return 0;
	}
	if (0 != m_pSymbolData->N.Name.Short)
	{
		m_pszShortString = (PSTR)malloc(9);
		memcpy(m_pszShortString, m_pSymbolData->N.ShortName, 8);
		m_pszShortString[8] = 0;

		return m_pszShortString;
	}
	return m_pStringTable + m_pSymbolData->N.Name.Long;
}

PSTR COFFSymbol::GetTypeName()
{
	if (!m_pSymbolData)
	{
		return 0;
	}
	sprintf(m_szTypeName, "%04X", m_pSymbolData->Type);
	return m_szTypeName;
}

BOOL COFFSymbol::GetAuxSymbolAsString(PSTR pszBuffer, unsigned cbBuffer)
{
	if (!m_pSymbolData || (0 == m_pSymbolData->NumberOfAuxSymbols))
	{
		return FALSE;
	}
	PIMAGE_AUX_SYMBOL auxSym = (PIMAGE_AUX_SYMBOL)(m_pSymbolData + 1);
	if (m_pSymbolData->StorageClass == IMAGE_SYM_CLASS_FILE)
	{
		lstrcpynA(pszBuffer, (char*)auxSym, cbBuffer);
	}
	else if ((m_pSymbolData->StorageClass == IMAGE_SYM_CLASS_EXTERNAL))
	{
		if ((m_pSymbolData->Type & 0xF0) == (IMAGE_SYM_DTYPE_FUNCTION << 4))
		{
			sprintf(pszBuffer, "tag: %04X  size: %04X  Line #'s: %08X  next fn: %04X", auxSym->Sym.TagIndex, auxSym->Sym.Misc.TotalSize, auxSym->Sym.FcnAry.Function.PointerToLinenumber, auxSym->Sym.FcnAry.Function.PointerToNextFunction);
		}
	}
	else if ((m_pSymbolData->StorageClass == IMAGE_SYM_CLASS_STATIC))
	{
		sprintf(pszBuffer, "Section: %04X  Len: %05X  Relocs: %04X  LineNums: %04X checksum:%08X", auxSym->Section.Number, auxSym->Section.Length, auxSym->Section.NumberOfRelocations, auxSym->Section.NumberOfLinenumbers,auxSym->Section.CheckSum);
	}
	else
	{
		lstrcpynA(pszBuffer, "<unhandled aux symbol>", cbBuffer);
	}

	return TRUE;
}

const char* SzStorageClass1[] =
{
	"NULL",
	"AUTOMATIC",
	"EXTERNAL",
	"STATIC",
	"REGISTER",
	"EXTERNAL_DEF",
	"LABEL",
	"UNDEFINED_LABEL",
	"MEMBER_OF_STRUCT",
	"ARGUMENT",
	"STRUCT_TAG",
	"MEMBER_OF_UNION",
	"UNION_TAG",
	"TYPE_DEFINITION",
	"UNDEFINED_STATIC",
	"ENUM_TAG",
	"MEMBER_OF_ENUM",
	"REGISTER_PARAM",
	"BIT_FIELD"
};

const char* SzStorageClass2[] =
{
	"BLOCK",
	"FUNCTION",
	"END_OF_STRUCT",
	"FILE",
	"SECTION",
	"WEAK_EXTERNAL"
};

PSTR COFFSymbol::GetStorageClassName()
{
	if (!m_pSymbolData)
	{
		return 0;
	}

	BYTE storageClass = m_pSymbolData->StorageClass;

	if (storageClass <= IMAGE_SYM_CLASS_BIT_FIELD)
	{
		return (PSTR)SzStorageClass1[storageClass];
	}

	if ((storageClass >= IMAGE_SYM_CLASS_BLOCK) && (storageClass <= IMAGE_SYM_CLASS_WEAK_EXTERNAL))
	{
		return (PSTR)SzStorageClass2[storageClass - IMAGE_SYM_CLASS_BLOCK];
	}

	return (PSTR)"???";
}

COFFSymbolTable::COFFSymbolTable(PVOID pSymbolBase, unsigned cSymbols)
{
	m_pSymbolBase = (NTSymbolPtr)pSymbolBase;
	m_cSymbols = cSymbols;
	m_pStringTable = (PSTR)(m_pSymbolBase + m_cSymbols);
}

COFFSymbolTable::~COFFSymbolTable()
{
}

PCOFFSymbol COFFSymbolTable::GetNextSymbol(PCOFFSymbol pSymbol)
{
	if (0 == pSymbol)
	{
		pSymbol = new COFFSymbol((NTSymbolPtr)m_pSymbolBase, m_pStringTable, 0);
		return pSymbol;
	}

	BYTE cAuxSymbols = pSymbol->m_pSymbolData->NumberOfAuxSymbols;

	if ((pSymbol->m_index + cAuxSymbols + 1) >= m_cSymbols)
	{
		delete pSymbol;

		return 0;
	}

	pSymbol->CleanUp();
	pSymbol->m_pSymbolData += (1 + cAuxSymbols);
	pSymbol->m_index += (1 + cAuxSymbols);

	return pSymbol;
}

PCOFFSymbol COFFSymbolTable::GetNearestSymbolFromRVA(DWORD rva, BOOL fExact)
{
	if (!fExact)
	{
		return 0;
	}

	PCOFFSymbol pSymbol = GetNextSymbol(0);

	while (pSymbol)
	{
		if (rva == pSymbol->GetValue())
		{
			if (pSymbol->GetStorageClass() == IMAGE_SYM_CLASS_EXTERNAL)
			{
				return pSymbol;
			}

			if (pSymbol->GetStorageClass() == IMAGE_SYM_CLASS_STATIC)
			{
				if (pSymbol->GetType())
				{
					return pSymbol;
				}
			}
		}

		pSymbol = GetNextSymbol(pSymbol);
	}

	return 0;
}

PCOFFSymbol COFFSymbolTable::GetSymbolFromIndex(DWORD index)
{
	if (index >= m_cSymbols)
	{
		return 0;
	}
	PCOFFSymbol pSymbol = new COFFSymbol(m_pSymbolBase + index, m_pStringTable, index);
	return pSymbol;
}
