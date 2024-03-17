#define MF_FILE_IMPLEMENTATION
#include <mf_file.h>
#include <mf_test.h>

TEST(mff_file_read_empty) {
    size_t size = 0;
    char *contents = mff_file_read("tests/data/testfile_empty.txt", "rb", &size);
    CHECK_EQ(size, 0);
}

TEST(mff_file_read) {
    size_t size = 0;
    char *contents = mff_file_read("tests/data/testfile.txt", "rb", &size);
    CHECK_EQ(size, 4);
    CHECK_STRN(contents, "abc", 3);
    CHECK_EQ(contents[3], '\n');
}

TEST(mff_path_join_create) {
    char *res = mff_path_join_create("home", "mf", MF_PATH_SEPARATOR_UNIX);
    CHECK_STR(res, "home/mf");
    free(res);
}

TEST(mff_is_file) {
    CHECK_TRUE(mff_is_file("./src/mf.h"));
    CHECK_TRUE(mff_is_file("src/mf.h"));
    CHECK_TRUE(!mff_is_file("src/mfnot.h"));
}

TEST(mf_directory) {
    MFF_Directory dir;
    MFF_PathItem item;
    mff_directory_open(&dir, "src", false);
    int counter = 0;
    while(mff_directory_next(&dir, &item)) {
        counter++;
    }
    CHECK_EQ(counter, 15);
}

