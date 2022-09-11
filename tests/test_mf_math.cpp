#define MF_MATH_IMPLEMENTATION
#include <mf_math.h>

#include <mf_test_def.h>

TEST("mf_clamp") {
    CHECK(mf_clamp(1.0, 0.0, 100.0), 1.0);
    CHECK(mf_clamp(1.0, 2.0, 100.0), 2.0);
    CHECK(mf_clamp(110.0, 0.0, 100.0), 100.0);
}

TEST("Vector 2 is equal") {
    mfm_v2 v1{2.0f, 3.0f};
    mfm_v2 v2{2.0f, 3.0f};

    CHECK(v1 == v2);
    CHECK(mfm_v2_is_equal(v1, v2));
}

TEST("Vector 2 distance") {
    mfm_v2 v1{-7, -4};
    mfm_v2 v2{17, 6.5};
    float res = mfm_v2_distance(v1, v2);
    CHECK(res, 26.196374f);
}

TEST("Vector 2 normalize and lenght") {
    mfm_v2 v1{1, 2};
    mfm_v2 res = mfm_v2_normalize(v1);
    CHECK(mfm_v2_length(res), 1.0f);
}

TEST("Vector 2 int add and add assign") {
    mfm_v2i v1{2, 3};
    mfm_v2i v2{2, 3};

    mfm_v2i res = v1 + v2;
    CHECK(res.x, 4);
    CHECK(res.y, 6);

    v1 += v2;
    CHECK(v1.x, 4);
    CHECK(v1.y, 6);
}

TEST("Vector 255 to 1") {
    mfm_v3<float> res = mfm_v3_255_to_1<float>(128, 0, 255);

    CHECK(res.x, 0.5019f);
    CHECK(res.y, 0.0f);
    CHECK(res.z, 1.0f);
}

TEST("Vector 3 add") {
    mfm_v3<float> v1{1.0f, 2.0f, 3.0f};
    mfm_v3<float> v2{4.0f, 5.0f, 6.0f};

    {
        mfm_v3<float> res = v1 + v2;
        CHECK(res.x, 5.0f);
        CHECK(res.y, 7.0f);
        CHECK(res.z, 9.0f);
    }

    {
        mfm_v3<float> res = mfm_v3_add(v1, v2);
        CHECK(res.x, 5.0f);
        CHECK(res.y, 7.0f);
        CHECK(res.z, 9.0f);
    }
}

TEST("Vector 3 sub") {
    mfm_v3<float> v1{1.0f, 2.0f, 3.0f};
    mfm_v3<float> v2{4.0f, 5.0f, 6.0f};

    {
        mfm_v3<float> res = v1 - v2;
        CHECK(res.x, -3.0f);
        CHECK(res.y, -3.0f);
        CHECK(res.z, -3.0f);
    }

    {
        mfm_v3<float> res = mfm_v3_sub(v1, v2);
        CHECK(res.x, -3.0f);
        CHECK(res.y, -3.0f);
        CHECK(res.z, -3.0f);
    }
}

TEST("Vector 3 mul") {
    mfm_v3<float> v1{1.0f, 2.0f, 3.0f};

    {
        mfm_v3<float> res = v1 * 2.0f;
        CHECK(res.x, 2.0f);
        CHECK(res.y, 4.0f);
        CHECK(res.z, 6.0f);
    }

    {
        mfm_v3<float> res = mfm_v3_mul(v1, 2.0f);
        CHECK(res.x, 2.0f);
        CHECK(res.y, 4.0f);
        CHECK(res.z, 6.0f);
    }
}

