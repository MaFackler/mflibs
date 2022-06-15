#define MF_FILE_IMPLEMENTATION
#include "../src/mf_file.h"

TEST("mff_file_read empty file") {
    size_t size = 0;
    char *contents = mff_file_read("tests/data/testfile_empty.txt", "rb", &size);
    MFT_CHECK_SIZET(size, 0);
}

TEST("mff_file_read") {
    size_t size = 0;
    char *contents = mff_file_read("tests/data/testfile.txt", "rb", &size);
    MFT_CHECK_SIZET(size, 4);
    MFT_CHECK_STRINGN(contents, "abc", 3);
    MFT_CHECK_INT(contents[3], '\n');
}

TEST("mf_path_join_create") {
    char *res = mf_path_join_create("home", "mf", MF_PATH_SEPARATOR_UNIX);
    MFT_CHECK_STRING(res, "home/mf");
    free(res);
}

TEST("mf_is_file") {
    MFT_CHECK(mf_is_file("./src/mf.h"));
    MFT_CHECK(mf_is_file("src/mf.h"));
    MFT_CHECK(!mf_is_file("src/mfnot.h"));
}

#if 0
TEST("mf_directory") {
    mf_directory dir;
    mf_path_item item;
    mf_directory_open(&dir, "src", false);
    int counter = 0;
    while(mf_directory_next(&dir, &item)) {
        counter++;
    }
    MFT_CHECK_INT(counter, 9);
}
#endif
