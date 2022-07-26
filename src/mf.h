#ifndef MF_H
#define MF_H


#ifdef __cplusplus
#else
    #include <stdbool.h>
    #define _POSIX_C_SOURCE 199309L
    #define mf_inline
#endif

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define MF_OS_WINDOWS
    #include <windows.h>
#elif defined(__linux)
    #include <unistd.h> 
    #define MF_OS_LINUX
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif // _GNU_SOURCE
#else
#error "Unknown operating system"
#endif // _WIN32

#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

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


#if defined(MF_TEST_ACTIVE)

TEST("MF_Max") {
    MFT_CHECK_INT(MF_Max(-1, 10), 10);
    MFT_CHECK_INT(MF_Max(1, 10), 10);
}

TEST("MF_Min") {
    MFT_CHECK_INT(MF_Min(-1, 10), -1);
    MFT_CHECK_INT(MF_Min(1, 10), 1);
}

#endif

#endif // MF_H
