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
    const char *my_base_string = "hello";
    MFT_CHECK(!mf_string_endswith((char *) my_base_string, "ll"));
    MFT_CHECK(mf_string_endswith((char *) my_base_string, "llo"));
}

TEST("mf_string_is_substring")
{
    const char *my_base_string = "hello";
    MFT_CHECK(!mf_string_is_substring((char *) my_base_string, "hllo"));
    MFT_CHECK(mf_string_is_substring((char *) my_base_string, "hel"));
}

TEST("mf_string_is_equal")
{
    const char *my_base_string = "hello";
    MFT_CHECK(!mf_string_is_equal((char *) my_base_string, "hllo"));
    MFT_CHECK(mf_string_is_equal((char *) my_base_string, "hello"));
    MFT_CHECK(!mf_string_is_equal((char *) my_base_string, "helloo"));
    MFT_CHECK(!mf_string_is_equal((char *) my_base_string, "hell"));
}

TEST("mf_string_count_char")
{
    const char *my_base_string = "hello";
    MFT_CHECK_INT(mf_string_count_char((char * ) my_base_string, 'l'), 2);
    MFT_CHECK_INT(mf_string_count_char((char * ) my_base_string, 'e'), 1);
    MFT_CHECK_INT(mf_string_count_char((char * ) my_base_string, 'o'), 1);
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

