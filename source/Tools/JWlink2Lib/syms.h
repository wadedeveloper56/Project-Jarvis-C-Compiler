#pragma once
typedef enum
{

    // These constants define action to take with undefined symbols

    WRITE_WARNS = 0,
    WRITE_IMPORTS = 1,

    // These constants define action to take with the symbol table

    ST_STATIC = 0x01,         // this is a static symbol
    ST_CREATE = 0x02,         // create the symbol if not found
    ST_NOALIAS = 0x04,         // don't expand aliases
    ST_FIND = 0x08,         // just looking for the symbol
    ST_REFERENCE = 0x10,         // mark symbol as referenced
    ST_DEFINE = 0x20,         // mark symbol as defined
    ST_DEFINE_SYM = 0x26,         // frequently used combo
    ST_DUPLICATE = 0x40          // jwlink: allow duplicate static syms
} sym_flags;

typedef enum
{

    // These constants define the meaning of the bits in the symbol "info" field
    // The values 0-15 are used for the type of the symbol
    // SYM_OLDHAT is only used in pass 1, and sym_checked is only used in pass 1.
    // SYM_MAP_GLOBAL is also used inbetween pass 1 and pass 2 to keep track of
    // symbols which are going in the map file (for sort global command)
    // SYM_KILL & SYM_RELOC_REFD only used before pass 1

    // Here is what is put in the first four bits of the info field.

    SYM_COMMUNAL_16 = 0,    // symbol is a 16-bit communal symbol
    SYM_COMMUNAL_32 = 1,    // symbol is a 32-bit communal symbol
    SYM_COMDAT = 2,    // symbol is an initialized communal.
    SYM_REGULAR = 3,    // a good, old-fashioned linker symbol
    SYM_ALIAS = 4,    // this is an alias for another symbol.
    SYM_IMPORTED = 5,    // symbol is imported (OS/2 & Novell).
    SYM_GROUP = 6,    // symbol is attached to a group
    SYM_LAZY_REF = 7,    // symbol is a "lazy" reference.
    SYM_WEAK_REF = 8,    // symbol is a "weak" reference.
    SYM_LINK_WEAK_REF = 9,    // symbol is a linker-generated weak reference
    SYM_VF_REF = 10,   // symbol is a virtual function table reference
    SYM_PURE_REF = 11,   // symbol is a pure virt. func. table reference

    // Here is all other values except the first four bits of the info field.

    SYM_DEAD = 0x00000010, // symbol has been eliminated.
    SYM_FREE_ALIAS = 0x00000010, // used for aliases only.
    SYM_OLDHAT = 0x00000020, // symbol referenced in a previous obj
    SYM_REFERENCED = 0x00000040, // used to mark unref'd symbols in map
    SYM_CHECKED = 0x00000080, // symbol has been checked by FindUndefined
    SYM_MAP_GLOBAL = 0x00000080, // put sym in global map symbol list
    SYM_STATIC = 0x00000100, // non-global symbol
    SYM_LOCAL_REF = 0x00000200, // symbol referenced locally.
    SYM_TRACE = 0x00000400, // keep track of references to this symbol
    SYM_IN_CURRENT = 0x00000800, // symbol seen in current module
    SYM_WAS_LAZY = 0x00000800, // used for aliases only
    SYM_DEFINED = 0x00001000, // symbol defined.
    SYM_ABSOLUTE = 0x00002000, // symbol is absolute
    SYM_EXPORTED = 0x00008000, // symbol has been exported
    SYM_CDAT_SEL_NODUP = 0x00000000, // do not allow duplicates
    SYM_CDAT_SEL_ANY = 0x00010000,
    SYM_CDAT_SEL_SIZE = 0x00020000,
    SYM_CDAT_SEL_EXACT = 0x00030000,
    SYM_CDAT_SEL_MASK = 0x00030000,
    SYM_CDAT_SEL_SHIFT = 16,
    SYM_RELOC_REFD = 0x00040000, // symbol referenced by a stored reloc
    SYM_KILL = 0x00080000, // remove symbol
    SYM_IS_ALTDEF = 0x00100000, // symbol is an altdef symbol.
    SYM_FAR_COMMUNAL = 0x00200000, // is a far communal symbol
    SYM_LINK_GEN = 0x00400000, // is a linker generated symbol
    SYM_HAS_DATA = 0x00800000, // only used for altdefs
    SYM_NAME_XLATED = 0x01000000, // only used during permdata writing
    SYM_IS_FREE = 0x10000000, // only used during permdata writing.

    SYM_CLEAR_ON_P2 = 0xE00000A0, // bits to clear before pass 2 starts.
    SYM_CLEAR_ON_INC = 0x010404F0,
    SYM_CLEAR_ON_ALT = 0x00980010,

    // the top three bits are used for the floating point fixup type during pass 2
    // SYM_DCE_REF, SYM_VF_REFS_DONE and SYM_VF_MARKED are only needed during
    // pass 1 and before the floating point syms are checked for.
    // SYM_VF_MARKED can be used at the same time as SYM_DISTRIB since
    // SYM_DISTRIB is only set when SYM_DEFINED is on, and SYM_VF_MARKED is only
    // needed when the symbol is not defined.

    SYM_DISTRIB = 0x20000000, // DOS ONLY: symbol is in a distrib. lib.
    SYM_VF_MARKED = 0x20000000, // vf reference record seen for this sym.
    SYM_DCE_REF = 0x40000000, // referenced for the purposes of dead code
    SYM_VF_REFS_DONE = 0x80000000  // ALL: vf refs added to call graph
} sym_info;

// some handy macros for checking and setting symbol type bits

#define SYM_TYPE_MASK      0xF

#define IS_SYM_COMMUNAL(sym) (((sym)->info & SYM_TYPE_MASK) <= SYM_COMDAT)
#define IS_SYM_NICOMDEF(sym) (((sym)->info & SYM_TYPE_MASK) <= SYM_COMMUNAL_32)
#define IS_SYM_COMDAT(sym)   (((sym)->info & SYM_TYPE_MASK) == SYM_COMDAT)
#define IS_SYM_ALIAS(sym)    (((sym)->info & SYM_TYPE_MASK) == SYM_ALIAS)
#define IS_SYM_IMPORTED(sym) (((sym)->info & SYM_TYPE_MASK) == SYM_IMPORTED)
#define IS_SYM_GROUP(sym)    (((sym)->info & SYM_TYPE_MASK) == SYM_GROUP)
#define IS_SYM_REGULAR(sym)  (((sym)->info & SYM_TYPE_MASK) == SYM_REGULAR)
#define IS_SYM_VF_REF(sym)   (((sym)->info & SYM_TYPE_MASK) >= SYM_VF_REF)
#define IS_SYM_PURE_REF(sym) (((sym)->info & SYM_TYPE_MASK) == SYM_PURE_REF)
#define IS_SYM_LAZY_REF(sym) (((sym)->info & SYM_TYPE_MASK) == SYM_LAZY_REF)
#define IS_SYM_A_REF(sym)    (((sym)->info & SYM_TYPE_MASK) >= SYM_LAZY_REF)
#define IS_SYM_WEAK_REF(sym) (((sym)->info & SYM_TYPE_MASK) >= SYM_WEAK_REF)
#define IS_SYM_LINK_WEAK(sym)(((sym)->info & SYM_TYPE_MASK) ==SYM_LINK_WEAK_REF)
#define IS_SYM_COMM32(sym)   (((sym)->info & SYM_TYPE_MASK) == SYM_COMMUNAL_32)
#define SET_SYM_TYPE(t,sym,type) ((sym)->info = (t)(((sym)->info & ~SYM_TYPE_MASK) | (type)))
/* note that OVL_VECTOR && OVL_FORCE can be thought of as a two-bit field.
 * OVL_NO_VECTOR == 0 && OVL_FORCE == 0 means undecided.
 * OVL_NO_VECTOR == 1 && OVL_FORCE == 0 means tenatively no vector generated.
 * OVL_NO_VECTOR == 0 && OVL_FORCE == 1 means generate a vector
 * OVL_NO_VECTOR == 1 && OVL_FORCE == 1 means do not generate a vector
*/

enum overlay_info
{
    OVL_UNDECIDED = 0x0,
    OVL_NO_VECTOR = 0x1,          // symbol has a vector
    OVL_FORCE = 0x2,          // force symbol to have a vector
    OVL_ALWAYS = 0x4,          // always use vector
    OVL_REF = 0x8,          // reference number assigned
    OVL_MAKE_VECTOR = OVL_FORCE,
    OVL_VEC_MASK = (OVL_NO_VECTOR | OVL_FORCE)
};

// values used to keep track of the special floating point symbols.
enum
{
    FFIX_NOT_A_FLOAT = 0,
    FFIX_WR_SYMBOL,
    FFIX_DR_SYMBOL,
    FFIX_ES_OVERRIDE,
    FFIX_CS_OVERRIDE,
    FFIX_SS_OVERRIDE,
    FFIX_DS_OVERRIDE,
    FFIX_IGNORE,          // an overlapping fixup symbol.
    FFIX_MASK = 0xe0000000
};

#define FFIX_SHIFT 29
#define GET_FFIX_VALUE(sym)       ((sym)->info >> FFIX_SHIFT)
#define SET_FFIX_VALUE(t,sym,value) ((sym)->info = (t)(((sym)->info & ~FFIX_MASK) | ((value) << FFIX_SHIFT)))

typedef struct
{
    uint16_t modnum;         // DOS: idx of module which defines this sym
    uint16_t ovlref : 12;  // DOS: overlay vector #
    uint16_t ovlstate : 4;  // DOS: overlay vector state
} dos_sym_data;

typedef struct symbol
{
    struct symbol* hash;
    struct symbol* publink;
    struct symbol* link;
    targ_addr           addr;
    uint16_t         namelen_cmp;
    sym_info            info;       // flags & floating point fixup type.
    struct mod_entry* mod;
    union
    {
        void* edges;     // for dead code elim. when sym undefd
        struct segdata* seg;       // seg symbol is in.
        char* alias;     // for aliased syms.
        void* import;    // NOVELL & OS/2 only: imported symbol data.
        offset          cdefsize;   // altdef comdefs: size of comdef
    } p;
    union
    {
        dos_sym_data    d;
        struct symbol* altdefs;   // for keeping track of comdat & comdef defs
        struct symbol* datasym;   // altdef comdats: sym which has data def
        unsigned        aliaslen;   // for aliases - length of name.
    } u;
    union
    {
        struct symbol* mainsym;   // altdefs: main symbol definition
        struct symbol* def;       // for lazy externs
        struct symbol** vfdata;   // for virtual function lazy externs.
        void* export1;    // OS/2 & PE only: exported sym info.
    } e;
    char* name;
    char* prefix;    // primarily for netware, though could be
    // subverted for other use. gives symbol
    // namespace qualification
} symbol;
