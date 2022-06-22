#ifndef MF_MATH_H
#define MF_MATH_H
#include <math.h>


inline float mfm_to_rad(float degree) {
    return degree * (M_PI / 180);
}

inline float mfm_to_degree(float rad) {
    return rad * (180 / M_PI);
}

float mfm_lerp(float a, float b, float t);

typedef struct
{
    float x;
    float y;
} mfm_v2;


float mfm_v2_distance(mfm_v2 a, mfm_v2 b);
float mfm_v2_distance_squared(mfm_v2 a, mfm_v2 b);
float mfm_v2_length(mfm_v2 a);
mfm_v2 mfm_v2_normalize(mfm_v2 a);
mfm_v2 mfm_v2_subtract(mfm_v2 a, mfm_v2 b);
mfm_v2 mfm_v2_mul(mfm_v2 a, mfm_v2 b);
mfm_v2 mfm_v2_mul(mfm_v2 a, float b);
mfm_v2 mfm_v2_div(mfm_v2 a, mfm_v2 b);
mfm_v2 mfm_v2_div(mfm_v2 a, float b);



typedef struct
{
    int x;
    int y;

} mfm_v2i;


bool mfm_v2_is_equal(mfm_v2 a, mfm_v2 b);

mfm_v2i mfm_v2i_add(mfm_v2i a, mfm_v2i b);

typedef struct
{
    float x;
    float y;
    float z;
} mfm_v3;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} mfm_v4;

typedef struct
{
    int x;
    int y;
    int z;
} mfm_v3i;

mfm_v3 mfm_v3_255_to_1(int r, int g, int b);

float mfm_v3_length(mfm_v3 a);
mfm_v3 mfm_v3_lerp(mfm_v3 a, mfm_v3 b, float t);
mfm_v3 mfm_v3_add(mfm_v3 a, mfm_v3 b);
mfm_v3 mfm_v3_sub(mfm_v3 a, mfm_v3 b);
mfm_v3 mfm_v3_mul(mfm_v3 a, float b);
float mfm_v3_dot(mfm_v3 a, mfm_v3 b);
mfm_v3 mfm_v3_normalize(mfm_v3 a);
mfm_v3 mfm_v3_cross(mfm_v3 a, mfm_v3 b);
mfm_v3 mfm_v3_negate(mfm_v3 a);


union mfm_rect
{
    struct {
        float x;
        float y;
        float width;
        float height;
    };
    struct {
        mfm_v2 xy;
        float __ignore3;
        float __ignore4;
    };
    float e[4];
};

union mfm_m4
{
    struct {
        float m[16];
    };
};

inline mfm_m4 mfm_m4_identity();
inline mfm_m4 mfm_m4_perspective(float fov, float aspect, float near, float far);
inline mfm_m4 mfm_m4_look_at(mfm_v3 eye, mfm_v3 center, mfm_v3 up);


#ifdef MF_MATH_IMPLEMENTATION

float mfm_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

float mfm_v2_distance(mfm_v2 a, mfm_v2 b)
{
    float res = sqrt(mfm_v2_distance_squared(a, b));
    return res;
}

float mfm_v2_distance_squared(mfm_v2 a, mfm_v2 b)
{
    float sx = (b.x - a.x) * (b.x - a.x);
    float sy = (b.y - a.y) * (b.y - a.y);
    float res = sx + sy;
    return res;
}

float mfm_v2_length(mfm_v2 a)
{
    float res = sqrt(a.x * a.x + a.y * a.y);
    return res;
}

mfm_v2 mfm_v2_normalize(mfm_v2 a)
{
    mfm_v2 res = {0};
    float length = mfm_v2_length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    return res;
}

mfm_v2 mfm_v2_subtract(mfm_v2 a, mfm_v2 b)
{
    mfm_v2 res = {0};
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    return res;
}

mfm_v2 mfm_v2_mul(mfm_v2 a, mfm_v2 b)
{
    mfm_v2 res = {0};
    res.x = a.x * b.x;
    res.y = a.x * b.y;
    return res;
}

mfm_v2 mfm_v2_mul(mfm_v2 a, float b)
{
    mfm_v2 res = {0};
    res.x = a.x * b;
    res.y = a.y * b;
    return res;
}

mfm_v2 mfm_v2_div(mfm_v2 a, mfm_v2 b)
{
    mfm_v2 res = {0};
    res.x = a.x / b.x;
    res.y = a.y / b.y;
    return res;
}

mfm_v2 mfm_v2_div(mfm_v2 a, float b)
{
    mfm_v2 res = {0};
    res.x = a.x / b;
    res.y = a.y / b;
    return res;
}

// mfm_v2
inline
bool mfm_v2_is_equal(mfm_v2 a, mfm_v2 b)
{
    bool res = a.x == b.x && a.y == b.y;
    return res;
}

mfm_v2i mfm_v2i_add(mfm_v2i a, mfm_v2i b)
{
    mfm_v2i res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    return res;
}


// mfm_v3
//
mfm_v3 mfm_v3_255_to_1(int r, int g, int b)
{
    mfm_v3 res;
    res.x = r / 255.0f;
    res.y = g / 255.0f;
    res.z = b / 255.0f;
    return res;
}


float mfm_v3_length(mfm_v3 a)
{
    float res = 0;
    res = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
    return res;
}

inline
float mfm_v3_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

inline
mfm_v3 mfm_v3_add(mfm_v3 a, mfm_v3 b)
{
    mfm_v3 res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

inline
mfm_v3 mfm_v3_sub(mfm_v3 a, mfm_v3 b)
{
    mfm_v3 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

inline
mfm_v3 mfm_v3_mul(mfm_v3 a, float b)
{
    mfm_v3 res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}

float mfm_v3_dot(mfm_v3 a, mfm_v3 b)
{
    float res = 0;
    res = a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
}

mfm_v3 mfm_v3_normalize(mfm_v3 a)
{
    mfm_v3 res = {};
    float length = mfm_v3_length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    res.z = a.z / length;
    return res;
}

mfm_v3 mfm_v3_cross(mfm_v3 a, mfm_v3 b)
{
    mfm_v3 res = {};
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}

mfm_v3 mfm_v3_negate(mfm_v3 a)
{
    mfm_v3 res = {};
    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
    return res;
}

inline
mfm_v3 mfm_v3_lerp(mfm_v3 a, mfm_v3 b, float t)
{
    mfm_v3 res = mfm_v3_add(a, mfm_v3_mul(mfm_v3_sub(b, a), t));
    return res;
}

inline
mfm_rect mfm_rect_margin_x(mfm_rect r, float value)
{
    mfm_rect res = r;
    res.x += value;
    res.width -= 2.0f * value;
    return res;
}

inline
mfm_rect mfm_rect_margin_y(mfm_rect r, float value)
{
    mfm_rect res = r;
    res.y += value;
    res.height -= 2.0f * value;
    return res;
}

inline
mfm_rect mfm_rect_margin(mfm_rect r, float value)
{
    mfm_rect res = r;
    res = mfm_rect_margin_x(res, value);
    res = mfm_rect_margin_y(res, value);
    return res;
}

inline
mfm_rect mfm_rect_margin(mfm_rect r, float x, float y)
{
    mfm_rect res = r;
    res = mfm_rect_margin_x(res, x);
    res = mfm_rect_margin_y(res, y);
    return res;
}

inline
mfm_rect mfm_rect_margin(mfm_rect r, float left, float top, float right, float bottom)
{
    mfm_rect res = r;
    res.x += left;
    res.y += top;
    res.width -= (right + left);
    res.height -= (bottom + top);
    return res;
}



#ifdef __cplusplus

// Vector2

inline
mfm_v2 operator-(mfm_v2 a, mfm_v2 b)
{
    return mfm_v2_subtract(a, b);
}

inline
mfm_v2 operator*(mfm_v2 a, mfm_v2 b)
{
    return mfm_v2_mul(a, b);
}

inline
mfm_v2 operator*(mfm_v2 a, float b)
{
    return mfm_v2_mul(a, b);
}

inline
mfm_v2 operator/(mfm_v2 a, mfm_v2 b)
{
    return mfm_v2_div(a, b);
}

inline
mfm_v2 operator/(mfm_v2 a, float b)
{
    return mfm_v2_div(a, b);
}

inline
bool operator==(mfm_v2 a, mfm_v2 b)
{
    return mfm_v2_is_equal(a, b);
}

inline
bool operator!=(mfm_v2 a, mfm_v2 b)
{
    return !mfm_v2_is_equal(a, b);
}

inline
mfm_v2i operator+(mfm_v2i a, mfm_v2i b)
{
    return mfm_v2i_add(a, b);
}

inline
mfm_v2i& operator+=(mfm_v2i& a, mfm_v2i& b)
{
    a = a + b;
    return a;
}

// Vector 3
inline
mfm_v3 operator+(mfm_v3 a, mfm_v3 b)
{
    return mfm_v3_add(a, b);
}

inline
mfm_v3 operator-(mfm_v3 a, mfm_v3 b)
{
    return mfm_v3_sub(a, b);
}

inline
mfm_v3 operator*(mfm_v3 a, float b)
{
    return mfm_v3_mul(a, b);
}

mfm_m4 mfm_m4_identity()
{
    mfm_m4 res = {};
    res.m[0 * 4 + 0] = 1.0f;
    res.m[1 * 4 + 1] = 1.0f;
    res.m[2 * 4 + 2] = 1.0f;
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

inline mfm_m4 mfm_m4_ortho(float left, float right, float bottom, float top, float near, float far) {
	mfm_m4 res = {};

    res.m[0 * 4 + 0] = 2.0f / (right - left);
    res.m[1 * 4 + 1] = 2.0f / (top - bottom);
    res.m[2 * 4 + 2] = 2.0f / (near - far);
    res.m[3 * 4 + 3] = 1.0f;

    res.m[3 * 4 + 0] = (left + right) / (left - right);
    res.m[3 * 4 + 1] = (bottom + top) / (bottom - top);
    res.m[3 * 4 + 2] = (far + near) / (near - far);

    return res;
}

inline mfm_m4 mfm_m4_perspective(float fov, float aspect, float near, float far)
{
    mfm_m4 res = {};

#if 0 
    // without aspect
    float s = 1.0f / tanf(fov * (M_PI / 360.0f));
    res.m[0 * 4 + 0] = s;
    res.m[1 * 4 + 1] = s;

    res.m[2 * 4 + 2] = -far / (far - near);
    res.m[2 * 4 + 3] = -1.0f;

    res.m[3 * 4 + 2] = -(far * near) / (far - near);
#else
    float c = 1.0f / (tanf(fov * (M_PI / 360.0f)));
    res.m[0 * 4 + 0] = c / aspect;
    res.m[1 * 4 + 1] =  c;

    res.m[2 * 4 + 2] = (near + far) / (near - far);
    res.m[2 * 4 + 3] = -1.0f;

    res.m[2 * 4 + 2] = (2.0f * far * near) / (near - far);
#endif
    return res;
}

inline mfm_m4 mfm_m4_look_at(mfm_v3 eye, mfm_v3 center, mfm_v3 up)
{
    mfm_v3 zaxis = mfm_v3_normalize(center - eye);
    mfm_v3 xaxis = mfm_v3_normalize(mfm_v3_cross(zaxis, up));
    mfm_v3 yaxis = mfm_v3_cross(xaxis, zaxis);

    mfm_m4 res = {
        xaxis.x, xaxis.y, xaxis.z, -mfm_v3_dot(xaxis, eye),
        yaxis.x, yaxis.y, yaxis.z, -mfm_v3_dot(yaxis, eye),
        zaxis.x, zaxis.y, zaxis.z, -mfm_v3_dot(zaxis, eye),
    };
    return res;
}

#endif
#endif

#ifdef MF_TEST_ACTIVE

TEST("Vector 2 is equal")
{
    mfm_v2 v1{2.0f, 3.0f};
    mfm_v2 v2{2.0f, 3.0f};

    MFT_CHECK(v1 == v2);
    MFT_CHECK(mfm_v2_is_equal(v1, v2));
}

TEST("Vector 2 distance")
{
    mfm_v2 v1{-7, -4};
    mfm_v2 v2{17, 6.5};
    float res = mfm_v2_distance(v1, v2);
    MFT_CHECK_FLOAT(res, 26.196374);
}

TEST("Vector 2 normalize and lenght")
{
    mfm_v2 v1{1, 2};
    mfm_v2 res = mfm_v2_normalize(v1);
    MFT_CHECK_FLOAT(mfm_v2_length(res), 1.0f);
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

#endif // MF_TEST_ACTIVE
#endif // MF_MATH_H
