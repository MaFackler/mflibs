#define MF_FILE_IMPLEMENTATION
#include <mf_file.h>
#include <mf_test.h>

TEST(MFF_FileRead_empty) {
    size_t size = 0;
    char *contents = MFF_FileRead("tests/data/testfile_empty.txt", "rb", &size);
    CHECK_EQ(size, 0);
}

TEST(MFF_FileRead) {
    size_t size = 0;
    char *contents = MFF_FileRead("tests/data/testfile.txt", "rb", &size);
    CHECK_EQ(size, 4);
    CHECK_STRN(contents, "abc", 3);
    CHECK_EQ(contents[3], '\n');
}

TEST(MFF_PathJoinCreate) {
    char *res = MFF_PathJoinCreate("home", "mf", MF_PATH_SEPARATOR_UNIX);
    CHECK_STR(res, "home/mf");
    free(res);
}

TEST(MFF_IsFile) {
    CHECK_TRUE(MFF_IsFile("./src/mf.h"));
    CHECK_TRUE(MFF_IsFile("src/mf.h"));
    CHECK_TRUE(!MFF_IsFile("src/mfnot.h"));
}

TEST(MFF_Directory) {
    MFF_Directory dir;
    MFF_PathItem item;
    MFF_DirectoryOpen(&dir, "src", false);
    int counter = 0;
    while(MFF_DirectoryNext(&dir, &item)) {
        counter++;
    }
    CHECK_EQ(counter, 19);
}

