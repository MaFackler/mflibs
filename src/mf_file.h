#ifndef MF_FILE_H
#define MF_FILE_H
#if defined(WIN32) || defined(_WIN32)
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

// Allocators
#ifndef mff_malloc
    #define mff_malloc malloc
#endif

#define MF_PATH_SEPARATOR_WINDOWS '\\'
#define MF_PATH_SEPARATOR_UNIX '/'

#if defined(WIN32) || defined(_WIN32)
#define MF_PATH_SEPARATOR MF_PATH_SEPARATOR_WINDOWS
#else
#define MF_PATH_SEPARATOR MF_PATH_SEPARATOR_UNIX
#endif

char* mff_file_read(const char *path, const char *mode, size_t *size);
char* mf_path_join_create(const char *a, const char *b, char separator);
void mf_file_copy(const char *src, const char *dest);
bool mf_is_file(const char *filename);
uint64_t mf_get_last_write_time(const char *filename);

typedef struct mf_path_item mf_path_item;
typedef struct mf_directory mf_directory;
bool mf_directory_open(mf_directory *dir, const char* name, bool recursive);
bool mf_directory_next(mf_directory *dir, mf_path_item *item);
void mf_directory_close(mf_directory *dir);
bool mf_path_item_is_directory(mf_path_item *item);
bool mf_path_item_is_file(mf_path_item *item);

#ifdef MF_FILE_IMPLEMENTATION

char* mff_file_read(const char *path, const char *mode, size_t *size) {
    FILE *file = fopen(path, mode);
    size_t bytesToRead = 0;
    fseek(file, 0, SEEK_END);
    bytesToRead = ftell(file);

    if (size)
        *size = bytesToRead;

    fseek(file, 0, SEEK_SET);
    char *res = (char *) mff_malloc(bytesToRead);
    while (bytesToRead > 0) {
        u32 readBytes = fread(res, 1, bytesToRead, file);
        bytesToRead -= readBytes;
    }
    return res;
}

char* mf_path_join_create(const char *a, const char *b, char separator) {
    char *res = (char *) mff_malloc(strlen(a) + strlen(b) + 2);
    sprintf(res, "%s%c%s", a, separator, b);
    return res;
}

void mf_file_copy(const char *src, const char *dest) {
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
    //MF_AssertSuccessOnZero(chmod(dest, st.st_mode));
#endif
}

bool mf_is_file(const char *filename) {
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

uint64_t mf_get_last_write_time(const char *filename) {
    uint64_t res = 0;
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

enum MF_PathItemType {
    PATH_ITEM_UNKNOWN,
    PATH_ITEM_FILE,
    PATH_ITEM_DIRECTORY,
};


struct mf_path_item {
    char *name;
    enum MF_PathItemType type;
};

struct mf_directory {

#ifdef WIN32
    HANDLE handle;
    WIN32_FIND_DATAA data;
    bool firstOne;
#else
    DIR *d;
#endif
};




bool mf_directory_open(mf_directory *dir, const char *name, bool recursive) {
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

bool mf_directory_next(mf_directory *dir, mf_path_item *item) {
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

void mf_directory_close(mf_directory *dir) {
#ifdef WIN32
    FindClose(dir->handle);
#else
    closedir(dir->d);
#endif
}

inline
bool mf_path_item_is_directory(mf_path_item *item) {
    return item->type == PATH_ITEM_DIRECTORY;
}

inline
bool mf_path_item_is_file(mf_path_item *item) {
    return item->type == PATH_ITEM_FILE;
}
#endif


#endif // MF_FILE_H

