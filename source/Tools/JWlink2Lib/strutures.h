#pragma once

#include <string>

using namespace std;

typedef enum _ExeFormat
{
    MK_OS2_NE = 0x00000001,
    MK_OS2_LE = 0x00000002,
    MK_OS2_LX = 0x00000004,
    MK_WIN_NE = 0x00000008,      /* Win16 (was: MK_WINDOWS) */
    MK_PE = 0x00000010,
    MK_DOS_EXE = 0x00000020,
    MK_COM = 0x00000040,
    MK_OVERLAYS = 0x00000080,
    MK_NOVELL = 0x00000100,
    MK_QNX_16 = 0x00000200,     // POSIX QNX, not ICON QNX
    MK_PHAR_SIMPLE = 0x00000400,
    MK_PHAR_FLAT = 0x00000800,
    MK_PHAR_REX = 0x00001000,
    MK_PHAR_MULTISEG = 0x00002000,
    MK_QNX_FLAT = 0x00004000,
    MK_ELF = 0x00008000,
    MK_WIN_VXD = 0x00010000,
    MK_DOS16M = 0x00020000,
    MK_RAW = 0x00080000
} ExeFormat;

typedef enum _CommandFlag
{
    CF_TO_STDOUT = 0x00000001,
    CF_SET_SECTION = 0x00000002,   // used for LIB/FIXEDLIB directives
    CF_NO_DEF_LIBS = 0x00000004,
    CF_FILES_BEFORE_DBI = 0x00000008,
    CF_UNNAMED = 0x00000010,
    CF_AUTO_SEG_FLAG = 0x00000020,   // used in CMDOS2
    CF_MEMBER_ADDED = 0x00000040,
    CF_SEPARATE_SYM = 0x00000080,
    CF_AUTOSECTION = 0x00000100,
    CF_SECTION_THERE = 0x00000200,
    CF_HAVE_FILES = 0x00000400,
    CF_HAVE_REALBREAK = 0x00000800,
    CF_LANGUAGE_MASK = 0x00003000,
    CF_LANGUAGE_ENGLISH = 0x00000000,
    CF_LANGUAGE_JAPANESE = 0x00001000,
    CF_LANGUAGE_CHINESE = 0x00002000,
    CF_LANGUAGE_KOREAN = 0x00003000,
    CF_ANON_EXPORT = 0x00004000,
    CF_AFTER_INC = 0x00008000,  // option must be specd. after op inc
    CF_DOING_OPTLIB = 0x00010000,
    CF_NO_EXTENSION = 0x00020000    // don't put an extension on exe name
} CommandFlag;

typedef struct _ParseEntry
{
    string keyword;
    bool (*rtn)(void);
    ExeFormat format;
    CommandFlag flags;
    bool operator<(const struct _ParseEntry& other) const {
        return keyword < other.keyword;
    }
} ParseEntry;

#define MK_DOS       (MK_OVERLAYS | MK_DOS_EXE | MK_COM)
#define MK_ONLY_OS2_16  MK_OS2_NE
#define MK_OS2_FLAT  (MK_OS2_LE | MK_OS2_LX | MK_WIN_VXD)
#define MK_ONLY_OS2  (MK_ONLY_OS2_16 | MK_OS2_LE | MK_OS2_LX)
#define MK_OS2_16BIT (MK_ONLY_OS2_16 | MK_WIN_NE)
/* MK_WIN_VXD is not included into MK_OS2 */
#define MK_OS2       (MK_OS2_16BIT | MK_OS2_LE | MK_OS2_LX)
#define MK_PHAR_LAP  (MK_PHAR_SIMPLE|MK_PHAR_FLAT|MK_PHAR_REX|MK_PHAR_MULTISEG)
#define MK_QNX       (MK_QNX_16 | MK_QNX_FLAT)
#define MK_386       (MK_PHAR_LAP | MK_NOVELL | MK_QNX|MK_OS2_LE|MK_OS2_LX|MK_PE|MK_ELF|MK_WIN_VXD|MK_RAW)
#define MK_286       (MK_DOS | MK_OS2_16BIT | MK_DOS16M)
/* MK_OS2_LE, MK_OS2_LX, MK_WIN_VXD and MK_PE are not treated as FLAT internally */
#define MK_FLAT      (MK_PHAR_SIMPLE | MK_PHAR_FLAT | MK_PHAR_REX | MK_RAW )
#define MK_ALLOW_64  (MK_PE | MK_ELF | MK_RAW) /* jwlink */
#define MK_ALLOW_32  (MK_PHAR_LAP|MK_OS2_LE|MK_OS2_LX|MK_NOVELL|MK_QNX|MK_PE|MK_ELF|MK_WIN_VXD|MK_RAW)
#define MK_ALLOW_16  (MK_286 | MK_PHAR_FLAT | MK_OS2 | MK_QNX | MK_PE | MK_WIN_VXD | MK_RAW)
#define MK_ID_SPLIT  (MK_NOVELL)
#define MK_REAL_MODE (MK_DOS)
#define MK_PROT_MODE (~MK_REAL_MODE)
#define MK_SEGMENTED (MK_286 | MK_OS2 | MK_PHAR_MULTISEG)
#define MK_IMPORTS   (MK_NOVELL | MK_OS2 | MK_PE | MK_ELF)
/* MK_SPLIT_DATA allows to split the uninitialized data part from
 * the rest of DGROUP, but it also prohibits BSS to share pages
 * with preceding initialized data segments!
 * To be improved!
 */
#define MK_SPLIT_DATA (MK_ELF | MK_PE)
#define MK_LINEARIZE (MK_ELF | MK_PE)
#define MK_END_PAD   (MK_DOS)
#define MK_ALL       ((ExeFormat)(0x000FFFFF))
