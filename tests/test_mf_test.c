#include <mf_test.h>

TEST(CHECK_NE) {
    CHECK_NE(1, 2);
}

TEST(CHECK_EQ) {
    CHECK_EQ(1, 1);
}

TEST(CHECK_LT) {
    CHECK_LT(1, 2);
}

TEST(CHECK_GT) {
    CHECK_GT(2, 1);
}

TEST(CHECK_LE) {
    CHECK_LE(1, 2);
    CHECK_LE(1, 1);
}

TEST(CHECK_GE) {
    CHECK_GE(2, 1);
    CHECK_GE(1, 1);
}

TEST(CHECK_INT) {
    CHECK_EQ(-1, -1);
}

TEST(CHECK_FLOAT) {
    CHECK_EQ(1.0f, 1.0f);
}

TEST(CHECK_UINT) {
    unsigned int a = 0;
    unsigned int b = 0;
    CHECK_EQ(a, b);
}

TEST(CHECK_DOUBLE) {
    double a = 0;
    double b = 0;
    CHECK_EQ(a, b);
}

TEST(CHECK_BOOL) {
    CHECK_TRUE(true);
    CHECK_FALSE(false);
}

// TODO: String equals
