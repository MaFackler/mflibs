#pragma once
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




void mf_print(const char *fmt, ...)
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

#endif

