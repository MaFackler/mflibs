#define MF_IMPLEMENTATION
#include "mf.h"

TEST("Stretchy buffer")
{
    int *arr = NULL;
    MFT_CHECK_INT(mf_stretchy_size(arr), 0);

    mf_stretchy_push(arr, 1);
    MFT_CHECK_INT(mf_stretchy_size(arr), 1);

    mf_stretchy_push(arr, 2);
    MFT_CHECK_INT(mf_stretchy_size(arr), 2);

    mf_stretchy_clear(arr);
    MFT_CHECK_INT(mf_stretchy_size(arr), 0);
}


// String

TEST("mf_string_endswith")
{
    mf_cstr my_base_string = "hello";
    MFT_CHECK(!mf_str_endswith(my_base_string, "ll"));
    MFT_CHECK(mf_str_endswith(my_base_string, "llo"));
}

TEST("mf_string_is_substring")
{
    mf_cstr my_base_string = "hello";
    MFT_CHECK(!mf_str_is_substr(my_base_string, "hllo"));
    MFT_CHECK(mf_str_is_substr(my_base_string, "hel"));
}

TEST("mf_string_is_equal")
{
    mf_cstr my_base_string = "hello";
    MFT_CHECK(!mf_string_is_equal((char *) my_base_string, "hllo"));
    MFT_CHECK(mf_string_is_equal((char *) my_base_string, "hello"));
    MFT_CHECK(!mf_string_is_equal((char *) my_base_string, "helloo"));
    MFT_CHECK(!mf_string_is_equal((char *) my_base_string, "hell"));
}

TEST("mf_string_count_char")
{
    mf_cstr my_base_string = "hello";
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'l'), 2);
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'e'), 1);
    MFT_CHECK_INT(mf_str_count_char(my_base_string, 'o'), 1);
}

TEST("mf_str_is_substr")
{
    MFT_CHECK_TRUE(mf_str_is_substr("abc", "ab"));
    MFT_CHECK_FALSE(mf_str_is_substr("abc", "ac"));
}

TEST("mf_str_is_fuzzy")
{
    MFT_CHECK_TRUE(mf_str_is_fuzzy("abc", "ac"));
    MFT_CHECK_FALSE(mf_str_is_fuzzy("abc", "ad"));

    MFT_CHECK_TRUE(mf_str_is_fuzzy("Abc", "ac"));
    MFT_CHECK_TRUE(mf_str_is_fuzzy("main.cpp", "mainc"));
}

TEST("mf_to_cbuf")
{
    char buf[16];
    mf_to_cbuf(buf, 20);
    MFT_CHECK_STRING(buf, "20");
}

TEST("mf_to_cbuf")
{
    char buf[16];
    mf_to_cbuf(buf, 20);
    mf_cbuf_rfill(buf, sizeof(buf), '-', 4);
    MFT_CHECK_STRING(buf, "20--");
}


// Path, File
TEST("mf_path_join_create")
{
    char *res = mf_path_join_create("home", "mf", MF_PATH_SEPARATOR_UNIX);
    MFT_CHECK_STRING(res, "home/mf");
    free(res);
}

TEST("mf_is_file")
{
    MFT_CHECK(mf_is_file("./src/mf.h"));
    MFT_CHECK(mf_is_file("src/mf.h"));
    MFT_CHECK(!mf_is_file("src/mfnot.h"));
}

TEST("mf_directory")
{
    mf_directory dir;
    mf_path_item item;
    mf_directory_open(&dir, "src", false);
    int counter = 0;
    while(mf_directory_next(&dir, &item))
    {
        counter++;
    }
    // TODO: 8 becuase "." "..". Best way to handle that?
    MFT_CHECK_INT(counter, 8);
}

