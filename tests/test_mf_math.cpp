#define MF_MATH_IMPLEMENTATION
#include "../src/mf_math.h"

TEST("Vector 2 is equal")
{
    mfm_v2 v1{2.0f, 3.0f};
    mfm_v2 v2{2.0f, 3.0f};

    MFT_CHECK(v1 == v2);
    MFT_CHECK(mfm_v2_is_equal(v1, v2));
}

TEST("Vector 2 int add and add assign")
{
    mfm_v2i v1{2, 3};
    mfm_v2i v2{2, 3};

    mfm_v2i res = v1 + v2;
    MFT_CHECK_INT(res.x, 4);
    MFT_CHECK_INT(res.y, 6);

    v1 += v2;
    MFT_CHECK_INT(v1.x, 4);
    MFT_CHECK_INT(v1.y, 6);
}

TEST("Vector 255 to 1")
{
    mfm_v3 res = mfm_v3_255_to_1(128, 0, 255);

    MFT_CHECK_FLOAT(res.x, 0.5019f);
    MFT_CHECK_FLOAT(res.y, 0.0f);
    MFT_CHECK_FLOAT(res.z, 1.0f);
}

TEST("Vector 3 add")
{
    mfm_v3 v1{1.0f, 2.0f, 3.0f};
    mfm_v3 v2{4.0f, 5.0f, 6.0f};

    {
        mfm_v3 res = v1 + v2;
        MFT_CHECK_FLOAT(res.x, 5.0f);
        MFT_CHECK_FLOAT(res.y, 7.0f);
        MFT_CHECK_FLOAT(res.z, 9.0f);
    }

    {
        mfm_v3 res = mfm_v3_add(v1, v2);
        MFT_CHECK_FLOAT(res.x, 5.0f);
        MFT_CHECK_FLOAT(res.y, 7.0f);
        MFT_CHECK_FLOAT(res.z, 9.0f);
    }
}

TEST("Vector 3 sub")
{
    mfm_v3 v1{1.0f, 2.0f, 3.0f};
    mfm_v3 v2{4.0f, 5.0f, 6.0f};

    {
        mfm_v3 res = v1 - v2;
        MFT_CHECK_FLOAT(res.x, -3.0f);
        MFT_CHECK_FLOAT(res.y, -3.0f);
        MFT_CHECK_FLOAT(res.z, -3.0f);
    }

    {
        mfm_v3 res = mfm_v3_sub(v1, v2);
        MFT_CHECK_FLOAT(res.x, -3.0f);
        MFT_CHECK_FLOAT(res.y, -3.0f);
        MFT_CHECK_FLOAT(res.z, -3.0f);
    }
}

TEST("Vector 3 mul")
{
    mfm_v3 v1{1.0f, 2.0f, 3.0f};

    {
        mfm_v3 res = v1 * 2.0f;
        MFT_CHECK_FLOAT(res.x, 2.0f);
        MFT_CHECK_FLOAT(res.y, 4.0f);
        MFT_CHECK_FLOAT(res.z, 6.0f);
    }

    {
        mfm_v3 res = mfm_v3_mul(v1, 2.0f);
        MFT_CHECK_FLOAT(res.x, 2.0f);
        MFT_CHECK_FLOAT(res.y, 4.0f);
        MFT_CHECK_FLOAT(res.z, 6.0f);
    }
}
