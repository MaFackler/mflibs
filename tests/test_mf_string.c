#define MF_STRING_IMPLEMENTATION
#define MF_STRING_DEFAULT_SIZE 16
#include <mf_string.h>
#include <mf_test.h>


void CHECK_MF_STR(MF_String s, const char *content, size_t size, size_t capacity) {
    CHECK_EQ(mf_str_length(s), size);
    CHECK_EQ(mf_str_capacity(s), capacity);
    CHECK_EQ(strcmp(s, content), 0);
}


TEST(mf_str_new_empty) {
    MF_String s = mf_str_new();
    CHECK_MF_STR(s, "", 0, 16);
}

TEST(mf_str_manipulation) {
    MF_String s = mf_str_news("hello");

    CHECK_MF_STR(s, "hello", 5, 16);

    mf_str_append(&s, "mflib");
    CHECK_MF_STR(s, "hellomflib", 10, 16);

    mf_str_append(&s, "12345");
    CHECK_MF_STR(s, "hellomflib12345", 15, 16);

    mf_str_append(&s, "_");
    CHECK_MF_STR(s, "hellomflib12345_", 16, 32);
}

TEST(mf_str_new_format) {
    MF_String s = mf_str_new_format("%s/%s", "mf", "libs");
    CHECK_STR(s, "mf/libs");
}

TEST(mf_str_append_on_empty) {
    MF_String s = mf_str_new();
    mf_str_append(&s, "appended");
    CHECK_STR(s, "appended");
}

TEST(mf_str_append_loop) {
    MF_String s = mf_str_new();
    for (int i = 0; i < 1000; ++i) {
        mf_str_append(&s, "12345");
    }
    CHECK_EQ(mf_str_length(s), 1000 * 5);
}
