#define MF_IMPLEMENTATION
#include "mf.h"
#include <regex>


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

