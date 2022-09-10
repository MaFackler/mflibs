#include <mf_test_def.h>

#define MF_STRING_IMPLEMENTATION
#define MF_STRING_DEFAULT_SIZE 16
#include <mf_string.h>

#define ASSERT_MF_STR(s, expected, size, capactity) \
    MFT_CHECK_U64(mf_str_size(s), size); \
    MFT_CHECK_U64(mf_str_capacity(s), capactity); \
    MFT_CHECK_CHAR(s[mf_str_size(s)], '\0'); \
    MFT_CHECK_STRING(s, expected); \

TEST("mf_str_new") {
    mf_str s = mf_str_new("hello");
    ASSERT_MF_STR(s, "hello", 5, 16);

    mf_str_append(&s, "mflib");
    ASSERT_MF_STR(s, "hellomflib", 10, 16);

    mf_str_append(&s, "12345");
    ASSERT_MF_STR(s, "hellomflib12345", 15, 16);

    // FORCE RESIZE
    mf_str_append(&s, "-");
    ASSERT_MF_STR(s, "hellomflib12345-", 16, 32);
}

#undef ASSERT_MF_STR

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
