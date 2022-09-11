#include <mf_test_def.h>
#define MF_FILE_IMPLEMENTATION
#include <mf_file.h>

TEST("mff_file_read empty file") {
    size_t size = 0;
    char *contents = mff_file_read("tests/data/testfile_empty.txt", "rb", &size);
    CHECK(size, 0);
}

TEST("mff_file_read") {
    size_t size = 0;
    char *contents = mff_file_read("tests/data/testfile.txt", "rb", &size);
    CHECK(size, 4);
    CHECK(contents, "abc", 3);
    CHECK(contents[3], '\n');
}

TEST("mf_path_join_create") {
    char *res = mf_path_join_create("home", "mf", MF_PATH_SEPARATOR_UNIX);
    CHECK(res, "home/mf");
    free(res);
}

TEST("mf_is_file") {
    CHECK(mf_is_file("./src/mf.h"));
    CHECK(mf_is_file("src/mf.h"));
    CHECK(!mf_is_file("src/mfnot.h"));
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
    CHECK(counter, 9);
}
#endif

