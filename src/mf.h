#pragma once
/* USAGE

// Vector
// ==================================================

mf::Vector<int> vec;
Push(&vec, 1);
Push(&vec, 2);
for (int i: vec)
{
    printf("%d\n", i);
}

for (int i = 0; i < vec.size; ++i)
{
    printf("%d\n", vec.data[i]);
    printf("%d\n", vec[i]);
}
Destroy(&vec);

// StretchyBuffer
// ==================================================

int *arr = NULL;
MF_StretchyPush(arr, 1);
MF_StretchyPush(arr, 2);

for (int i = 0; i < MF_StretchySize(arr); ++i)
{
    printf("%d\n", arr[i]);
}

MF_StretchyFor(int*, ele, arr)
{
    printf("%d\n", *ele);
}

MF_StretchyClear(arr);
MF_StretchyDestroy(arr);
*/


#if defined(WIN32) || defined(_WIN32)
#define MF_WINDOWS
#include <windows.h>
#else
#define MF_LINUX
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#endif

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

namespace mf
{


#define MF_KiloByte(value) (1024 * value)
#define MF_MegaByte(value) (1024 * (MF_KiloByte(value)))

#define MF_Realloc(ptr, size) realloc(ptr, size)
#define MF_Malloc(size) malloc(size)
#define MF_MallocA(size) _malloca(size)
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



template <class T> void DestorySafe(T **obj)
{
    if (*obj)
    {
        (*obj)->destroy();
        (*obj) = NULL;
    }
}

void Print(const char *fmt, ...)
{
    char buffer[1024] = {};
    va_list args; 
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
#ifdef MF_WINDOWS
    OutputDebugString(buffer);
    OutputDebugString("\n");
#else
    printf(buffer);
    printf("\n");
#endif
    va_end(args);
}


// Stretchy buffer
struct StretchyHeader
{
    u32 size;
    u32 capacity;
};

#define MF_StretchyFor(t, e, v) \
	for (t e = v; e < (v) + MF_StretchySize(v); ++e)


#define getStretchyHeader(v) ((mf::StretchyHeader *)((char *) v - sizeof(mf::StretchyHeader)))
#define MF_StretchySize(v) ((v) ? getStretchyHeader(v)->size : 0)
#define MF_StretchyCapacity(v) ((v) ? getStretchyHeader(v)->capacity : 0)
#define MF_StretchyFull(v) (MF_StretchySize(v) == MF_StretchyCapacity(v))
#define MF_StretchyClear(v) ((v) ? (getStretchyHeader(v)->size = 0) : 0)
#define MF_StretchyDestroy(v) ((v) ? (free(getStretchyHeader(v)), (v) = NULL) : 0)


#define MF_StretchyPush(v, e) \
	MF_StretchyFull(v) ? mf::_StretchyGrow((void **) &v, sizeof(*(v))) : 0, \
	(v)[getStretchyHeader(v)->size++] = (e)

inline int _StretchyGrow(void **v, size_t elementSize)
{
    size_t newCapacity = (MF_StretchySize(*v) + 1) * 2;
    size_t bytesToAlloc = sizeof(StretchyHeader) + newCapacity * elementSize;
    StretchyHeader *header;

    if (*v)
    {
        header = (StretchyHeader *) realloc(getStretchyHeader(*v), bytesToAlloc);
    }
    else
    {
        header = (StretchyHeader *) malloc(bytesToAlloc);
        header->size = 0;
    }
    header->capacity = newCapacity;
    *v  = (char *) header + sizeof(StretchyHeader);
	return 0;
}


// InternString


struct InternString
{
    size_t size;
    char *data;
};

static InternString *internStrings;

char* InternStringCreate(char *start, char *end)
{
    u32 size = end - start;
    MF_StretchyFor(InternString*, string, internStrings)
    {
        if (string->size == size && strncmp(string->data, start, size) == 0)
        {
            return string->data;
        }
    }
    char *string = (char *) malloc(size + 1);
    memcpy(string, start, size);
    string[size] = 0;
    MF_StretchyPush(internStrings, (InternString{size, string}));
    return string;
}

char* InternStringCreate(char *string)
{
    return InternStringCreate(string, string + strlen(string));
}

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

}

