#pragma once
#include <mf.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace mf {


inline f32 to_rad(f32 degree) {
    return degree * (M_PI / 180);
}

inline f32 to_degree(f32 rad) {
    return rad * (180 / M_PI);
}

MF_Generici T clamp(T x, T min, T max) {
    double res = x;
    if (x < min) {
        res = min;
    }
    if (x > max) {
        res = max;
    }
    return res;
}


template <typename T>
struct v2 {
    T x;
    T y;
};

MF_Generici f32 v2_distance(v2<T> a, v2<T> b);
MF_Generici f32 v2_distance_squared(v2<T> a, v2<T> b);
MF_Generici f32 v2_length(v2<T> a);
MF_Generici v2<T> v2_normalize(v2<T> a);

MF_Generici v2<T> operator+(v2<T> a, v2<T> b);
MF_Generici v2<T> operator-(v2<T> a, v2<T> b);
MF_Generici v2<T> operator*(v2<T> a, v2<T> b);
MF_Generici v2<T> operator/(v2<T> a, v2<T> b);
MF_Generici bool operator==(v2<T> a, v2<T> b);
MF_Generici v2<T> operator*(v2<T> a, T b);
MF_Generici v2<T> operator/(v2<T> a, T b);


template <typename T>
union v3 {
    struct {
        T x;
        T y;
        T z;
    };
    struct {
        T r;
        T g;
        T b;
    };

};

MF_Generici static v3<T> v3_create_255_to_1(i32 r, i32 g, i32 b);
MF_Generici static v3<T> v3_lerp(v3<T> a, v3<T> b, f32 t);
MF_Generici v3<T> operator+(v3<T> a, v3<T> b);
MF_Generici v3<T> operator-(v3<T> a, v3<T> b);
MF_Generici v3<T> operator*(v3<T> a, v3<T> b);
MF_Generici v3<T> operator/(v3<T> a, v3<T> b);
MF_Generici v3<T> operator*(v3<T> a, T b);
MF_Generici v3<T> operator/(v3<T> a, T b);

MF_Generici f32 v3_length(v3<T> a);
MF_Generici f32 v3_length_squared(v3<T> a);
MF_Generici f32 v3_dot(v3<T> a, v3<T> b);
MF_Generici v3<T> v3_normalize(v3<T> a);
MF_Generici v3<T> v3_cross(v3<T> a, v3<T> b);
MF_Generici v3<T> v3_negate(v3<T> a);
MF_Generici bool v3_near_zero(v3<T> a);

template <typename T>
struct v4 {
    T r;
    T g;
    T b;
    T a;
};

template <typename T>
union Rect {
    struct {
        T x;
        T y;
        T width;
        T height;
    };
    struct {
        v2<T> xy;
        T __ignore3;
        T __ignore4;
    };
    T e[4];
};

union m4 {
    struct {
        f32 m[16];
    };

};

inline m4 m4_create_identity();
inline m4 m4_create_ortho(float left, float right, float bottom, float top, float nearr, float farr);
inline m4 m4_create_perspective(f32 fov, f32 aspect, f32 nearr, f32 farr);
inline m4 m4_create_look_at(v3<f32> eye, v3<f32> center, v3<f32> up);



#ifdef MF_MATH_IMPLEMENTATION


MF_Generici f32 v2_distance(v2<T> a, v2<T> b) {
    float res = sqrt(v2_distance_squared(a, b));
    return res;
}

MF_Generici f32 v2_distance_squared(v2<T> a, v2<T> b) {
    float sx = (b.x - a.x) * (b.x - a.x);
    float sy = (b.y - a.y) * (b.y - a.y);
    float res = sx + sy;
    return res;
}


MF_Generici f32 v2_length(v2<T> a) {
    float res = sqrt(a.x * a.x + a.y * a.y);
    return res;
}


MF_Generici v2<T> v2_normalize(v2<T> a) {
    v2<T> res = {0};
    f32 length = v2_length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    return res;
}

MF_Generici v2<T> operator+(v2<T> a, v2<T> b) {
    v2<T> res = {0};
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    return res;
}

MF_Generici v2<T> operator-(v2<T> a, v2<T> b) {
    v2<T> res = {0};
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    return res;
}

MF_Generici v2<T> operator*(v2<T> a, v2<T> b) {
    v2<T> res = {0};
    res.x = a.x * b.x;
    res.y = a.x * b.y;
    return res;
}

MF_Generici v2<T> operator*(v2<T> a, T b) {
    v2<T> res = {0};
    res.x = a.x * b;
    res.y = a.x * b;
    return res;
}

MF_Generici v2<T> operator/(v2<T> a, v2<T> b) {
    v2<T> res = {0};
    res.x = a.x / b.x;
    res.y = a.y / b.y;
    return res;
}

MF_Generici v2<T> operator/(v2<T> a, T b) {
    v2<T> res = {0};
    res.x = a.x / b;
    res.y = a.y / b;
    return res;
}

MF_Generici bool operator==(v2<T> a, v2<T> b) {
    bool res = a.x == b.x && a.y == b.y;
    return res;
}



// v3

MF_Generici v3<T> v3_create_255_to_1(i32 r, i32 g, i32 b) {
    v3<T> res;
    res.x = r / 255.0f;
    res.y = g / 255.0f;
    res.z = b / 255.0f;
    return res;
}

MF_Generici f32 v3_length(v3<T> a) {
    f32 res = 0;
    res = sqrt(v3_length_squared(a));
    return res;
}

MF_Generici f32 v3_length_squared(v3<T> a) {
    float res = (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
    return res;
}

template <typename T>
inline
float v3_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

MF_Generici v3<T> operator+(v3<T> a, v3<T> b) {
    v3<T> res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

MF_Generici v3<T> operator-(v3<T> a, v3<T> b) {
    v3<T> res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

MF_Generici v3<T> operator*(v3<T> a, v3<T> b) {
    v3<T> res;
    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;
    return res;
}

MF_Generici v3<T> operator*(v3<T> a, T b) {
    v3<T> res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}

MF_Generici v3<T> operator/(v3<T> a, T b) {
    v3<T> res;
    res.x = a.x / x;
    res.y = a.y / y;
    res.z = a.z / z;
    return res;
}

MF_Generici f32 v3_dot(v3<T> a, v3<T> b) {
    float res = 0;
    res = a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
}

MF_Generici v3<T> v3_normalize(v3<T> a) {
    v3<T> res = {};
    float length = v3_length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    res.z = a.z / length;
    return res;
}

MF_Generici v3<T> v3_cross(v3<T> a, v3<T> b) {
    v3<T> res = {};
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}

MF_Generici v3<T> v3_negate(v3<T> a) {
    v3<T> res = {};
    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
    return res;
}

MF_Generici bool v3_near_zero(v3<T> a) {
    const f64 s = 1e-8;
    return ((fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s));
}

inline m4 m4_create_identity() {
    m4 res = {};
    res.m[0 * 4 + 0] = 1.0f;
    res.m[1 * 4 + 1] = 1.0f;
    res.m[2 * 4 + 2] = 1.0f;
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

inline m4 m4_create_ortho(float left, float right, float bottom, float top, float nearr, float farr) {
    m4 res = {};

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

inline m4 m4_create_perspective(f32 fov, f32 aspect, f32 nearr, f32 farr) {
    m4 res = {};

#if 0 
    // without aspect
    float s = 1.0f / tanf(fov * (M_PI / 360.0f));
    res.m[0 * 4 + 0] = s;
    res.m[1 * 4 + 1] = s;

    res.m[2 * 4 + 2] = -farr / (farr - nearr);
    res.m[2 * 4 + 3] = -1.0f;

    res.m[3 * 4 + 2] = -(farr * nearr) / (farr - nearr);
#else
    float c = 1.0f / (tanf(fov * (M_PI / 360.0f)));
    res.m[0 * 4 + 0] = c / aspect;
    res.m[1 * 4 + 1] =  c;

    res.m[2 * 4 + 2] = (nearr + farr) / (nearr - farr);
    res.m[2 * 4 + 3] = -1.0f;

    res.m[2 * 4 + 2] = (2.0f * farr * nearr) / (nearr - farr);
#endif
    return res;
}

inline m4 m4_create_look_at(v3<f32> eye, v3<f32> center, v3<f32> up) {
    v3<f32> zaxis = v3_normalize(center - eye);
    v3<f32> xaxis = v3_normalize(v3_cross(zaxis, up));
    v3<f32> yaxis = v3_cross(xaxis, zaxis);

    m4 res = {
        xaxis.x, xaxis.y, xaxis.z, -v3_dot(xaxis, eye),
        yaxis.x, yaxis.y, yaxis.z, -v3_dot(yaxis, eye),
        zaxis.x, zaxis.y, zaxis.z, -v3_dot(zaxis, eye),
    };
    return res;
}



#if 0
Rect mfm_rect_margin_x(mfm_rect r, float value)
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
#endif



#endif

#ifdef MF_TEST_ACTIVE

TEST("clamp") {
    MFT_CHECK(clamp(1.0, 0.0, 100.0) == 1.0);
    MFT_CHECK(clamp(1.0, 2.0, 100.0) == 2.0);
    MFT_CHECK(clamp(110.0, 0.0, 100.0) == 100.0);
}


TEST("v2::operator==") {
    v2<f32> v1{2.0f, 3.0f};
    v2<f32> v2{2.0f, 3.0f};

    MFT_CHECK(v1 == v2);
}

TEST("v2::distance") {
    v2<f32> v1{-7, -4};
    v2<f32> v2{17, 6.5};
    f32 res = v2_distance(v1, v2);
    MFT_CHECK_FLOAT(res, 26.196374);
}

TEST("v2::normalize/length") {
    v2<f32> v1{1, 2};
    v2<f32> res = v2_normalize(v1);
    MFT_CHECK_FLOAT(v2_length(res), 1.0f);
}

TEST("v2<i32>::operator+") {
    v2<i32> v1{2, 3};
    v2<i32> v2{2, 3};

    auto res = v1 + v2;
    MFT_CHECK_INT(res.x, 4);
    MFT_CHECK_INT(res.y, 6);

    v1 = v1 + v2;
    MFT_CHECK_INT(v1.x, 4);
    MFT_CHECK_INT(v1.y, 6);
}

TEST("v3<f32>::create_255_to_1") {
    v3<f32> res = v3_create_255_to_1<f32>(128, 0, 255);

    MFT_CHECK_FLOAT(res.x, 0.5019f);
    MFT_CHECK_FLOAT(res.y, 0.0f);
    MFT_CHECK_FLOAT(res.z, 1.0f);
}

TEST("v3::operator+") {
    v3<f32> v1{1.0f, 2.0f, 3.0f};
    v3<f32> v2{4.0f, 5.0f, 6.0f};

    v3<f32> res = v1 + v2;
    MFT_CHECK_FLOAT(res.x, 5.0f);
    MFT_CHECK_FLOAT(res.y, 7.0f);
    MFT_CHECK_FLOAT(res.z, 9.0f);
}

TEST("v3::operator-") {
    v3<f32> v1{1.0f, 2.0f, 3.0f};
    v3<f32> v2{4.0f, 5.0f, 6.0f};

    v3<f32> res = v1 - v2;
    MFT_CHECK_FLOAT(res.x, -3.0f);
    MFT_CHECK_FLOAT(res.y, -3.0f);
    MFT_CHECK_FLOAT(res.z, -3.0f);
}

TEST("v3::operator*") {
    v3<f32> v1{1.0f, 2.0f, 3.0f};

    v3<f32> res = v1 * 2.0f;
    MFT_CHECK_FLOAT(res.x, 2.0f);
    MFT_CHECK_FLOAT(res.y, 4.0f);
    MFT_CHECK_FLOAT(res.z, 6.0f);
}

#endif // MF_TEST_ACTIVE

}
