#include <mf_lib.h>
MF_LIB_LINUX("m")
#define MF_STRING_IMPLEMENTATION
#define MF_STRING_DEFAULT_SIZE 16
#include <mf_string.h>
#include <mf_test.h>

void CHECK_MF_STR(MF_String s, const char *content, size_t size, size_t capacity) {
    CHECK_EQ(MF_StringLen(s), size);
    CHECK_EQ(MF_StringCapacity(s), capacity);
    CHECK_EQ(strcmp(s, content), 0);
}

TEST(MF_StringNew_empty) {
    MF_String s = MF_StringNew();
    CHECK_MF_STR(s, "", 0, 16);
}

TEST(MF_String_manipulation) {
    MF_String s = MF_StringNewS("hello");

    CHECK_MF_STR(s, "hello", 5, 16);

    MF_StringAppend(&s, "mflib");
    CHECK_MF_STR(s, "hellomflib", 10, 16);

    MF_StringAppend(&s, "12345");
    CHECK_MF_STR(s, "hellomflib12345", 15, 16);

    MF_StringAppend(&s, "_");
    CHECK_MF_STR(s, "hellomflib12345_", 16, 32);
}

TEST(MF_StringNewFormat) {
    MF_String s = MF_StringNewFormat("%s/%s", "mf", "libs");
    CHECK_STR(s, "mf/libs");
}

TEST(MF_StringAppend_empty) {
    MF_String s = MF_StringNew();
    MF_StringAppend(&s, "appended");
    CHECK_STR(s, "appended");
}

TEST(MF_StringAppend_loop) {
    MF_String s = MF_StringNew();
    for (int i = 0; i < 1000; ++i) {
        MF_StringAppend(&s, "12345");
    }
    CHECK_EQ(MF_StringLen(s), 1000 * 5);
}
