#ifndef MF_H
#define MF_H


#include <stdbool.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #define MF_MAIN int WinMain(HINSTANCE __hInstance, HINSTANCE __hPrevInstance, LPSTR __lpCmdLine, int __nShowCmd)
#else
    // #define _POSIX_C_SOURCE 199309L
    // #define _DEFAULT_SOURCE
    #include <unistd.h> 
    #define MF_MAIN int main(int __argc, char **__argv)
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif // _GNU_SOURCE
#endif // _WIN32

#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

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

#define MF_KiloByte(value) (1024 * value)
#define MF_MegaByte(value) (1024 * (MF_KiloByte(value)))

#define MF_Realloc(ptr, n) realloc(ptr, n)
#define MF_Malloc(n) malloc(n)
#define MF_MallocA(n) _malloca(n)
#define MF_Free free
#define MF_Calloc(n) calloc(1, n)

#define MF_Assert(expr) if (!(expr)) {fprintf(stderr, "%s\n", #expr); *(int *) 0 = 0; }
#define MF_AssertInvalid() MF_Assert(!"InvalidCodePath")
#define MF_Error(msg, ...) fprintf(stderr, msg, __VA_ARGS__); *(int *) 0 = 0
#define MF_ErrorIfNull(obj, msg) if (obj == NULL) { MF_Error(msg); }
#define MF_ArrayLength(arr) (sizeof(arr) / sizeof(arr[0]))
#define MF_ArrayFor(arr) for (__typeof__(arr[0]) *it = &arr[0]; it != NULL && (it < (arr) + MF_ArrayLength(arr)); ++it)
#define MF_ArrayZeroOut(arr) (memset(&arr[0], 0, sizeof(arr[0]) * MF_ArrayLength(arr)))
#define MF_Min(a,b) (((a)<(b))?(a):(b))
#define MF_Max(a,b) (((a)>(b))?(a):(b))

#define MF_CallocStruct(s) ((s *) MF_Calloc(sizeof(s)))
#define MF_MallocStruct(s) ((s *) MF_Malloc(sizeof(s)))
#define MF_MallocArray(n, s) ((s *) MF_Malloc(n * sizeof(s)))

#define MF_MemberOffset(s, m) (int)((char *) &m - (char *) &s)

#ifdef MF_SHORT_NAMES

#define KiloByte MF_KiloByte
#define MegaByte MF_MegaByte

#define Assert MF_Assert
#define Malloc MF_Malloc
#define Calloc MF_Calloc
#define Realloc MF_Realloc
#define Free MF_Free
#define Min MF_Min
#define Max MF_Max

#define MallocStruct MF_MallocStruct
#define CallocStruct MF_CallocStruct
#define MallocArray MF_MallocArray
#endif

#endif // MF_H
