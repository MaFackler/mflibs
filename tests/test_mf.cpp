#define MF_IMPLEMENTATION
#include "mf.h"
#include <regex>


TEST("Stretchy buffer")
{
    int *arr = NULL;
    MFT_CHECK_INT(mf_stretchy_size(arr), 0);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 0);

    mf_stretchy_push(arr, 1);
    MFT_CHECK_INT(mf_stretchy_size(arr), 1);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 2);

    mf_stretchy_push(arr, 2);
    MFT_CHECK_INT(mf_stretchy_size(arr), 2);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 2);

    *mf_stretchy_add(arr) = 3;
    MFT_CHECK_INT(mf_stretchy_size(arr), 3);
    MFT_CHECK_INT(mf__stretchy_capacity(arr), 6);

    MFT_CHECK_INT(arr[0], 1);
    MFT_CHECK_INT(arr[1], 2);
    MFT_CHECK_INT(arr[2], 3);

    mf_stretchy_for(arr) {
        printf("%d\n", *it); 
    }


    mf_stretchy_clear(arr);
    MFT_CHECK_INT(mf_stretchy_size(arr), 0);


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

TEST("MF_Max, MF_Min, MF_Clamp") {
    MFT_CHECK_INT(MF_Max(-1, 10), 10);
    MFT_CHECK_INT(MF_Max(1, 10), 10);

    MFT_CHECK_INT(MF_Min(-1, 10), -1);
    MFT_CHECK_INT(MF_Min(1, 10), 1);
}

