#include "pch.h"
#include "File.h"
#include "start.h"
#include "globals.h"
#include "ntio.h"

int      OpenFiles;      // the number of _open files
unsigned LastResult;
bool     CaughtBreak;    // set to TRUE if break hit.
infilelist* CachedLibFiles;
infilelist* CachedFiles;

void TrapBreak(int sig_num)
{
    CaughtBreak = true;
}

void CheckBreak(void)
{
#ifdef __OSI__
    if (*_BreakFlagPtr)
    {
        *_BreakFlagPtr = 0;
        LnkMsg(FTL + MSG_BREAK_HIT, NULL);    /* suicides */
    }
#else
    if (CaughtBreak)
    {
        CaughtBreak = false;        /* prevent recursion */
        //LnkMsg(FTL + MSG_BREAK_HIT, NULL);    /* suicides */
    }
#endif
}

void SetBreak(void)
{
}

void RestoreBreak(void)
{
}

void LnkFilesInit(void)
{
    OpenFiles = 0;
    CaughtBreak = false;
#if !defined( _DLLHOST )
    _setmode(STDIN_HANDLE, O_BINARY);
    _setmode(STDOUT_HANDLE, O_BINARY);
#endif
}

void PrintIOError(unsigned msg, char* types, char* name)
{
    //LnkMsg(msg, types, name, strerror(errno));
}

int DoOpen(char* name, unsigned mode, bool isexe)
{
    int     h;

    isexe = isexe;
    CheckBreak();
    mode |= O_BINARY;
    for (;; )
    {
        if (OpenFiles >= MAX_OPEN_FILES)
            CleanCachedHandles();
        h = _open(name, mode, S_IRUSR | S_IWUSR);
        if (h != -1)
        {
            OpenFiles++;
            break;
        }
        if (errno != TOOMANY)
            break;
        if (!CleanCachedHandles())
        {
            break;
        }
    }
    return(h);
}

f_handle QOpenR(char* name)
{
    int     h;

    h = DoOpen(name, O_RDONLY, FALSE);
    if (h != -1)
        return(h);
    //LnkMsg(FTL + MSG_CANT_OPEN, "12", name, strerror(errno));
    return(NIL_HANDLE);
}

f_handle QOpenRW(char* name)
{
    int     h;

    h = DoOpen(name, O_RDWR | O_CREAT | O_TRUNC, FALSE);
    if (h != -1)
        return(h);
    //LnkMsg(FTL + MSG_CANT_OPEN, "12", name, strerror(errno));
    return(NIL_HANDLE);
}

int ResOpen(const char* path, int access, ...)
{
    int     perm;

    perm = 0666;
    return(_open(path, access, perm));
}

f_handle ExeCreate(char* name)
{
    int     h;

    h = DoOpen(name, O_RDWR | O_CREAT | O_TRUNC, TRUE);
    if (h != -1)
        return(h);
    //LnkMsg(FTL + MSG_CANT_OPEN, "12", name, strerror(errno));
    return(NIL_HANDLE);
}

f_handle ExeOpen(char* name)
{
    int     h;

    h = DoOpen(name, O_RDWR, TRUE);
    if (h != -1)
        return(h);
    //LnkMsg(FTL + MSG_CANT_OPEN, "12", name, strerror(errno));
    return(NIL_HANDLE);
}

#define doread( f, b, l )  _read( f, b, l )
#define dowrite( f, b, l ) _write( f, b, l )

unsigned QRead(f_handle file, void* buffer, unsigned len, char* name)
{
    int     h;

    CheckBreak();
    h = doread(file, buffer, len);
    if (h == -1)
    {
        //LnkMsg(ERR + MSG_IO_PROBLEM, "12", name, strerror(errno));
    }
    return(h);
}

unsigned QWrite(f_handle file, void* buffer, unsigned len, char* name)
{
    int     h;
    char    rc_buff[RESOURCE_MAX_SIZE];

    if (len == 0)
        return(0);

#ifdef _INT_DEBUG
    {
        unsigned long pos = QPos(file);
        if (pos <= SpyWrite && SpyWrite <= pos + len
            && file == Root->outfile->handle)
        {
            DEBUG((DBG_ALWAYS, "About to _write to %s (handle %d) %d bytes at position %d:",
                name, file, len, pos));
            PrintMemDump(buffer, len, DUMP_BYTE);
        }
    }
#endif

    CheckBreak();
    h = dowrite(file, buffer, len);
    if (name != NULL)
    {
        if (h == -1)
        {
            //LnkMsg(ERR + MSG_IO_PROBLEM, "12", name, strerror(errno));
        }
        else if (h != len)
        {
            //Msg_Get(MSG_IOERRLIST_7, rc_buff);
            //LnkMsg((FTL + MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff);
        }
    }
    return(h);
}

char NLSeq[] = { "\r\n" };

void QWriteNL(f_handle file, char* name)
{
    QWrite(file, NLSeq, sizeof(NLSeq) - 1, name);
}

void QClose(f_handle file, char* name)
{
    int         h;

    CheckBreak();
    h = _close(file);
    OpenFiles--;
    if (h != -1)
        return;
    //LnkMsg(ERR + MSG_IO_PROBLEM, "12", name, strerror(errno));
}

long QLSeek(f_handle file, long position, int start, char* name)
{
    long int    h;

    CheckBreak();
    h = _lseek(file, position, start);
    if (h == -1 && name != NULL)
    {
        //LnkMsg(ERR + MSG_IO_PROBLEM, "12", name, strerror(errno));
    }
    return(h);
}

void QSeek(f_handle file, long position, char* name)
{
    QLSeek(file, position, SEEK_SET, name);
}

unsigned long QPos(f_handle file)
{
    CheckBreak();
    return(_lseek(file, 0L, SEEK_CUR));
}

unsigned long QFileSize(f_handle file)
{
    long        result;

    result = _filelength(file);
    if (result == -1)
    {
        result = 0;
    }
    return result;
}

void QDelete(char* name)
{
    int   h;

    if (name == NULL)
        return;
    h = remove(name);
    if (h == -1 && errno != ENOENT)
    { /* file not found is OK */
        //LnkMsg(ERR + MSG_IO_PROBLEM, "12", name, strerror(errno));
    }
}

bool QReadStr(f_handle file, char* dest, unsigned size, char* name)
{
    bool            eof;
    char            ch;

    eof = FALSE;
    while (--size > 0)
    {
        if (QRead(file, &ch, 1, name) == 0)
        {
            eof = TRUE;
            break;
        }
        else if (ch != '\r')
        {
            *dest++ = ch;
        }
        if (ch == '\n')
        {
            break;
        }
    }
    *dest = '\0';
    return(eof);
}

bool QIsDevice(f_handle file)
{
    return(_isatty(file));
}

f_handle NSOpen(char* name, unsigned mode)
{
    int         h;

    h = DoOpen(name, mode, FALSE);
    LastResult = h;
    if (h != -1)
        return(h);
    return(NIL_HANDLE);
}

f_handle QObjOpen(char* name)
{
    return(NSOpen(name, O_RDONLY));
}

f_handle TempFileOpen(char* name)
{
    return(NSOpen(name, O_RDWR));
}

int QMakeFileName(char** pos, char* name, char* fname)
{
    char* pathptr;
    unsigned int        path_len;
    char* file_ptr;

    pathptr = *pos;
    if (pathptr == NULL)
        return(0);
    while (*pathptr != '\0')
    {
        if (*pathptr == PATH_LIST_SEP)
            *pos = ++pathptr;
        for (;;)
        {
            if (*pathptr == '\0')
                break;
            if (*pathptr == PATH_LIST_SEP)
                break;
            pathptr++;
        }
        path_len = pathptr - *pos;
        if (path_len != 0)
        {
            memcpy(fname, *pos, path_len);
            file_ptr = fname + path_len;
            switch (file_ptr[-1])
            {
            CASE_PATH_SEP:
                break;
                default:
                    *file_ptr++ = PATH_SEP;
                    break;
            }
            strcpy(file_ptr, name);
            *pos = pathptr;
            return(1);
        }
        *pos = pathptr;
    }
    return(0);
}

bool QHavePath(char* name)
{
    return(*name == '\\' || *name == '/' || *(name + 1) == ':');
}

bool QSysHelp(char** cmd_ptr)
{
    return(FALSE);
}

bool QModTime(char* name, time_t* time)
{
    int         result;
    struct stat buf;

    result = stat(name, &buf);
    *time = buf.st_mtime;
    return result != 0;
}

time_t QFModTime(int handle)
{
    struct stat buf;

    fstat(handle, &buf);
    return buf.st_mtime;
}

char WaitForKey(void)
{
    return getch();
}

int _bgetcmd(char* buffer, int len)
{
    int     total;
    int     i;
    char* word;
    char* p = NULL;
    char** argv = &_argv[1];

    --len; // reserve space for NULL byte

    if (buffer && (len > 0))
    {
        p = buffer;
        *p = '\0';
    }

    /* create approximation of original command line */
    for (word = *argv++, i = 0, total = 0; word; word = *argv++)
    {
        i = strlen(word);
        total += i;

        if (p)
        {
            if (i >= len)
            {
                strncpy(p, word, len);
                p[len] = '\0';
                p = NULL;
                len = 0;
            }
            else
            {
                strcpy(p, word);
                p += i;
                len -= i;
            }
        }
        /* account for at least one space separating arguments */
        if (*argv)
        {
            if (p)
            {
                *p++ = ' ';
                --len;
            }
            ++total;
        }
    }
    return(total);
}

char* getcmd(char* buffer)
{
    _bgetcmd(buffer, 1024);
    return(buffer);
}

void GetCmdLine(char* buff)
{
    getcmd(buff);
}

bool CleanCachedHandles(void)
{
    infilelist* list;

    for (list = CachedFiles; list != NULL; list = list->next)
    {
        if (!(list->flags & INSTAT_IN_USE) && list->handle != NIL_HANDLE)break;
    }
    if (list == NULL) return(FALSE);
    QClose(list->handle, list->name);
    list->handle = NIL_HANDLE;
    return(TRUE);
}

char* GetEnvString(char* envname)
{
    return(getenv(envname));
}

f_handle SearchPath(char* name)
{
    char* path;
    f_handle    file;
    char        fullpath[PATH_MAX];

    file = QObjOpen(name);
    if (file != NIL_HANDLE)
    {
        return(file);
    }
#if defined( __QNX__ )
    path = "/usr/watcom";
#else
    path = GetEnvString("PATH");
#endif
    if (path != NULL)
    {
        while (QMakeFileName(&path, name, fullpath))
        {
            file = QObjOpen(fullpath);
            if (file != NIL_HANDLE)
            {
                //DEBUG((DBG_OLD, "SearchPath: %s found in %s", name, fullpath));
                return(file);
            }
        }
    }
    //DEBUG((DBG_OLD, "SearchPath: %s not found", name));
    return(NIL_HANDLE);
}

void SetCommandFile(f_handle file, char* fname)
{
    unsigned long   size;
    char* buff;

    if (QIsDevice(file))
    {
        size = 0x10000;
    }
    else
    {
        size = QFileSize(file);
    }
    buff = NULL;
    if (size < 65510)
    {       // if can alloc a chunk big enough
		buff = (char*)malloc(size + 1);
        if (buff != NULL)
        {
            size = QRead(file, buff, size, fname);
            *(buff + size) = '\0';
            NewCommandSource(fname, buff, BUFFERED);
        }
    }
    if (buff == NULL)
    {  // if couldn't buffer for some reason.
		buff = (char*)malloc(MAX_REC + 1); // have to have at least this much RAM
        NewCommandSource(fname, buff, NONBUFFERED);
        Token.where = ENDOFLINE;
        Token.line++;
    }
    CmdFile->file = file;
}

