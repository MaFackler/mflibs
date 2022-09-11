#ifndef MF_TEST_H
#define MF_TEST_H
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <mf.h>
#define MF_STRING_IMPLEMENTATION
#include <mf_string.h>
#define MF_VECTOR_IMPLEMENTATION
#include <mf_vector.h>

#ifndef MFT_AMOUNT_TESTCASES
#define MFT_AMOUNT_TESTCASES 1024
#endif

#define MFT_FLOAT_DIFF 0.0001f
#define MFT_DOUBLE_DIFF 0.0001

/* USAGE

#include "mf_test.h"

#include "test_other_stuff.cpp"

TEST("Example") {
    CHECK(1 == 1);
    int a = 2;
    int *aa = &a;
    int *ab = &a;
    CHECK(aa, ab);
    CHECK(1.0f, 1.00000001f);
    CHECK('a', 'a');
    CHECK(1, 1);
}


TEST("STRING") {
    char buffer[256];
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';
    CHECK(&buffer[0], "abc", 3);
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
    i32 check_line;
    void (*func)(mft__test_case *testCase);
    bool failure;
    mf_str output;

};

struct mft__test_collection
{
    mft__test_case *test_cases = NULL;
    u32 current_item;
};

static mft__test_collection __mft_collection = {};

mft__test_case* mft_get_current_test_case() {
    return &__mft_collection.test_cases[__mft_collection.current_item];
}

inline void mft_test_error(const char *fmt, ...);

void mft_check(int a, int b);
void mft_check(bool cond);
void mft_check(float a, float b);
void mft_check(double a, double b);
void mft_check(const char *actual, const char *expected, size_t n);
void mft_check(const char *actual, const char *expected);

// NOTE: all compare/check functions
// Int32 
void mft_check(int a, int b) {
    if (a != b) {
        mft_test_error("Int32 failure %d != %d\n", a, b);
    }
}

// Bool
void mft_check(bool cond) {
    if (!cond) {
        mft_test_error("Is not True");
    }
}

// Float
void mft_check(float a, float b) {
    // TODO: good float compare
    if (abs(a - b) > MFT_FLOAT_DIFF) {
        mft_test_error("Float failure %f != %f\n", a, b);
    }
}

// Double
void mft_check(double a, double b) {
    // TODO: good float compare
    if (abs(a - b) > MFT_DOUBLE_DIFF) {
        mft_test_error("Double failure %f != %f\n", a, b);
    }
}

// String

bool mft__check_string_null(const char *actual, const char *expected) {
    if (actual == NULL && expected == NULL) {
        mft_test_error("String failure \"\" != \"\"");
        return true;
    } else if (actual == NULL) {
        mft_test_error("String failure \"\" != \"%s\"", expected);
        return true;
    } else if (expected == NULL) {
        mft_test_error("String failure \"%s\" != \"\"", actual);
        return true;
    }
    return false;
}

void mft__check_string(const char *actual, size_t actual_size, const char *expected, size_t expected_size) {
    if (actual_size != expected_size) {
        mft_test_error("%.*s (%d) != %.*s (%d) different length", actual_size, actual, actual_size, expected_size, expected, expected_size);
    }
    for (u32 i = 0; i < actual_size; ++i)
    {
        char a = actual[i];
        char e = expected[i];
        if (a != e) {
            if (a == '\n')
                a = ' ';
            if (e == '\n')
                e = ' ';
            mft_test_error("%.*s != %.*s at index %d",
                           actual_size, actual,
                           expected_size, expected,
                           i);
            break;
        }
    }
}

void mft_check(const char *actual, const char *expected, size_t n) {
    if (!mft__check_string_null(actual, expected)) {
        mft__check_string(actual, n, expected, n);
    }
}

void mft_check(const char *actual, const char *expected) {
    if (!mft__check_string_null(actual, expected)) {
        mft__check_string(actual, strlen(actual), expected, strlen(expected));
    }
}

#define CHECK(...) mft__check(__LINE__, __VA_ARGS__)

template <typename... Args> void mft__check(u64 line, Args... args) {
    auto testCase = mft_get_current_test_case();
    testCase->check_line = line;
    mft_check(args...);
}


// TEST CASE DEFINITION
#define TEST_CASE__(name, number) void test_func##number(mft__test_case *testCase); \
   mft__test_case *test_##number = mft__test_case_push(&__mft_collection, name, &test_func##number, __FILE__, __LINE__); \
   void test_func##number(mft__test_case *testCase)
#define TEST_CASE_(name, number) TEST_CASE__(name, number)
#define TEST(name) TEST_CASE_(name, __COUNTER__)


mft__test_case *mft__test_case_push(mft__test_collection *collection, const char *name, void (*func)(mft__test_case *testCase), const char *file, i32 line) {
    mft__test_case *res = mf_vec_add(collection->test_cases);
    res->name = name;
    res->func = func;
    res->file = file;
    res->line = line;
    res->output = mf_str_new();
    return res;
}

void mft__test_case_destroy(mft__test_case *tc) {
    if (tc->output)
        mf_str_free(tc->output);
}


inline void mft_test_error(const char *fmt, ...) {
    auto testCase = mft_get_current_test_case(); 

    va_list args;
    va_start(args, fmt);
    FILE *fp = fopen(testCase->file, "r");
    testCase->line = testCase->check_line;
    testCase->failure = true;
    char line[256];
    char outbuffer[1024];
    char msgbuffer[1024];
    vsprintf(msgbuffer, fmt, args);
    i32 i = 0;
    while (fgets(line, sizeof(line), fp)) {
        i++;
        if (i == testCase->check_line) {
            line[strlen(line) - 1] = 0;  // remove new line
            sprintf(&outbuffer[0], "%s\t%s\n", line, msgbuffer);

            mf_str_append(&testCase->output, &outbuffer[0]);
            break;
        }
    }
    va_end(args);
    fclose(fp);
}


#define _RESULT_TEXT(failure) (failure ? "FAILURE" : "SUCCESS")

enum mft__terminal_color {
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
#ifdef _WIN32 
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

    if (color == MFT_COLOR_DARK_WHITE) {
        printf("\033[0m");
    } else {
        u32 value = colorValues[color];
        printf("\033[1;%dm", value);
    }
#endif


}

void mft__reset_color() {
    mft__set_color(MFT_COLOR_DARK_WHITE);
}

void onSegfault(int signal, siginfo_t *si, void *arg) {
    // TODO: better reporting
    mft__set_color(MFT_COLOR_LIGHT_RED);
    printf("GOT SEGFAULT\n");
    mft__reset_color();
    exit(0);
}

i32 mft_run()
{
    bool collectionFailure = false;
    struct sigaction sa = {};
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = onSegfault;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);

#if 0 
    printf("\n=============================================\n");
    printf("Found %d TestCases\n", __mft_collection.index);
    printf("=============================================\n\n");
#endif
    //printf("\n---------- Found %d TestCases ---------->\n", __mft_collection.index);

    for (size_t i = 0; i < mf_vec_size(__mft_collection.test_cases); ++i) {
        mft__test_case *item = &__mft_collection.test_cases[i];
        __mft_collection.current_item = i;
        item->func(item);

        printf("%s:%d: '%s'  ", item->file, item->line, item->name);
        mft__set_color(item->failure ? MFT_COLOR_LIGHT_RED : MFT_COLOR_LIGHT_GREEN);
        printf("%s\n", _RESULT_TEXT(item->failure));
        if (item->failure) {
            printf("%s\n", item->output);
        }
        mft__reset_color();

        collectionFailure |= item->failure;

        mft__test_case_destroy(item);
    }

    //printf("<----------");
    mft__set_color(collectionFailure ? MFT_COLOR_LIGHT_RED : MFT_COLOR_LIGHT_GREEN);
    //printf("\n================ %s =================\n", _RESULT_TEXT(collectionFailure));
    //printf("%s", _RESULT_TEXT(collectionFailure));
    mft__reset_color();
    //printf("---------->\n");

    return collectionFailure;
}

#undef _RESULT_TEXT

#endif // MF_TEST_H
