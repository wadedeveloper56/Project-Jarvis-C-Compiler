#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned f_handle;
#define NIL_HANDLE -1

enum perms {// names for permissions
	none = 0,
	owner_read = 0400,  S_IRUSR=0400,
	owner_write = 0200, S_IWUSR=0200,
	owner_exec = 0100,  S_IXUSR=0100,
	owner_all = 0700,   S_IRWXU=0700,
	group_read = 040,   S_IRGRP=040,
	group_write = 020,  S_IWGRP=020,
	group_exec = 010,   S_IXGRP=010,
	group_all = 070,    S_IRWXG=070,
	others_read = 04,   S_IROTH=04,
	others_write = 02,  S_IWOTH=02,
	others_exec = 01,   S_IXOTH=010,
	others_all = 07,    S_IRWXO=07,
	all = 0777,
	set_uid = 04000,    S_ISUID=04000,
	set_gid = 02000,    S_ISGID=02000,
	sticky_bit = 01000, S_ISVTX=01000,
	mask = 07777,
	unknown = 0xFFFF,
	add_perms = 0x10000,
	remove_perms = 0x20000,
	resolve_symlinks = 0x40000
};

f_handle OpenFile2(const char* filename, int oflag, int pmode);
int CloseFile2(f_handle handle);
int FlushFile2(f_handle handle);
int ReadFile2(f_handle handle, void* const buffer, unsigned const buffer_size);
int WriteFile2(f_handle handle, void* const buffer, unsigned const buffer_size);
__int64 FileTell2(f_handle handle);
__int64 FileSeek2(f_handle handle, __int64 offset, int origin);
__int64 FileSize2(f_handle handle);

#ifdef __cplusplus
}
#endif
