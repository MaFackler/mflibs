#ifndef MF_FILE_H
#define MF_FILE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#if defined(WIN32) || defined(_WIN32)
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

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
bool mff_is_file(const char *filename);
// -- }}}

// -- {{{ Path
char* mff_path_join_create(const char *a, const char *b, char separator);
// -- }}}

// -- {{{ File
char* mff_file_read(const char *path, const char *mode, size_t *size);
void mff_file_copy(const char *src, const char *dest);
long unsigned int mff_file_get_last_write_time(const char *filename);
// -- }}}

// -- {{{ Directory
typedef struct MFF_PathItem MFF_PathItem;
typedef struct MFF_Directory MFF_Directory;
bool mff_directory_open(MFF_Directory *dir, const char* name, bool recursive);
bool mff_directory_next(MFF_Directory *dir, MFF_PathItem *item);
void mff_directory_close(MFF_Directory *dir);
bool mff_path_item_is_directory(MFF_PathItem *item);
bool mff_path_item_is_file(MFF_PathItem *item);
// -- }}}

// }}}

#if defined(MF_FILE_IMPLEMENTATION) | defined(MF_IMPLEMENTATION)

// {{{ Definitions

// -- {{{ General
bool mff_is_file(const char *filename) {
    bool res = false;
#ifdef WIN32
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(filename, &data);
    res = handle != INVALID_HANDLE_VALUE;
    if (res && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        res = false;
    }
#else
    res = access(filename, F_OK) == 0;
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
#endif
    return res;
}
// -- }}}

// -- {{{ Path
char* mff_path_join_create(const char *a, const char *b, char separator) {
    char *res = (char *) malloc(strlen(a) + strlen(b) + 2);
    sprintf(res, "%s%c%s", a, separator, b);
    return res;
}
// -- }}}

// -- {{{ File
char* mff_file_read(const char *path, const char *mode, size_t *size) {
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

void mff_file_copy(const char *src, const char *dest) {
#ifdef WIN32
    int res = CopyFile(src, dest, 0);
    assert(res != 0);
#else
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
#endif
}


long unsigned int mff_file_get_last_write_time(const char *filename) {
    long unsigned int res = 0;
#ifdef WIN32
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data)) {
        res = data.ftLastWriteTime.dwHighDateTime;
        res = (res << 32);
        res += data.ftLastWriteTime.dwLowDateTime;
    }
#else
    struct stat st = {};
    assert(stat(filename, &st) == 0);
    res = st.st_mtime;
#endif
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




bool mff_directory_open(MFF_Directory *dir, const char *name, bool recursive) {
    bool res = false;
#ifdef WIN32
    char buffer[256];
    sprintf(buffer, "%s\\*.*", name);
    dir->handle = FindFirstFile(buffer, &dir->data);
    res = dir->handle != INVALID_HANDLE_VALUE;
    dir->firstOne = true;
#else
    dir->d = opendir(name);
    res = dir->d != NULL;
#endif
    return res;
}

bool mff_directory_next(MFF_Directory *dir, MFF_PathItem *item) {
    bool res = false;
#ifdef WIN32
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
    }
#else
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

#endif
    return res;
}

void mff_directory_close(MFF_Directory *dir) {
#ifdef WIN32
    FindClose(dir->handle);
#else
    closedir(dir->d);
#endif
}

inline
bool mff_path_item_is_directory(MFF_PathItem *item) {
    return item->type == PATH_ITEM_DIRECTORY;
}

inline
bool mff_path_item_is_file(MFF_PathItem *item) {
    return item->type == PATH_ITEM_FILE;
}

// }}}

// }}}

#endif // MF_FILE_IMPLEMENTATION
#endif // MF_FILE_H

