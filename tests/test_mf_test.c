#include <mf_test.h>

TEST(check_ne) {
    CHECK_NE(1, 2);
}

TEST(check_eq) {
    CHECK_EQ(1, 1);
}

TEST(check_lt) {
    CHECK_LT(1, 2);
}

TEST(check_gt) {
    CHECK_GT(2, 1);
}

TEST(check_le) {
    CHECK_LE(1, 2);
    CHECK_LE(1, 1);
}

TEST(check_ge) {
    CHECK_GE(2, 1);
    CHECK_GE(1, 1);
}

TEST(check_int) {
    CHECK_EQ(-1, -1);
}

TEST(check_float) {
    CHECK_EQ(1.0f, 1.0f);
}

TEST(check_uint) {
    unsigned int a = 0;
    unsigned int b = 0;
    CHECK_EQ(a, b);
}

TEST(check_double) {
    double a = 0;
    double b = 0;
    CHECK_EQ(a, b);
}

TEST(check_bool) {
    CHECK_TRUE(true);
    CHECK_FALSE(false);
}

// TODO: String equals
