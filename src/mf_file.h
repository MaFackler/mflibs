#ifndef MF_FILE_H
#define MF_FILE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#define FUNC_CROSS
#define FUNC_WIN32
#define FUNC_LINUX

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#define API static inline

// {{{ Macros
#define MF_PATH_SEPARATOR_WINDOWS '\\'
#define MF_PATH_SEPARATOR_UNIX '/'

#if defined(WIN32) || defined(_WIN32)
#define MF_PATH_SEPARATOR MF_PATH_SEPARATOR_WINDOWS
#else
#define MF_PATH_SEPARATOR MF_PATH_SEPARATOR_UNIX
#endif
// }}}

// {{{ Declarations

// -- {{{ General
// TODO: rename to mf_filesystem
FUNC_CROSS API bool MFF_Exists(const char *target);
FUNC_CROSS API bool MFF_IsFile(const char *filename);
// -- }}}

// -- {{{ Path
API char* MFF_PathJoinCreate(const char *a, const char *b, char separator);
// -- }}}

// -- {{{ File
API char* MFF_FileRead(const char *path, const char *mode, size_t *size);
FUNC_CROSS API void MFF_FileCopy(const char *src, const char *dest);
FUNC_CROSS API uint64_t MFF_FileGetWriteTime(const char *filename);
// -- }}}

// -- {{{ Directory
typedef struct MFF_PathItem MFF_PathItem;
typedef struct MFF_Directory MFF_Directory;
FUNC_CROSS API bool MFF_DirectoryOpen(MFF_Directory *dir, const char* name, bool recursive);
FUNC_CROSS API bool MFF_DirectoryNext(MFF_Directory *dir, MFF_PathItem *item);
FUNC_CROSS API void MFF_DirectoryClose(MFF_Directory *dir);
API bool MFF_PathItemIsDirectory(MFF_PathItem *item);
API bool MFF_PathItemIsFile(MFF_PathItem *item);
// -- }}}

// }}}

#if defined(MF_FILE_IMPLEMENTATION) | defined(MF_IMPLEMENTATION)

// {{{ Definitions

// -- {{{ Path
API char* MFF_PathJoinCreate(const char *a, const char *b, char separator) {
    char *res = (char *) malloc(strlen(a) + strlen(b) + 2);
    sprintf(res, "%s%c%s", a, separator, b);
    return res;
}
// -- }}}

// -- {{{ File
API char* MFF_FileRead(const char *path, const char *mode, size_t *size) {
    FILE *file = fopen(path, mode);
    size_t bytesToRead = 0;
    fseek(file, 0, SEEK_END);
    bytesToRead = ftell(file);

    if (size)
        *size = bytesToRead;

    fseek(file, 0, SEEK_SET);
    char *res = (char *) malloc(bytesToRead);
    while (bytesToRead > 0) {
        size_t readBytes = fread(res, 1, bytesToRead, file);
        bytesToRead -= readBytes;
    }
    return res;
}


// -- }}}

// -- {{{ Directory
enum MF_PathItemType {
    PATH_ITEM_UNKNOWN,
    PATH_ITEM_FILE,
    PATH_ITEM_DIRECTORY,
};


struct MFF_PathItem {
    char *name;
    enum MF_PathItemType type;
};

struct MFF_Directory {
#ifdef WIN32
    HANDLE handle;
    WIN32_FIND_DATAA data;
    bool firstOne;
#else
    DIR *d;
#endif
};


API bool MFF_PathItemIsDirectory(MFF_PathItem *item) {
    return item->type == PATH_ITEM_DIRECTORY;
}

API bool MFF_PathItemIsFile(MFF_PathItem *item) {
    return item->type == PATH_ITEM_FILE;
}

// }}}

#ifdef WIN32

FUNC_WIN32 API bool MFF_Exists(const char *target) {
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(target, &data);
    bool res = handle != INVALID_HANDLE_VALUE;
    return res;
}

FUNC_WIN32 API bool MFF_IsFile(const char *filename) {
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(filename, &data);
    bool res = handle != INVALID_HANDLE_VALUE;
    if (res && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        res = false;
    }
    return res;
}

FUNC_WIN32 API void MFF_FileCopy(const char *src, const char *dest) {
    int res = CopyFile(src, dest, 0);
    assert(res != 0);
}

FUNC_WIN32 API uint64_t MFF_FileGetWriteTime(const char *filename) {
    uint64_t res = 0;
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data)) {
        res = data.ftLastWriteTime.dwHighDateTime;
        res = (res << 32);
        res = data.ftLastWriteTime.dwLowDateTime + res;
    }
    return res;
}

FUNC_WIN32 API bool MFF_DirectoryOpen(MFF_Directory *dir, const char *name, bool recursive) {
    bool res = false;
    char buffer[256];
    sprintf(buffer, "%s\\*.*", name);
    dir->handle = FindFirstFile(buffer, &dir->data);
    res = dir->handle != INVALID_HANDLE_VALUE;
    dir->firstOne = true;
    return res;
}

FUNC_WIN32 API bool MFF_DirectoryNext(MFF_Directory *dir, MFF_PathItem *item) {
    bool res = false;
    if (dir->firstOne) {
        dir->firstOne = false;
        res = dir->handle != INVALID_HANDLE_VALUE;
    } else {
        res = FindNextFileA(dir->handle, &dir->data);
    }
    if (res) {
        item->name = dir->data.cFileName;
        item->type = PATH_ITEM_FILE;
        if (dir->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            item->type = PATH_ITEM_DIRECTORY;
        }
        if (strcmp(item->name, ".") == 0 || strcmp(item->name, "..") == 0) {
            MFF_DirectoryNext(dir, item);
        }
    }
    return res;
}

FUNC_WIN32 API void MFF_DirectoryClose(MFF_Directory *dir) {
    FindClose(dir->handle);
}

#elif __linux__

FUNC_LINUX API bool MFF_Exists(const char *target) {
    bool res = access(target, F_OK) == 0;
    return res;
}

FUNC_LINUX API bool MFF_IsFile(const char *filename) {
    bool res = access(filename, F_OK) == 0;
    if (res) {
        // TODO: dirty hack to check if its a directory
        DIR *d;
        d = opendir(filename);
        struct dirent *dire;
        if (d) {
            dire = readdir(d);
            if (dire && dire->d_type != DT_REG) {
                res = false;
            }
        }
    }
    return res;
}

FUNC_LINUX API void MFF_FileCopy(const char *src, const char *dest) {
    struct stat st;
    assert(stat(src, &st) == 0);
    int fdSrc = open(src, O_RDONLY);
    assert(fdSrc >= 0);
    int fdDest = open(dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode);
    assert(fdDest >= 0);
    char buf[8 * 1024];

    while (1) {
        size_t bytesRead = read(fdSrc, &buf[0], sizeof(buf));
        if (!bytesRead)
            break;
        size_t bytesWrote = write(fdDest, &buf[0], bytesRead);
        assert(bytesRead == bytesWrote);
    }

    close(fdSrc);
    close(fdDest);
}

FUNC_LINUX API uint64_t MFF_FileGetWriteTime(const char *filename) {
    uint64_t res = 0;
    struct stat st = {};
    assert(stat(filename, &st) == 0);
    res = st.st_mtime;
    return res;
}


FUNC_LINUX API bool MFF_DirectoryOpen(MFF_Directory *dir, const char *name, bool recursive) {
    bool res = false;
    dir->d = opendir(name);
    res = dir->d != NULL;
    return res;
}

FUNC_LINUX API bool MFF_DirectoryNext(MFF_Directory *dir, MFF_PathItem *item) {
    bool res = false;
    struct dirent *dire;
    dire = readdir(dir->d);
    while (dire != NULL && strcmp(dire->d_name, ".") == 0
           || dire != NULL && strcmp(dire->d_name, "..") == 0) {
        dire = readdir(dir->d);
    }
    if (dire) {
        if (strcmp(dire->d_name, "..") == 0) {
            dire = readdir(dir->d);
        }
        item->name = dire->d_name;
        switch (dire->d_type) {
            case DT_DIR: {
                item->type = PATH_ITEM_DIRECTORY;
            } break;
            case DT_REG: {
                item->type = PATH_ITEM_FILE;
            } break;
            default:
                item->type =PATH_ITEM_UNKNOWN;
        }
        res = true;
    }
    return res;
}

FUNC_LINUX API void MFF_DirectoryClose(MFF_Directory *dir) {
    closedir(dir->d);
}

#endif

// }}}

#endif // MF_FILE_IMPLEMENTATION
#endif // MF_FILE_H

