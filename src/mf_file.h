#pragma once
#include <mf.h>

namespace mf {

#if defined(MF_OS_WINDOWS)
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif // MF_OS_

char* file_read(const char *path, const char *mode, size_t *size);
char* path_join_create(const char *a, const char *b, char separator);
void file_copy(const char *src, const char *dest);
bool is_file(const char *filename);
u64 get_last_write_time(const char *filename);

enum class PathItemType {
    UNKNOWN,
    FILE,
    DIRECTORY,
};

struct PathItem {
    char *name;
    PathItemType type;
};

bool path_item_is_directory(PathItem *item);
bool path_item_is_file(PathItem *item);


struct Directory {
#if defined(MF_OS_WINDOWS)
    HANDLE handle;
    WIN32_FIND_DATAA data;
    bool firstOne;
#elif defined(MF_OS_LINUX)
    DIR *d;
#else
#error MF_OS_NOT_SUPPORTED
#endif
};

bool directory_open(Directory *directory, const char* name, bool recursive);
void directory_close(Directory *directory);
bool directory_next(PathItem *item);


#if defined(MF_FILE_IMPLEMENTATION)

char* file_read(const char *path, const char *mode, size_t *size) {
    FILE *file = fopen(path, mode);
    int bytesToRead = 0;
    fseek(file, 0, SEEK_END);
    bytesToRead = ftell(file);
    if (strcmp(mode, "r") == 0) {
        MF_AssertInvalid();
    }

    if (size != NULL)
        *size = bytesToRead;

    fseek(file, 0, SEEK_SET);
    char *res = (char *) malloc(bytesToRead);
    while (bytesToRead > 0) {
        u32 readBytes = fread(res, 1, bytesToRead, file);
        bytesToRead -= readBytes;
    }
    return res;
}

char* path_join_create(const char *a, const char *b, char separator) {
    char *res = (char *) malloc(strlen(a) + strlen(b) + 2);
    sprintf(res, "%s%c%s", a, separator, b);
    return res;
}

void file_copy(const char *src, const char *dest) {
#if defined(MF_OS_WINDOWS)
    int res = CopyFile(src, dest, 0);
    assert(res != 0);
#elif defined(MF_OS_LINUX)
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

bool is_file(const char *filename) {
    bool res = false;
#if defined(MF_OS_WINDOWS)
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(filename, &data);
    res = handle != INVALID_HANDLE_VALUE;
    if (res && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        res = false;
    }
#elif defined(MF_OS_LINUX)
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
#else
#error "Not supported on this platform"
#endif
    return res;
}

uint64_t get_last_write_time(const char *filename) {
    uint64_t res = 0;
#if defined(MF_OS_WINDOWS)
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data)) {
        res = data.ftLastWriteTime.dwHighDateTime;
        res = (res << 32);
        res += data.ftLastWriteTime.dwLowDateTime;
    }
#elif defined(MF_OS_LINUX)
    struct stat st = {};
    assert(stat(filename, &st) == 0);
    res = st.st_mtime;
#else
#error "Not supported on this os"
#endif
    return res;
}

bool directory_open(Directory *directory, const char *name, bool recursive) {
    bool res = false;
#if defined(MF_OS_WINDOWS)
    char buffer[256];
    sprintf(buffer, "%s\\*.*", name);
    directory->handle = FindFirstFile(buffer, &directory->data);
    res = directory->handle != INVALID_HANDLE_VALUE;
    directory->firstOne = true;
#elif defined(MF_OS_LINUX)
    directory->d = opendir(name);
    res = directory->d != NULL;
#else
#error MF_OS_NOT_SUPPORTED
#endif
    return res;
}

bool directory_next(Directory *directory, PathItem *item) {
    bool res = false;
#if defined(MF_OS_WINDOWS)
    if (directory->firstOne) {
        directory->firstOne = false;
        res = directory->handle != INVALID_HANDLE_VALUE;
    } else {
        res = FindNextFileA(directory->handle, &directory->data);
    }
    if (res) {
        item->name = directory->data.cFileName;
        item->type = PathItemType::FILE;
        if (directory->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            item->type = PathItemType::DIRECTORY;
        }
    }
#elif defined(MF_OS_LINUX)
    struct dirent *dire;
    dire = readdir(this->d);
    while (dire != NULL && strcmp(dire->d_name, ".") == 0
           || dire != NULL && strcmp(dire->d_name, "..") == 0) {
        dire = readdir(this->d);
    }
    if (dire) {
        if (strcmp(dire->d_name, "..") == 0) {
            dire = readdir(this->d);
        }
        item->name = dire->d_name;
        switch (dire->d_type) {
            case DT_DIR: {
                item->type = PathItemType::DIRECTORY;
            } break;
            case DT_REG: {
                item->type = PathItemType::FILE;
            } break;
            default:
                item->type =PathItemType::UNKNOWN;
        }
        res = true;
    }
#else
#error MF_OS_NOT_SUPPORTED
#endif
    return res;
}

void directory_close(Directory *directory) {
#if defined(MF_OS_WINDOWS)
    FindClose(directory->handle);
#elif defined(MF_OS_LINUX)
    closedir(directory->d);
#else
#error MF_OS_NOT_SUPPORTED
#endif
}

inline
bool path_item_is_directory(PathItem *item) {
    return item->type == PathItemType::DIRECTORY;
}

inline
bool path_item_is_file(PathItem *item) {
    return item->type == PathItemType::FILE;
}

       
#if defined(MF_TEST_ACTIVE)

TEST("read - empty file") {
    size_t size = 0;
    char *contents = file_read("tests/data/testfile_empty.txt", "rb", &size);
    MFT_CHECK_SIZET(size, 0);
}

TEST("read") {
    size_t size = 0;
    char *contents = file_read("tests/data/testfile.txt", "rb", &size);
#if defined(MF_OS_WINDOWS)
    MFT_CHECK_SIZET(size, 5);
    MFT_CHECK_STRINGN(contents, "abc", 3);
    MFT_CHECK_INT(contents[3], '\r');
    MFT_CHECK_INT(contents[4], '\n');
#elif defined(MF_OS_LINUX)
    MFT_CHECK_SIZET(size, 4);
    MFT_CHECK_STRINGN(contents, "abc", 3);
    MFT_CHECK_INT(contents[3], '\n');
#endif
}

TEST("path_join_create") {
#if defined(MF_OS_LINUX)
    char *res = path_join_create("home", "mf", '/');
    MFT_CHECK_STRING(res, "home/mf");
    free(res);
#endif
}

TEST("is_file") {
    MFT_CHECK(is_file("./src/mf.h"));
    MFT_CHECK(is_file("src/mf.h"));
    MFT_CHECK(!is_file("src/mfnot.h"));
}

//TEST("mf_directory") {
//    mf_directory dir;
//    mf_path_item item;
//    mf_directory_open(&dir, "src", false);
//    int counter = 0;
//    while(mf_directory_next(&dir, &item)) {
//        counter++;
//    }
//    MFT_CHECK_INT(counter, 9);
//}

#endif // MF_TEST_ACTIVE

#endif // MF_FILE_IMPLEMENTATION

}
