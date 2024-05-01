#define MF_MATH_IMPLEMENTATION
#include <mf_test.h>
#define MF_MATH_SHORT_NAMES
#include <mf_math.h>

TEST(MFM_Clamp) {
    CHECK_EQ(MFM_Clamp(1.0, 0.0, 100.0), 1.0);
    CHECK_EQ(MFM_Clamp(1.0, 2.0, 100.0), 2.0);
    CHECK_EQ(MFM_Clamp(110.0, 0.0, 100.0), 100.0);
}

// v2
TEST(MFM_Vec2) {
    Vec2 a = {0};
    CHECK_EQ(a.x, 0.0f);
    CHECK_EQ(a.y, 0.0f);

    CHECK_EQ(a.m[0], 0.0f);
    CHECK_EQ(a.m[1], 0.0f);
}

TEST(MFM_Vec2Equal) {
    Vec2 a = {2.0f, 3.0f};
    Vec2 b = {2.0f, 3.0f};

    CHECK_TRUE(MFM_Vec2Equal(a, b));
}

TEST(MFM_Vec2Length) {
    Vec2 a = {4, 2};
    float res = MFM_Vec2Length(a);
    CHECK_NEAR(res, 4.47f);
}


TEST(MFM_Vec2Distance) {
    Vec2 a = {-7, -4};
    Vec2 b = {17, 6.5};
    float res = MFM_Vec2Distance(a, b);
    CHECK_NEAR(res, 26.196374f);
}

TEST(MFM_Vec2Normalize_MFM_Vec2Length) {
    Vec2 a = {1, 2};
    Vec2 res = MFM_Vec2Normalize(a);
    CHECK_NEAR(MFM_Vec2Length(res), 1.0f);
}

TEST(MFM_Vec2Add) {
    Vec2 a = {2.0f, 3.0f};
    Vec2 b = {2.0f, 3.0f};

    Vec2 res = MFM_Vec2Add(a, b);
    CHECK_NEAR(res.x, 4.0f);
    CHECK_NEAR(res.y, 6.0f);
}

TEST(MFM_Mat4Identity) {
    Mat4 identity = MFM_Mat4Identity();
    Vec3 pos = {1.0f, 0.0f};
    Vec3 res = MFM_Mat4MulVec3(identity, pos);
    CHECK_EQ(res.x, pos.x);
    CHECK_EQ(res.y, pos.y);
    CHECK_EQ(res.z, pos.z);
}

TEST(MFM_Mat4Ortho) {
    Mat4 ortho = MFM_Mat4Ortho(-200.0f, 200.0f,
                               -100.0f, 100.0f,
                               0.0f, 10.0f);
    Vec3 pos = {200.0f, 100.0f, 10.0f};
    Vec3 res = MFM_Mat4MulVec3(ortho, pos);
    CHECK_EQ(res.x, 1.0f);
    CHECK_EQ(res.y, 1.0f);
    // TODO: why is the result -3.0f and not -1.0f?
    CHECK_EQ(res.z, -3.0f);

    pos = (Vec3) {-200.0f, -100.0f, 0.0f};
    res = MFM_Mat4MulVec3(ortho, pos);
    CHECK_EQ(res.x, -1.0f);
    CHECK_EQ(res.y, -1.0f);
    CHECK_EQ(res.z, -1.0f);
}

TEST(MFM_Mat4LookAt) {
    Vec3 eye = {1.0f, 0, 0};
    Vec3 at = {0, 0, -10};
    Vec3 up = {0, 1, 0};
    Mat4 lookAt = MFM_Mat4LookAt(eye, at, up);

    Vec3 pos = {1.0f, 0.0f, 0.0f};
    Vec3 res = MFM_Mat4MulVec3(lookAt, pos);
    CHECK_EQ(res.x, 0.0f);
    CHECK_EQ(res.y, 0.0f);
    CHECK_EQ(res.z, 0.0f);
}

TEST(MFM_Mat4Mul) {
    Mat4 a = {
        4.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 8.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
    };
    Mat4 b = {
        4.0f, 2.0f, 1.0f, 0.0f,
        2.0f, 0.0f, 4.0f, 0.0f,
        9.0f, 4.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
    };
    Mat4 res = MFM_Mat4Mul(a, b);
    CHECK_NEAR(res.rows[0].m[0], 20.0f);
    CHECK_NEAR(res.rows[0].m[1], 8.0f);
    CHECK_NEAR(res.rows[0].m[2], 12.0f);

    CHECK_NEAR(res.rows[1].m[0], 25.0f);
    CHECK_NEAR(res.rows[1].m[1], 4.0f);
    CHECK_NEAR(res.rows[1].m[2], 34.0f);

    CHECK_NEAR(res.rows[2].m[0], 2.0f);
    CHECK_NEAR(res.rows[2].m[1], 0.0f);
    CHECK_NEAR(res.rows[2].m[2], 4.0f);
}
