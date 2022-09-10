#ifndef MF_H
#define MF_H


#ifdef __cplusplus
    #define mf_inline inline
#else
    #include <stdbool.h>
    #define _POSIX_C_SOURCE 199309L
    #define mf_inline
#endif

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #define MF_MAIN int WinMain(HINSTANCE __hInstance, HINSTANCE __hPrevInstance, LPSTR __lpCmdLine, int __nShowCmd)
#else
    #include <unistd.h> 
    #define MF_MAIN int main(int __argc, char **__argv)
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif // _GNU_SOURCE
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


// general
void mf_sleep_ms(int value);
void mf_print(const char *fmt, ...);

u64 mf_next_power_of_2(u64 a);



// Thread
typedef struct mf_thread_context mf_thread_context;
void mf_thread_run(mf_thread_context *tc);

// Random
u32 mf_random_int(u32 min, u32 max);
double mf_random_double(double min, double max);



void mf_sleep_ms(int value)
{
#ifdef _WIN32
    Sleep(value);
#else
    usleep(value * 1000);
#endif
}


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

u64 mf_next_power_of_2(u64 a) {
    u64 n = a + 1;
    u64 res = 0;
    while (res == 0) {
        if ((n & (n - 1)) == 0) {
            res = n;
        }
        n++;
    }
    return res;
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

double mf_random_double(double min, double max)
{
    double res = rand() / (RAND_MAX + 1.0);
    res = min + (max - min) * res;
    return res;
}



#endif // MF_H
