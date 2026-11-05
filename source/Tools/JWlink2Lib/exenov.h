#pragma once

#include <stdint.h>
#include "pshpack1.h"

#define MAX_DESCRIPTION_LENGTH			127
#define MAX_SCREEN_NAME_LENGTH			71
#define MAX_THREAD_NAME_LENGTH			17
#define OLD_THREAD_NAME_LENGTH			5
#define NLM_SIGNATURE					"NetWare Loadable Module\x01a"
#define NLM_VERSION						4
#define VERSION_SIGNATURE				"VeRsIoN#"
#define VERSION_SIGNATURE_LENGTH		8
#define COPYRIGHT_SIGNATURE				"CoPyRiGhT="
#define COPYRIGHT_SIGNATURE_LENGTH		10
#define MAX_COPYRIGHT_LENGTH			255
#define EXTENDED_NLM_SIGNATURE			"MeSsAgEs"
#define EXTENDED_NLM_SIGNATURE_LENGTH	(sizeof(EXTENDED_NLM_SIGNATURE)-1)
#define MSG_FILE_SIGNATURE				"NetWare Message File: "
#define MSG_FILE_SIGNATURE_LENGTH		22
#define LANGUAGE_ID_OFFSET				106
#define NUM_MESSAGES_OFFSET				110
/*
//	Internal module name must 8.3 maximum
*/
#define NOV_MAX_NAME_LEN				8
#define NOV_MAX_EXT_LEN					3
#define NOV_MAX_MODNAME_LEN				(NOV_MAX_NAME_LEN + NOV_MAX_EXT_LEN + 1)

/* the fields marked vbl are variable length. */

/* the first "fixed chunk" of the header */

typedef struct nlm_header
{
    char            signature[sizeof(NLM_SIGNATURE) - 1];
    uint32_t     version;
    char            moduleName[14];     /* 1st character is a length byte.*/
    uint32_t     codeImageOffset;
    uint32_t     codeImageSize;
    uint32_t     dataImageOffset;
    uint32_t     dataImageSize;
    uint32_t     uninitializedDataSize;
    uint32_t     customDataOffset;
    uint32_t     customDataSize;
    uint32_t     moduleDependencyOffset;
    uint32_t     numberOfModuleDependencies;
    uint32_t     relocationFixupOffset;
    uint32_t     numberOfRelocationFixups;
    uint32_t     externalReferencesOffset;
    uint32_t     numberOfExternalReferences;
    uint32_t     publicsOffset;
    uint32_t     numberOfPublics;
    uint32_t     debugInfoOffset;
    uint32_t     numberOfDebugRecords;
    uint32_t     codeStartOffset;
    uint32_t     exitProcedureOffset;
    uint32_t     checkUnloadProcedureOffset;
    uint32_t     moduleType;
    uint32_t     flags;
    uint8_t      descriptionLength;
} nlm_header;

/* variable length part of the header  */

typedef struct nlm_header_2
{
    char            descriptionText[MAX_DESCRIPTION_LENGTH + 1]; /*vbl*/
    uint32_t     stackSize;
    uint32_t     reserved;
    char            oldThreadName[OLD_THREAD_NAME_LENGTH];
    uint8_t      screenNameLength;
    char            screenName[MAX_SCREEN_NAME_LENGTH + 1];   /*vbl*/
    uint8_t      threadNameLength;
    char            threadName[MAX_THREAD_NAME_LENGTH + 1];       /*vbl*/
} nlm_header_2;

/* the second "fixed chunk" of the header */

typedef struct nlm_header_3
{
    char            versionSignature[VERSION_SIGNATURE_LENGTH];
    uint32_t     majorVersion;
    uint32_t     minorVersion;
    uint32_t     revision;
    uint32_t     year;
    uint32_t     month;      /* starts at 1 */
    uint32_t     day;
} nlm_header_3;

/* remaining header fields */

typedef struct nlm_header_4
{
    char            copyrightSignature[COPYRIGHT_SIGNATURE_LENGTH];
    uint8_t      copyrightLength;
    char            copyrightString[MAX_COPYRIGHT_LENGTH + 1];    /*vbl*/
} nlm_header_4;

typedef struct
{
    uint8_t  stamp[EXTENDED_NLM_SIGNATURE_LENGTH];
    uint32_t languageID;
    uint32_t messageFileOffset;
    uint32_t messageFileLength;
    uint32_t messageCount;
    uint32_t helpFileOffset;
    uint32_t helpFileLength;
    uint32_t RPCDataOffset;
    uint32_t RPCDataLength;
    uint32_t sharedCodeOffset;
    uint32_t sharedCodeLength;
    uint32_t sharedDataOffset;
    uint32_t sharedDataLength;
    uint32_t sharedRelocationFixupOffset;
    uint32_t sharedRelocationFixupCount;
    uint32_t sharedExternalReferenceOffset;
    uint32_t sharedExternalReferenceCount;
    uint32_t sharedPublicsOffset;
    uint32_t sharedPublicsCount;
    uint32_t sharedDebugRecordOffset;
    uint32_t sharedDebugRecordCount;
    uint32_t sharedInitializationOffset;
    uint32_t sharedExitProcedureOffset;
    uint32_t productID;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t reserved4;
    uint32_t reserved5;
} extended_nlm_header;

/* values for the flags field above. */
#define NOV_REENTRANT           0x0001
#define NOV_MULTIPLE            0x0002
#define NOV_SYNCHRONIZE         0x0004
#define NOV_PSEUDOPREEMPTION    0x0008
#define NOV_OS_DOMAIN           0x0010		/*	16	*/
#define NOV_AUTOUNLOAD          0x0040		/*	64	*/

#define DBG_DATA 0
#define DBG_CODE 1

typedef struct nov_dbg_info
{
    uint8_t          type;
    uint32_t         offset;
    uint8_t          namelen;
} nov_dbg_info;

#define DEFAULT_PRELUDE_FN_CLIB	"_Prelude"
#define DEFAULT_EXIT_FN_CLIB	"_Stop"

#define DEFAULT_PRELUDE_FN_LIBC	"_LibCPrelude"
#define DEFAULT_EXIT_FN_LIBC	"_LibCPostlude"

/* values for upper bits of the relocation offsets. */
#define NOV_IMP_NONRELATIVE		0x80000000
#define NOV_IMP_ISCODE			0x40000000

#define NOV_EXP_ISCODE			0x80000000

#include "poppack.h"

