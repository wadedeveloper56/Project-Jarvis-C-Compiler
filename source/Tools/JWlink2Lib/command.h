#pragma once

typedef enum
{
    MIDST,
    ENDOFLINE,
    ENDOFFILE,
    ENDOFCMD
}                       place;

typedef enum
{
    NONBUFFERED,
    COMMANDLINE,
    INTERACTIVE,
    BUFFERED,
    ENVIRONMENT,
    SYSTEM
}                       method;

typedef enum
{
    SEP_NO,
    SEP_COMMA,
    SEP_EQUALS,
    SEP_PERIOD,
    SEP_END,
    SEP_QUOTE,
    SEP_PAREN,
    SEP_SPACE,
    SEP_PERCENT,
    SEP_DOT_EXT,
    SEP_LCURLY,
    SEP_RCURLY
}                       sep_type;

typedef enum
{
    OK,
    REJECT
}                       status;

typedef enum
{
    ST_IS_ORDINAL,
    ST_NOT_ORDINAL,
    ST_INVALID_ORDINAL
} ord_state;

typedef struct tok
{
    char* buff;
    unsigned    len;
    char* next;
    char* this1;
    uint16_t line;
    char        quoted;     /* if token parsed as a quoted string*/
    uint8_t  where : 2;
    uint8_t  how : 3;
    uint8_t  thumb : 1;
    uint8_t  locked : 1;
    uint8_t  skipToNext : 1;   /* set true if we need to skip to next token without a separator */
} tok;

typedef enum commandflag
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
} commandflag;

typedef struct cmdfilelist
{
    struct cmdfilelist* prev;
    struct cmdfilelist* next;
    f_handle            file;
    char* symprefix;
    char* name;
    tok                 token;
} cmdfilelist;

typedef struct parse_entry
{
    char* keyword;
    bool                (*rtn)(void);
    enum exe_format     format;
    commandflag         flags;
} parse_entry;

typedef struct sysblock
{
    struct sysblock* next;
    char* name;
    char                commands[1];
} sysblock;

typedef enum
{
    TOK_INCLUDE_DOT = 0x01,
    TOK_IS_FILENAME = 0x02
} tokcontrol;
