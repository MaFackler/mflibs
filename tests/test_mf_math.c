#define MF_MATH_IMPLEMENTATION
#include <mf_test.h>
#include <mf_math.h>

TEST(clamp) {
    CHECK_EQ(clamp(1.0, 0.0, 100.0), 1.0);
    CHECK_EQ(clamp(1.0, 2.0, 100.0), 2.0);
    CHECK_EQ(clamp(110.0, 0.0, 100.0), 100.0);
}

// v2
TEST(v2_basic) {
    v2 a = {0};
    CHECK_EQ(a.x, 0.0f);
    CHECK_EQ(a.y, 0.0f);

    CHECK_EQ(a.m[0], 0.0f);
    CHECK_EQ(a.m[1], 0.0f);
}

TEST(v2_equal) {
    v2 a = {2.0f, 3.0f};
    v2 b = {2.0f, 3.0f};

    CHECK_TRUE(v2_eq(a, b));
}


TEST(v2_distance) {
    v2 a = {-7, -4};
    v2 b = {17, 6.5};
    float res = v2_distance(a, b);
    CHECK_NEAR(res, 26.196374f);
}

TEST(v2_normalize_length) {
    v2 a = {1, 2};
    v2 res = v2_normalize(a);
    CHECK_NEAR(v2_length(res), 1.0f);
}

TEST(v2_add) {
    v2 a = {2.0f, 3.0f};
    v2 b = {2.0f, 3.0f};

    v2 res = v2_add(a, b);
    CHECK_NEAR(res.x, 4.0f);
    CHECK_NEAR(res.y, 6.0f);
}

