#define MF_STRING_IMPLEMENTATION
#include "../src/mf_string.h"

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