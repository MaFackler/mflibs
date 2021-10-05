#ifndef MF_H
#define MF_H
/* USAGE

// StretchyBuffer
// ==================================================

int *arr = NULL;
mf_stretchy_push(arr, 1);
mf_stretchy_push(arr, 2);

for (int i = 0; i < mf_stretchy_size(arr); ++i)
{
    printf("%d\n", arr[i]);
}

mf_stretchy_for(int*, ele, arr)
{
    printf("%d\n", *ele);
}

mf_stretchy_clear(arr);
mf_stretchy_destroy(arr);
*/


#ifdef __cplusplus
#define mf_inline inline

#else
#include <stdbool.h>
#define _POSIX_C_SOURCE 199309L
#define mf_inline
#endif

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define MF_WINDOWS
#include <windows.h>
#define MF_MAIN int WinMain(HINSTANCE __hInstance, HINSTANCE __hPrevInstance, LPSTR __lpCmdLine, int __nShowCmd)
#else
#define MF_MAIN int main(int __argc, char **__argv)
#define MF_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


typedef float f32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef char *mf_str;
typedef const char *mf_cstr;


#define MF_KiloByte(value) (1024 * value)
#define MF_MegaByte(value) (1024 * (MF_KiloByte(value)))

#define MF_Realloc(ptr, size) realloc(ptr, size)
#define MF_Malloc(size) malloc(size)
#define MF_MallocA(size) _malloca(size)
#define MF_Free free
#define MF_Calloc(size) calloc(size, 1);


#define MF_Assert(expr) if (!(expr)) {*(int *) 0 = 0; }
#define MF_AssertInvalid() MF_Assert(!"InvalidCodePath")
#define MF_AssertSuccessOnZero(func) if (func) MF_AssertInvalid()
#define MF_AssertSuccessOnNonZero(func) if (!func) MF_AssertInvalid()
#define MF_ArrayLength(arr) (sizeof(arr) / sizeof(arr[0]))
#define MF_ArrayZeroOut(arr) (memset(&arr[0], 0, sizeof(arr[0]) * MF_ArrayLength(arr)))
#define MF_Min(a,b) (((a)<(b))?(a):(b))
#define MF_Max(a,b) (((a)>(b))?(a):(b))

#define MF_MallocStruct(s) ((s *) MF_Malloc(sizeof(s)))
#define MF_MallocArray(n, s) ((s *) MF_Malloc(n * sizeof(s)))


// TODO: i would like to use it like
// char *buf = FormatStringN(
#define MF_FormatStringN(varName, N, format, ...) \
    char varName[N] = {}; \
    sprintf(&varName[0], format, __VA_ARGS__)

#define MF_FormatString(varName, format, ...) \
    MF_FormatStringN(varName, 1024, format, __VA_ARGS__)

#define MF_PATH_SEPARATOR_WINDOWS '\\'
#define MF_PATH_SEPARATOR_UNIX '/'
#ifdef MF_WINDOWS
#define MF_PATH_SEPARATOR MF_PATH_SEPARATOR_WINDOWS
#else
#define MF_PATH_SEPARATOR MF_PATH_SEPARATOR_UNIX
#endif // MF_WINDOWS


// general
void mf_sleep_ms(int value);
void mf_print(const char *fmt, ...);

// String utilities
// TODO: rename to mf_str...
mf_str mf_str_new(size_t n);
void mf_str_free(mf_str a);
bool mf_str_endswith(mf_cstr a, mf_cstr b);
bool mf_str_is_substr(mf_cstr a, mf_cstr b);
bool mf_str_is_equal(mf_cstr a, mf_cstr b);
bool mf_str_is_empty(const mf_str a);
bool mf_str_is_fuzzy(mf_cstr a, mf_cstr b);
void mf_str_append(mf_str *a, mf_cstr b);
void mf_str_nappend(mf_str a, int n, ...);
mf_str mf_str_subdup(mf_str a, size_t start, size_t end);
mf_str mf_str_subfree(mf_str a, size_t start, size_t end);
u32 mf_str_count_char(mf_cstr a, char c);
size_t mf_strlen(mf_cstr a);

mf_str mf_strdup(mf_cstr a);

// Path, File
char* mf_read_file(const char *path, const char *mode, u32 *size);
char* mf_path_join_create(const char *a, const char *b, char separator);
void mf_file_copy(const char *src, const char *dest);
bool mf_is_file(const char *filename);
u64 mf_get_last_write_time(const char *filename);


typedef struct mf_path_item mf_path_item;
typedef struct mf_directory mf_directory;
bool mf_directory_open(mf_directory *dir, const char* name, bool recursive);
bool mf_directory_next(mf_directory *dir, mf_path_item *item);
void mf_directory_close(mf_directory *dir);
bool mf_path_item_is_directory(mf_path_item *item);
bool mf_path_item_is_file(mf_path_item *item);

const mf_str* mf_readdir(const char *directory) {
    return NULL;
}

// Thread
typedef struct mf_thread_context mf_thread_context;
void mf_thread_run(mf_thread_context *tc);

// Random
u32 mf_random_int(u32 min, u32 max);


#ifdef MF_IMPLEMENTATION

void mf_sleep_ms(int value)
{
#ifdef _WIN32
    Sleep(value);
#else
    usleep(value);
#endif
}


void mf_print(const char *fmt, ...)
{
    char buffer[1024] = {};
    va_list args; 
    va_start(args, fmt);
    vsprintf_s(buffer, fmt, args);
#ifdef MF_WINDOWS
    OutputDebugString(buffer);
    OutputDebugString("\n");
#else
    printf(buffer);
    printf("\n");
#endif
    va_end(args);
}

// String utilities

mf_str mf_str_new(size_t n)
{
    mf_str res = (mf_str) MF_Calloc(n);
    return res;
}

void mf_str_free(mf_str a)
{
    if (a)
        MF_Free(a);
}

bool mf_str_endswith(mf_cstr a, mf_cstr b)
{
    size_t aSize = strlen(a);
    size_t bSize = strlen(b);

    bool res = strncmp(a + aSize - bSize, b, bSize) == 0;
    return res;
}

bool mf_str_is_substr(mf_cstr a, mf_cstr b)
{
    bool res = false;

    size_t sizeA = strlen(a);
    size_t sizeB = strlen(b);

    if (sizeB < sizeA)
    {
        for (size_t i = 0; i <= sizeA - sizeB; ++i)
        {
            size_t j;
            for (j = 0; j < sizeB; ++j)
            {
                if (a[i + j] != b[j])
                    break;

            }
            if (j == sizeB)
            {
                res = true;
                break;
            }

        }
    }
    return res;
}

bool mf_str_is_empty(const mf_str a)
{
    if (a == NULL)
        return false;
    return mf_strlen(a) == 0;
}

bool mf_str_is_fuzzy(mf_cstr a, mf_cstr b)
{
    size_t indexB = 0;
    bool caseSensitive = false;
    size_t sizeA = mf_strlen(a);
    size_t sizeB = mf_strlen(b);
    if (sizeB == 0)
        return true;
    for (size_t indexA = 0; indexA < sizeA; ++indexA) {
        char ca = a[indexA];
        char cb = b[indexB];

        caseSensitive = caseSensitive || isupper(cb);
        if (!caseSensitive) {
            ca = tolower(ca);
            cb = tolower(cb);
        }
        if (ca == cb) {
            indexB++;
            if (indexB == sizeB)
                break;
        }
    }
    return indexB == sizeB;
}

void mf_str_append(mf_str *a, mf_cstr b)
{
    u32 sizeA = mf_strlen(*a);
    u32 sizeB = mf_strlen(b);
    mf_str neww = (mf_str) MF_Realloc(*a, sizeA + sizeB + 1);
    for (size_t i = 0; i < mf_strlen(b); ++i)
    {
        neww[sizeA + i] = b[i];
    }
    neww[sizeA + sizeB] = 0;
    *a = neww;
}

void mf_str_nappend(mf_str a, int n, ...)
{
    va_list ap;

    va_start(ap, a);
    for (size_t i = 0; i < n; ++i)
    {
        mf_cstr value = va_arg(ap, mf_cstr);
        mf_str_append(&a, value);
    }
    va_end(ap);
}

mf_str mf_str_subdup(mf_str a, size_t start, size_t end)
{
    size_t n = end - start;
    mf_str res = mf_str_new(n + 1);
    for (size_t i = 0; i < n; ++i)
    {
        res[i] = a[start + i];
    }
    res[n + 1] = 0;
    return res;
}

mf_str mf_str_subfree(mf_str a, size_t start, size_t end)
{
    mf_str res = mf_str_subdup(a, start, end);
    mf_str_free(a);
    return res;
}

bool mf_string_is_equal(char *a, const char *b)
{
    bool res = strcmp(a, b) == 0;
    return res;
}

u32 mf_str_count_char(mf_cstr a, char c)
{
    u32 res = 0;
    while(*a)
    {
        if (*a++ == c)
            res++;
    }
    return res;
}

size_t mf_strlen(mf_cstr a)
{
    size_t res = 0;
    if (a != NULL)
        res = strlen(a);
    return res;
}

mf_str mf_strdup(mf_cstr a)
{
    mf_str res = strdup(a);
    return res;
}


// File io

char* mf_read_file(const char *path, const char *mode, u32 *size)
{
    FILE *file = fopen(path, mode);
    u32 bytesToRead = 0;
    if (strcmp(mode, "r") == 0)
    {
        // NOTE: if in text mode ignore \r
        char c = 0;
        do
        {
            c = getc(file);
            if (c != '\r')
                bytesToRead++;
        }
        while(c != EOF);
        bytesToRead--;
        // TODO: why do i have to subtract one?
    }
    else
    {
        fseek(file, 0, SEEK_END);
        bytesToRead = ftell(file);
    }

    if (size)
        *size = bytesToRead;
    fseek(file, 0, SEEK_SET);
    char *res = (char *) malloc(bytesToRead);

    while (bytesToRead > 0)
    {
        u32 readBytes = fread(res, 1, bytesToRead, file);
        bytesToRead -= readBytes;
    }
    return res;
}


char* mf_path_join_create(const char *a, const char *b, char separator)
{
    char *res = (char *) MF_Malloc(strlen(a) + strlen(b) + 2);
    sprintf(res, "%s%c%s", a, separator, b);
    return res;
}

void mf_file_copy(const char *src, const char *dest)
{
#ifdef MF_WINDOWS
    i32 res = CopyFile(src, dest, 0);
    MF_Assert(res != 0);
#else
    struct stat st;
    MF_AssertSuccessOnZero(stat(src, &st));
    i32 fdSrc = open(src, O_RDONLY);
    MF_Assert(fdSrc >= 0);
    i32 fdDest = open(dest, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode);
    MF_Assert(fdDest >= 0);
    char buf[MF_KiloByte(8)];

    while (1)
    {
        size_t bytesRead = read(fdSrc, &buf[0], sizeof(buf));
        if (!bytesRead)
            break;
        size_t bytesWrote = write(fdDest, &buf[0], bytesRead);
        MF_Assert(bytesRead == bytesWrote);
    }

    close(fdSrc);
    close(fdDest);
    //MF_AssertSuccessOnZero(chmod(dest, st.st_mode));
#endif
}

bool mf_is_file(const char *filename)
{
    bool res = false;
#ifdef MF_WINDOWS
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(filename, &data);
    res = handle != INVALID_HANDLE_VALUE;
    if (res && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        res = false;
    }
#else
    res = access(filename, F_OK) == 0;
    if (res)
    {
        // TODO: dirty hack to check if its a directory
        DIR *d;
        d = opendir(filename);
        struct dirent *dire;
        if (d)
        {
            dire = readdir(d);
            if (dire && dire->d_type != DT_REG)
            {
                res = false;
            }
        }
    }
#endif
    return res;
}

u64 mf_get_last_write_time(const char *filename)
{
    u64 res = 0;
#ifdef MF_WINDOWS
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    {
        res = data.ftLastWriteTime.dwHighDateTime;
        res = (res << 32);
        res += data.ftLastWriteTime.dwLowDateTime;
    }
#else
    struct stat st = {};
    MF_Assert(stat(filename, &st) == 0);
    res = st.st_mtime;
#endif
    return res;
}

enum MF_PathItemType
{
    PATH_ITEM_UNKNOWN,
    PATH_ITEM_FILE,
    PATH_ITEM_DIRECTORY,
};


struct mf_path_item
{
    char *name;
    enum MF_PathItemType type;
};

struct mf_directory
{

#ifdef MF_WINDOWS
    HANDLE handle;
    WIN32_FIND_DATAA data;
    bool firstOne;
#else
    DIR *d;
#endif
};




bool mf_directory_open(mf_directory *dir, const char *name, bool recursive)
{
    bool res = false;
#ifdef MF_WINDOWS
    MF_FormatString(buffer, "%s\\*.*", name);
    dir->handle = FindFirstFile(buffer, &dir->data);
    res = dir->handle != INVALID_HANDLE_VALUE;
    dir->firstOne = true;
#else
    dir->d = opendir(name);
    res = dir->d != NULL;
#endif
    return res;
}

bool mf_directory_next(mf_directory *dir, mf_path_item *item)
{
    bool res = false;
#ifdef MF_WINDOWS
    if (dir->firstOne)
    {
        dir->firstOne = false;
        res = dir->handle != INVALID_HANDLE_VALUE;
    }
    else
    {
        res = FindNextFileA(dir->handle, &dir->data);
    }
    if (res)

    {
        item->name = dir->data.cFileName;
        item->type = PATH_ITEM_FILE;
        if (dir->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            item->type = PATH_ITEM_DIRECTORY;
        }
    }
#else
    struct dirent *dire;
    dire = readdir(dir->d);
    if (dire)
    {
        item->name = dire->d_name;
        switch (dire->d_type)
        {
            case DT_DIR:
            {
                item->type = PATH_ITEM_DIRECTORY;
            } break;
            case DT_REG:
            {
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

void mf_directory_close(mf_directory *dir)
{
#ifdef MF_WINDOWS
    FindClose(dir->handle);
#else
    closedir(dir->d);
#endif
}

inline
bool mf_path_item_is_directory(mf_path_item *item)
{
    return item->type == PATH_ITEM_DIRECTORY;
}

inline
bool mf_path_item_is_file(mf_path_item *item)
{
    return item->type == PATH_ITEM_FILE;
}

// Threads
typedef void (*mf_thread_proc)(void *param);

struct mf_thread_context
{
    mf_thread_proc proc;
    void *arg;
};


#ifdef MF_WINDOWS
DWORD mf__thread_proc_impl(LPVOID param)
{

    mf_thread_context *threadContext = (mf_thread_context *) param;
    threadContext->proc(threadContext->arg);

    return 0;
}
#endif

void mf_thread_run(mf_thread_context *tc)
{
#ifdef MF_WINDOWS 
    DWORD id = 0;
    HANDLE thread = CreateThread(NULL,
                                 0,
                                 mf__thread_proc_impl,
                                 (LPVOID) tc,
                                 0,
                                 &id);
#endif

}


static bool mf__random_inizialized = false;
void mf__random_initialize()
{
#ifdef MF_WINDOWS
#else
    if (!mf__random_inizialized)
    {
        srand(time(NULL));
        mf__random_inizialized = true;
    }
#endif
}

u32 mf_random_int(u32 min, u32 max)
{
    mf__random_initialize();
    u32 res = rand() % (max + 1 - min) + min;
    return res;
}


// Stretchy buffer

typedef struct
{
    u32 size;
    u32 capacity;
} mf__stretchy_header;

#define mf_stretchy_for(t, e, v) \
	for (t e = v; e < (v) + mf_stretchy_size(v); ++e)


#define mf__get_stretchy_header(v) ((mf__stretchy_header *)((char *) v - sizeof(mf__stretchy_header)))
#define mf_stretchy_size(v) ((v) ? mf__get_stretchy_header(v)->size : 0)
#define mf__stretchy_capacity(v) ((v) ? mf__get_stretchy_header(v)->capacity : 0)
#define mf__stretchy_full(v) (mf_stretchy_size(v) == mf__stretchy_capacity(v))
#define mf_stretchy_clear(v) ((v) ? (mf__get_stretchy_header(v)->size = 0) : 0)
#define mf_stretchy_destroy(v) ((v) ? (free(mf__get_stretchy_header(v)), (v) = NULL) : 0)


#define mf_stretchy_push(v, e) \
	mf__stretchy_full(v) ? mf__stretchy_grow((void **) &v, sizeof(*(v))) : 0, \
	(v)[mf__get_stretchy_header(v)->size++] = (e)

inline int mf__stretchy_grow(void **v, size_t elementSize)
{
    size_t newCapacity = (mf_stretchy_size(*v) + 1) * 2;
    size_t bytesToAlloc = sizeof(mf__stretchy_header) + newCapacity * elementSize;
    mf__stretchy_header *header;

    if (*v)
    {
        header = (mf__stretchy_header *) realloc(mf__get_stretchy_header(*v), bytesToAlloc);
    }
    else
    {
        header = (mf__stretchy_header *) malloc(bytesToAlloc);
        header->size = 0;
    }
    header->capacity = newCapacity;
    *v  = (char *) header + sizeof(mf__stretchy_header);
	return 0;
}

#ifdef __cplusplus

template <typename T>
struct Vector
{
    size_t size = 0;
    size_t capacity = 0;
    T* data = NULL;

    Vector();
    Vector(size_t n);

    T& operator[](size_t n);
    void _grow(size_t size);

    // iterator
    T* begin() { return &data[0]; };
    T* end() { return &data[size]; };
};

template <typename T>
void DestroyWithContent(Vector<T> *vec);
template <typename T>
void Destroy(Vector<T> *vec);

template <typename T>
Vector<T>::Vector()
{
}

template <typename T>
Vector<T>::Vector(size_t n)
    :Vector()
{
    this->_grow(n);
}


template <typename T>
T& Vector<T>::operator[](size_t n)
{
    return this->data[n];
}

template <typename T>
void DestroyWithContent(Vector<T> *vec)
{
    for (size_t i = 0; i < vec->size; ++i)
    {
        Destroy(&vec->data[i]);
    }
    Destroy(vec);
}

template <typename T>
void Destroy(Vector<T> *vec)
{
    if (vec->size)
    {
        free(vec->data);
    }
    vec->size = 0;
    vec->capacity = 0;
    vec->data = NULL;
}

template <typename T>
void Push(Vector<T> *vec, T element)
{
    if (vec->size + 1 > vec->capacity)
    {
        vec->_grow((vec->size + 1) * 2);
    }
    vec->data[vec->size++] = element;
}

template <typename T>
void Vector<T>::_grow(size_t size)
{
    size_t bytesToAlloc = size * sizeof(T);

    if (this->data)
    {
        this->data = (T *) realloc(this->data, bytesToAlloc);
    }
    else
    {
        this->data = (T *) malloc(bytesToAlloc);
        this->size = 0;
    }
    this->capacity = size;
}

template <typename T>
bool VectorContains(Vector<T> *vec, T ele)
{
    bool res = false;
    for (size_t i = 0; i < vec->size; ++i)
    {
        T value = vec->data[i];
        if (ele == value)
        {
            res = true;
            break;
        }
    }
    return res;
}


#endif

#endif // MF_IMPLEMENTATION

#endif // MF_H
