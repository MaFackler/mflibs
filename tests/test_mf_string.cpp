#define MF_STRING_IMPLEMENTATION
#define MF_STRING_DEFAULT_SIZE 16
#include <mf_string.h>

void mft_check(mf_str s, const char *content, u64 size, u64 capacity);
#include <mf_test_def.h>


void mft_check(mf_str s, const char *content, u64 size, u64 capacity) {
    if (mf_str_size(s) != size || mf_str_capacity(s) != capacity || strcmp(s, content) != 0) {
        mft_test_error("mf_str differs %s != %s", s, content);
    }
}


TEST("mf_str_new empty") {
    mf_str s = mf_str_new();
    CHECK(s, "", 0, 16);
}

TEST("mf_str manipulation") {
    mf_str s = mf_str_new("hello");

    CHECK(s, "hello", 5, 16);

    mf_str_append(&s, "mflib");
    CHECK(s, "hellomflib", 10, 16);

    mf_str_append(&s, "12345");
    CHECK(s, "hellomflib12345", 15, 16);

    // FORCE RESIZE
    mf_str_append(&s, "-");
    CHECK(s, "hellomflib12345-", 16, 32);
}

TEST("mf_str_new_format") {
    mf_str s = mf_str_new_format("%s/%s", "mf", "libs");
    CHECK(s, "mf/libs");
}

TEST("mf_str_append on empty") {
    mf_str s = mf_str_new();
    mf_str_append(&s, "appended");
    CHECK(s, "appended");
}

TEST("mf_str_append loop") {
    mf_str s = mf_str_new();
    for (i32 i = 0; i < 1000; ++i) {
        mf_str_append(&s, "appended");
    }
}

#if 0

TEST("mf_string_endswith") {
    const char* my_base_string = "hello";
    MFT_CHECK(!mf_str_endswith(my_base_string, "ll"));
    MFT_CHECK(mf_str_endswith(my_base_string, "llo"));
}

TEST("mf_string_is_substring") {
    const char* my_base_string = "hello";
    MFT_CHECK(!mf_str_is_substr(my_base_string, "hllo"));
    MFT_CHECK(mf_str_is_substr(my_base_string, "hel"));
}

TEST("mf_string_is_equal") {
    const char* my_base_string = "hello";
    MFT_CHECK(!mf_str_is_equal((char *) my_base_string, "hllo"));
    MFT_CHECK(mf_str_is_equal((char *) my_base_string, "hello"));
    MFT_CHECK(!mf_str_is_equal((char *) my_base_string, "helloo"));
    MFT_CHECK(!mf_str_is_equal((char *) my_base_string, "hell"));
}

TEST("mf_string_count_char") {
    const char* my_base_string = "hello";
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'l'), 2);
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'e'), 1);
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'o'), 1);
}

TEST("mf_str_is_substr") {
    MFT_CHECK_TRUE(mf_str_is_substr("abc", "ab"));
    MFT_CHECK_FALSE(mf_str_is_substr("abc", "ac"));
}

TEST("mf_str_is_fuzzy") {
    MFT_CHECK_TRUE(mf_str_is_fuzzy("abc", "ac"));
    MFT_CHECK_FALSE(mf_str_is_fuzzy("abc", "ad"));

    MFT_CHECK_TRUE(mf_str_is_fuzzy("Abc", "ac"));
    MFT_CHECK_TRUE(mf_str_is_fuzzy("main.cpp", "mainc"));
}

TEST("mf_to_cbuf") {
    char buf[16];
    mf_to_cbuf(buf, 20);
    MFT_CHECK_STRING(buf, "20");
}

TEST("mf_to_cbuf") {
    char buf[16];
    mf_to_cbuf(buf, 20);
    MFT_CHECK_STRING(buf, "20");
    mf_cbuf_rfill(buf, sizeof(buf), '-', 4);
    MFT_CHECK_STRING(buf, "20--");
}
#endif
