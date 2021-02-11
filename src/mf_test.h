#ifndef MF_TEST_H
#define MF_TEST_H
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef MFT_AMOUNT_TESTCASES
#define MFT_AMOUNT_TESTCASES 1024
#endif

typedef uint32_t u32;
typedef int32_t i32;

/* USAGE

#include "mf_test.h"

#include "test_other_stuff.cpp"

TEST("Example")
{
    MFT_CHECK(1 == 1);
    int a = 2;
    int *aa = &a;
    int *ab = &a;
    MFT_CHECK_PTR(aa, ab);
    MFT_CHECK_FLOAT(1.0f, 1.00000001f);
    MFT_CHECK_FLOAT('a', 'a');
    MFT_CHECK_FLOAT(1, 1);
}


TEST("STRING")
{
    char buffer[256];
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';
    MFT_CHECK_STRN(&buffer[0], "abc", 3);
}


int main()
{
    mft_run();
}

*/

i32 mft_run();


typedef struct mft__test_case mft__test_case;
typedef struct mft__test_collection mft__test_collection;

struct mft__test_case
{
    const char *name;
    const char *file;
    i32 line;
    void (*func)(mft__test_case *testCase);
    bool failure;
    char *output;

};

struct mft__test_collection
{
    mft__test_case testCases[MFT_AMOUNT_TESTCASES];
    u32 index;
};

static mft__test_collection __mft_collection = {};


// TEST CASE DEFINITION
#define TEST_CASE__(name, number) void test_func##number(mft__test_case *testCase); \
   mft__test_case *test_##number = mft__test_case_push(&__mft_collection, name, &test_func##number, __FILE__, __LINE__); \
   void test_func##number(mft__test_case *testCase)
#define TEST_CASE_(name, number) TEST_CASE__(name, number)
#define TEST(name) TEST_CASE_(name, __COUNTER__)


// TEST CHECKS
#define MFT_CHECK(condition) mft__check(testCase, __LINE__, condition)
#define MFT_CHECK_PTR(a, b) mft__check_ptr(testCase, __LINE__, a, b)
#define MFT_CHECK_FLOAT(a, b) mft__check_float(testCase, __LINE__, a, b)
#define MFT_CHECK_CHAR(a, b) mft__check_char(testCase, __LINE__, a, b)
#define MFT_CHECK_INT(a, b) mft__check_int(testCase, __LINE__, a, b)
#define MFT_CHECK_STRN(a, b, c) mft__check_strn(testCase, __LINE__, a, b, c)




mft__test_case *mft__test_case_push(mft__test_collection *collection, const char *name, void (*func)(mft__test_case *testCase), const char *file, i32 line)
{

    assert(collection->index < MFT_AMOUNT_TESTCASES);
    mft__test_case *res = &collection->testCases[collection->index++];
    res->name = name;
    res->func = func;
    res->file = file;
    res->line = line;
    return res;
}

void mft__test_case_destroy(mft__test_case *tc)
{
    if (tc->output)
        free(tc->output);
}

char* mft__strdup(const char *s)
{
    size_t len = strlen(s);
    char *res = (char *) malloc(len + 1);
    memcpy(res, s, len);
    res[len] = 0;
    return res;
}

inline void mft__on_check_error(mft__test_case *testCase, i32 lineNr, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    FILE *fp = fopen(testCase->file, "r");
    testCase->line = lineNr;
    testCase->failure = true;
    char line[256];
    char outbuffer[1024];
    char msgbuffer[1024];
    vsprintf(msgbuffer, fmt, args);
    i32 i = 0;
    while (fgets(line, sizeof(line), fp))
    {
        i++;
        if (i == lineNr)
        {
            line[strlen(line) - 1] = 0;  // remove new line
            sprintf(&outbuffer[0], "%s\t%s\n", line, msgbuffer);
            if (testCase->output)
            {
                char buffer[1024] = {};
                sprintf(&buffer[0], "%s%s", testCase->output, &outbuffer[0]);
                testCase->output = mft__strdup(&buffer[0]);
            }
            else
            {
                testCase->output = mft__strdup(&outbuffer[0]);
            }
            break;
        }
    }
    va_end(args);
    fclose(fp);
}

inline void mft__check(mft__test_case *testCase, u32 line, bool condition)
{
    if (!condition)
        mft__on_check_error(testCase, line, "Is not true");
}

inline
void mft__check_ptr(mft__test_case *testCase, u32 line, void *ptr1, void *ptr2)
{
    if (ptr1 != ptr2)
        mft__on_check_error(testCase, line, "Pointer differs");
}

inline
void mft__check_float(mft__test_case *testCase, u32 line, float actual, float expected)
{
    char buffer[1024];
    sprintf(&buffer[0], "%f != %f", actual, expected);
    // TODO whats a good precision
    if (abs(actual - expected) > 0.001f )
        mft__on_check_error(testCase, line, &buffer[0]);
}

inline void mft__check_char(mft__test_case *testCase, u32 line, char actual, char expected)
{
    if (actual != expected)
        mft__on_check_error(testCase, line, "%c != %c", actual, expected);
}


inline void mft__check_int(mft__test_case *testCase, u32 line, i32 actual, i32 expected)
{
    if (actual != expected)
        mft__on_check_error(testCase, line, "i32 %d != %d", actual, expected);
}

inline void mft__check_strn(mft__test_case *testCase, u32 line, char *actual, const char *expected, u32 size)
{
    if (actual == NULL || expected == NULL)
        mft__on_check_error(testCase, line, "STRN actual or expected is NULL");
    else
    {
        for (u32 i = 0; i < size; ++i)
        {
            char a = actual[i];
            char e = expected[i];
            if (a != e)
            {
                if (a == '\n')
                    a = ' ';
                if (e == '\n')
                    e = ' ';
                mft__on_check_error(testCase, line, "%.*s != %.*s at index %d", size, actual, size, expected, i);
                break;
            }
        }
    }
}


#define _RESULT_TEXT(failure) (failure ? "FAILURE" : "SUCCESS")

enum mft__terminal_color
{
    MFT_COLOR_DARK_BLUE=1,
    MFT_COLOR_DARK_GREEN,
    MFT_COLOR_DARK_CYAN,
    MFT_COLOR_DARK_RED,
    MFT_COLOR_DARK_MAGENTA,
    MFT_COLOR_DARK_YELLOW,
    MFT_COLOR_DARK_WHITE,
    MFT_COLOR_DARK_GRAY,

    MFT_COLOR_LIGHT_BLUE,
    MFT_COLOR_LIGHT_GREEN,
    MFT_COLOR_LIGHT_CYAN,
    MFT_COLOR_LIGHT_RED,
    MFT_COLOR_LIGHT_MAGENTA,
    MFT_COLOR_LIGHT_YELLOW,
    MFT_COLOR_LIGHT_WHITE,
    MFT_COLOR_COUNT,

};

void mft__set_color(enum mft__terminal_color color)
{
#ifdef WIN32 
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (int)color);
#else

    // NOTE: +1 because enum starts at one
    static u32 colorValues[MFT_COLOR_COUNT + 1] = {
        0,  // dummy
        34, // TERMINAL_COLOR_DARK_BLUE
        32, // TERMINAL_COLOR_DARK_GREEN
        36, // TERMINAL_COLOR_DARK_CYAN
        31, // TERMINAL_COLOR_DARK_RED
        35, // TERMINAL_COLOR_DARK_MAGENTA
        33, // TERMINAL_COLOR_DARK_YELLOW
        37, // TERMINAL_COLOR_DARK_WHITE
        37, // TERMINAL_COLOR_DARK_GRAY TODO ???

        94, // TERMINAL_COLOR_LIGHT_BLUE
        92, // TERMINAL_COLOR_LIGHT_GREEN
        96, // TERMINAL_COLOR_LIGHT_CYAN
        91, // TERMINAL_COLOR_LIGHT_RED
        95, // TERMINAL_COLOR_LIGHT_MAGENTA
        93, // TERMINAL_COLOR_LIGHT_YELLOW
        97, // TERMINAL_COLOR_LIGHT_WHITE
    };

    if (color == MFT_COLOR_DARK_WHITE)
    {
        printf("\033[0m");
    }
    else
    {
        u32 value = colorValues[color];
        printf("\033[1;%dm", value);
    }
#endif


}

void mft__reset_color()
{
    mft__set_color(MFT_COLOR_DARK_WHITE);
}


i32 mft_run()
{
    bool collectionFailure = false;

    printf("\n=============================================\n");
    printf("Found %zu TestCases\n", __mft_collection.index);
    printf("=============================================\n\n");

    for (size_t i = 0; i < __mft_collection.index; ++i)
    {
        mft__test_case *item = &__mft_collection.testCases[i];
        item->func(item);

        printf("%s:%d: '%s'  ", item->file, item->line, item->name);
        mft__set_color(item->failure ? MFT_COLOR_LIGHT_RED : MFT_COLOR_LIGHT_GREEN);
        printf("%s\n", _RESULT_TEXT(item->failure));
        if (item->failure)
        {
            printf("%s\n", item->output);
        }
        mft__reset_color();

        collectionFailure |= item->failure;

        mft__test_case_destroy(item);
    }

    mft__set_color(collectionFailure ? MFT_COLOR_LIGHT_RED : MFT_COLOR_LIGHT_GREEN);
    printf("\n================ %s =================\n", _RESULT_TEXT(collectionFailure));
    mft__reset_color();

    return collectionFailure;
}

#undef _RESULT_TEXT

#endif // MF_TEST_H
