#ifndef MF_TEST_H
#define MF_TEST_H
#include <stdbool.h>
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

#define MFT_MODULE_INIT_FUNC __attribute__((constructor))
#define MFT_MALLOC malloc

#define MFT_MAIN() \
    MftState state = {0}; \
    int main(int argc, char **argv) { \
        mft_main(argc, argv); \
    } \

#define TEST(n) \
    extern MftState state; \
    void n ## run(MftTestCase *self); \
    MFT_MODULE_INIT_FUNC void n ## initialize() { \
        MftTestCase* test_case = _mft_test_case_add(&state); \
        test_case->cb = n ## run; \
        test_case->name = #n; \
        test_case->filename = __FILE__; \
        test_case->line = __LINE__; \
    } \
    void n ## run(MftTestCase *self) \


typedef struct MftTestCase MftTestCase;
struct MftTestCase {
    const char* filename;
    const char* name;
    size_t line;
    bool failure;
    void (*cb)(MftTestCase *self);
};

typedef struct MftState MftState;
struct MftState {
    unsigned int count;
    unsigned int capacity;
    MftTestCase *test_cases;
};

MftTestCase* _mft_test_case_add(MftState *state) {
    if (state->capacity == 0) {
        state->capacity = 256;
        state->test_cases = (MftTestCase*) MFT_MALLOC(sizeof(MftTestCase) * state->capacity);
    }

    if (state->count >= state->capacity) {
        state->capacity *= 2;
        state->test_cases = (MftTestCase*) realloc(state->test_cases,
                                    sizeof(MftTestCase) * state->capacity);
    }

    MftTestCase *res = &state->test_cases[state->count++];
    memset(res, 0, sizeof(MftTestCase));
    return res;
}

extern MftState state;

#define MFT_ANSI_ESCAPE_RESET "\033[0m" 
#define MFT_ANSI_ESCAPE_COLOR "\033[1;%dm"

void _mft_on_segfault(int signal, siginfo_t *si, void *arg) {
    printf(MFT_ANSI_ESCAPE_COLOR, 91);
    printf("GOT SEGFAULT\n");
    printf(MFT_ANSI_ESCAPE_RESET);
    exit(1);
}

void mft_main(int argc, char **argv) {
    struct sigaction sa = {0};
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = _mft_on_segfault;
    sa.sa_flags = SA_SIGINFO;

    for (unsigned int i = 0; i < state.count; ++i) {
        MftTestCase *test_case = &state.test_cases[i];
        printf("%s:%d %s: ", test_case->filename, test_case->line, test_case->name);
        test_case->cb(test_case);
        if (!test_case->failure) {
            printf(MFT_ANSI_ESCAPE_COLOR, 92);
            printf("SUCCESS\n");
        }
        printf(MFT_ANSI_ESCAPE_RESET);
    }
}

#define MFT_PRINT_TYPE(t) _Generic((t), \
    int: printf("%d", t), \
    unsigned int: printf("%u", t), \
    double: printf("%lf", t), \
    float: printf("%f", t), \
    const char*: printf("%s", t), \
    char*: printf("%s", t), \
    char: printf("%c", t), \
    long unsigned int: printf("%lu", t), \
    _Bool: printf("%s", t ? "true" : "false"))

#define MFT_COMPARE(a, b, condition) \
    {__typeof__(a) aresolved = (a); \
    __typeof__(b) bresolved = (b); \
    if (!((aresolved) condition (bresolved))) { \
        printf(MFT_ANSI_ESCAPE_COLOR, 91); \
        printf("FAILED\n"); \
        printf(MFT_ANSI_ESCAPE_RESET); \
        printf(MFT_ANSI_ESCAPE_COLOR, 97); \
        printf("%s:%d\n", __FILE__, __LINE__); \
        printf(" Expected: %s %s %s\n", #a, #condition, #b); \
        printf("      Got: "); \
        MFT_PRINT_TYPE(aresolved); \
        printf(" <> "); \
        MFT_PRINT_TYPE(bresolved); \
        printf(MFT_ANSI_ESCAPE_RESET); \
        printf("\n"); \
        self->failure = true; \
    }}

#define MFT_NEAR(a, b) \
    {__typeof__(a) aresolved = (a); \
    __typeof__(b) bresolved = (b); \
    if (abs((aresolved) - (bresolved)) > 0.001f) { \
        printf(MFT_ANSI_ESCAPE_COLOR, 91); \
        printf("FAILED\n"); \
        printf(MFT_ANSI_ESCAPE_RESET); \
        printf(MFT_ANSI_ESCAPE_COLOR, 97); \
        printf("%s:%d\n", __FILE__, __LINE__); \
        printf(" Expected: %s near %s\n", #a, #b); \
        printf("      Got: "); \
        MFT_PRINT_TYPE(aresolved); \
        printf(" <> "); \
        MFT_PRINT_TYPE(bresolved); \
        printf(MFT_ANSI_ESCAPE_RESET); \
        printf("\n"); \
        self->failure = true; \
    }}

#define MFT__COMPARE_STRN(a, b, n) \
    {__typeof__(a) aresolved = (a); \
    __typeof__(b) bresolved = (b); \
    if (strncmp(a, b, n) != 0) { \
        printf(MFT_ANSI_ESCAPE_COLOR, 91); \
        printf("FAILED\n"); \
        printf(MFT_ANSI_ESCAPE_RESET); \
        printf(MFT_ANSI_ESCAPE_COLOR, 97); \
        printf("%s:%d\n", __FILE__, __LINE__); \
        printf(" Expected: %s near %s\n", #a, #b); \
        printf("      Got: "); \
        MFT_PRINT_TYPE(aresolved); \
        printf(" <> "); \
        MFT_PRINT_TYPE(bresolved); \
        printf(MFT_ANSI_ESCAPE_RESET); \
        printf("\n"); \
        self->failure = true; \
    }}


#define CHECK_EQ(a, b) MFT_COMPARE(a, b, ==)
#define CHECK_NE(a, b) MFT_COMPARE(a, b, !=)
#define CHECK_LT(a, b) MFT_COMPARE(a, b, <)
#define CHECK_GT(a, b) MFT_COMPARE(a, b, >)
#define CHECK_LE(a, b) MFT_COMPARE(a, b, <=)
#define CHECK_GE(a, b) MFT_COMPARE(a, b, >=)
#define CHECK_TRUE(a) MFT_COMPARE(a, true, ==)
#define CHECK_FALSE(a) MFT_COMPARE(a, false, ==)

#define CHECK_NEAR(a, b) MFT_NEAR(a, b)
#define CHECK_STR(a, b) MFT__COMPARE_STRN(a, b, ((strlen(a) < strlen(b)) ? strlen(a) : strlen(b)))
#define CHECK_STRN(a, b, n) MFT__COMPARE_STRN(a, b, n)


#ifdef MFT_WITH_MAIN
MFT_MAIN();
#endif

#endif // MF_TEST_H
