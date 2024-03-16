#ifndef MF_MATH_H
#define MF_MATH_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

typedef float f32;
typedef int32_t i32;
typedef uint32_t u32;

// {{{ basic

f32 clamp(f32 value, f32 min, f32 max) {
    f32 res = value;
    if (value < min) {
        res = min;
    } else if (value > max) {
        res = max;
    }
    return res;
}

// }}}

// {{{ v2

typedef union v2 {
   struct {
       f32 x;
       f32 y;
   };
   f32 m[2];
} v2;

bool v2_eq(v2 a, v2 b);
v2 v2_add(v2 a, v2 b);

float v2_distance(v2 a, v2 b);
float v2_distance_squared(v2 a, v2 b);
float v2_length(v2 a);
v2 v2_normalize(v2 a);

// Implementations

bool v2_eq(v2 a, v2 b) {
    return a.x == b.x && a.y == b.y;
}

v2 v2_add(v2 a, v2 b) {
    v2 res = {0};
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    return res;
}

float v2_distance(v2 a, v2 b) {
    float res = sqrt(v2_distance_squared(a, b));
    return res;
}

float v2_distance_squared(v2 a, v2 b) {
    float sx = (b.x - a.x) * (b.x - a.x);
    float sy = (b.y - a.y) * (b.y - a.y);
    float res = sx + sy;
    return res;
}

float v2_length(v2 a) {
    float res = sqrt(a.x * a.x + a.y * a.y);
    return res;
}

v2 v2_normalize(v2 a) {
    v2 res = {0};
    float length = v2_length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    return res;
}

// }}}

// {{{ v2ui

typedef struct v2i {
    i32 x;
    i32 y;
} v2i;

// }}}

// {{{ v2ui

typedef struct v2ui {
    u32 x;
    u32 y;
} v2ui;

// }}}

// {{{ v3

typedef union v3 {
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
} v3;

// }}}

#if 0

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


mfm_v2 mfm_v2_subtract(mfm_v2 a, mfm_v2 b);
mfm_v2 mfm_v2_mul(mfm_v2 a, mfm_v2 b);
mfm_v2 mfm_v2_scale(mfm_v2 a, float b);
mfm_v2 mfm_v2_div(mfm_v2 a, mfm_v2 b);
mfm_v2 mfm_v2_iscale(mfm_v2 a, float b);



typedef struct
{
    int x;
    int y;

} mfm_v2i;


bool mfm_v2_is_equal(mfm_v2 a, mfm_v2 b);

mfm_v2i mfm_v2i_add(mfm_v2i a, mfm_v2i b);

union v3f {
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
};

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

typedef union mfm_m4
{
    struct {
        float m[16];
    };
} mfm_m4;

inline mfm_m4 mfm_m4_identity();
inline mfm_m4 mfm_m4_perspective(float fov, float aspect, float nearr, float farr);
inline mfm_m4 mfm_m4_look_at(mfm_v3 eye, mfm_v3 center, mfm_v3 up);


#ifdef MF_MATH_IMPLEMENTATION

float mfm_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
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



// mfm_v3

template <typename T>
mfm_v3<T> mfm_v3_255_to_1(int r, int g, int b) {
    mfm_v3<T> res;
    res.x = r / 255.0f;
    res.y = g / 255.0f;
    res.z = b / 255.0f;
    return res;
}

template <typename T>
float mfm_v3_length(mfm_v3<T> a) {
    float res = 0;
    res = sqrt(mfm_v3_length_squared(a));
    return res;
}

template <typename T>
float mfm_v3_length_squared(mfm_v3<T> a) {
    float res = (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
    return res;
}

template <typename T>
inline
float mfm_v3_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

template <typename T>
inline
mfm_v3<T> mfm_v3_add(mfm_v3<T> a, mfm_v3<T> b)
{
    mfm_v3<T> res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

template <typename T>
inline
mfm_v3<T> mfm_v3_sub(mfm_v3<T> a, mfm_v3<T> b)
{
    mfm_v3<T> res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

template <typename T>
inline
mfm_v3<T> mfm_v3_mul(mfm_v3<T> a, float b)
{
    mfm_v3<T> res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}

template <typename T>
mfm_v3<T> mfm_v3_mul(mfm_v3<T> a, mfm_v3<T> b)
{
    mfm_v3<T> res;
    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;
    return res;
}

template <typename T>
mfm_v3<T> mfm_v3_div(mfm_v3<T> a, float b)
{
    mfm_v3<T> res;
    res.x = a.x / b;
    res.y = a.y / b;
    res.z = a.z / b;
    return res;
}

template <typename T>
float mfm_v3_dot(mfm_v3<T> a, mfm_v3<T> b)
{
    float res = 0;
    res = a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
}

template <typename T>
mfm_v3<T> mfm_v3_normalize(mfm_v3<T> a)
{
    mfm_v3<T> res = {};
    float length = mfm_v3_length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    res.z = a.z / length;
    return res;
}

template <typename T>
mfm_v3<T> mfm_v3_cross(mfm_v3<T> a, mfm_v3<T> b)
{
    mfm_v3<T> res = {};
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}

template <typename T>
mfm_v3<T> mfm_v3_negate(mfm_v3<T> a)
{
    mfm_v3<T> res = {};
    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
    return res;
}

template <typename T>
bool mfm_v3_near_zero(mfm_v3<T> a)
{
    const double s = 1e-8;
    return ((fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s));
}

template <typename T>
inline
mfm_v3<T> mfm_v3_lerp(mfm_v3<T> a, mfm_v3<T> b, float t)
{
    mfm_v3<T> res = mfm_v3_add(a, mfm_v3_mul(mfm_v3_sub(b, a), t));
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
template <typename T>
inline
mfm_v3<T> operator+(mfm_v3<T> a, mfm_v3<T> b)
{
    return mfm_v3_add(a, b);
}

template <typename T>
inline
mfm_v3<T> operator-(mfm_v3<T> a, mfm_v3<T> b)
{
    return mfm_v3_sub(a, b);
}

template <typename T>
inline
mfm_v3<T> operator*(mfm_v3<T> a, float b)
{
    return mfm_v3_mul(a, b);
}

template <typename T>
inline
mfm_v3<T> operator*(float b, mfm_v3<T> a)
{
    return mfm_v3_mul(a, b);
}

template <typename T>
inline
mfm_v3<T> operator*(mfm_v3<T> a, double b)
{
    return mfm_v3_mul(a, b);
}

template <typename T>
inline
mfm_v3<T> operator*(double b, mfm_v3<T> a)
{
    return mfm_v3_mul(a, b);
}

template <typename T>
inline
mfm_v3<T> operator*(mfm_v3<T> a, mfm_v3<T> b)
{
    return mfm_v3_mul(a, b);
}

template <typename T>
inline
mfm_v3<T> operator/(mfm_v3<T> a, T b)
{
    return mfm_v3_div(a, b);
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

inline mfm_m4 mfm_m4_ortho(float left, float right, float bottom, float top, float nearr, float farr) {
	mfm_m4 res = {};

    res.m[0 * 4 + 0] = 2.0f / (right - left);
    res.m[1 * 4 + 1] = 2.0f / (top - bottom);
    res.m[2 * 4 + 2] = 2.0f / (nearr - farr);
    res.m[2 * 4 + 2] = 2.0f / nearr;
    res.m[3 * 4 + 3] = 1.0f;

    res.m[3 * 4 + 0] = (left + right) / (left - right);
    res.m[3 * 4 + 1] = (bottom + top) / (bottom - top);
    res.m[3 * 4 + 2] = (farr + nearr) / (nearr - farr);

    return res;
}

inline mfm_m4 mfm_m4_perspective(float fov, float aspect, float nearr, float farr)
{
    mfm_m4 res = {};
    //  TODO: this is column-major?
#if 0 
    // without aspect
    float s = 1.0f / tanf(fov * (M_PI / 360.0f));
    res.m[0 * 4 + 0] = s;
    res.m[1 * 4 + 1] = s;

    res.m[2 * 4 + 2] = -farr / (farr - nearr);
    res.m[2 * 4 + 3] = -1.0f;

    res.m[3 * 4 + 2] = -(farr * nearr) / (farr - nearr);
    float c = 1.0f / (tanf(fov * (M_PI / 360.0f)));
    res.m[0 * 4 + 0] = c / aspect;
    res.m[1 * 4 + 1] =  c;

    res.m[2 * 4 + 2] = (nearr + farr) / (nearr - farr);
    res.m[2 * 4 + 3] = -1.0f;

    res.m[2 * 4 + 2] = (2.0f * farr * nearr) / (nearr - farr);

#else
    float c = 1.0f / (tanf(fov * (M_PI / 360.0f)));
    res.m[0 * 4 + 0] = c / aspect;

    res.m[1 * 4 + 1] = c;

    res.m[2 * 4 + 2] = -((farr + nearr) / (farr - nearr));
    res.m[2 * 4 + 3] = -((2 * farr * nearr) / (farr - nearr));

    res.m[3 * 4 + 2] = -1;
#endif
    return res;
}

template <typename T>
inline mfm_m4 mfm_m4_look_at(mfm_v3<T> eye, mfm_v3<T> center, mfm_v3<T> up)
{
    mfm_v3<T> zaxis = mfm_v3_normalize(center - eye);
    mfm_v3<T> xaxis = mfm_v3_normalize(mfm_v3_cross(zaxis, up));
    mfm_v3<T> yaxis = mfm_v3_normalize(mfm_v3_cross(xaxis, zaxis));
    // NOTE: negate because Opengl zaxis
    zaxis = mfm_v3_negate(zaxis);

    mfm_m4 res = {
        xaxis.x, xaxis.y, xaxis.z, -mfm_v3_dot(xaxis, eye),
        yaxis.x, yaxis.y, yaxis.z, -mfm_v3_dot(yaxis, eye),
        zaxis.x, zaxis.y, zaxis.z, -mfm_v3_dot(zaxis, eye),
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return res;
}

#endif
#endif
#endif

#endif // MF_MATH_H
