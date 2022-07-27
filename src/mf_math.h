#pragma once
#include <mf.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace mf { namespace math {


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

    f32 distance(v2<T> b);
    f32 distance_squared(v2<T> b);
    f32 length();
    v2<T> normalize();

    inline v2<T> operator+(v2<T> b);
    inline v2<T> operator-(v2<T> b);
    inline v2<T> operator*(v2<T> b);
    inline v2<T> operator/(v2<T> b);
    inline bool operator==(v2<T> b);
    inline v2<T> operator*(T b);
    inline v2<T> operator/(T b);
};




template <typename T>
union v3
{
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

    inline static v3<T> create_255_to_1(i32 r, i32 g, i32 b);
    inline static v3<T> v3_lerp(v3<T> a, v3<T> b, f32 t);
    inline v3<T> operator+(v3<T> b);
    inline v3<T> operator-(v3<T> b);
    inline v3<T> operator*(v3<T> b);
    inline v3<T> operator/(v3<T> b);
    inline v3<T> operator*(T b);
    inline v3<T> operator/(T b);

    f32 length();
    f32 length_squared();
    f32 dot(v3<T> b);
    v3<T> normalize();
    v3<T> cross(v3<T> b);
    v3<T> negate();
    bool near_zero();
};

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

    static inline m4 create_identity() {
        m4 res = {};
        res.m[0 * 4 + 0] = 1.0f;
        res.m[1 * 4 + 1] = 1.0f;
        res.m[2 * 4 + 2] = 1.0f;
        res.m[3 * 4 + 3] = 1.0f;
        return res;
    }
    static inline m4 create_ortho(float left, float right, float bottom, float top, float nearr, float farr) {
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
    static inline m4 create_perspective(f32 fov, f32 aspect, f32 nearr, f32 farr) {
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
    static inline m4 create_look_at(v3<f32> eye, v3<f32> center, v3<f32> up) {
        v3<f32> zaxis = (center - eye).normalize();
        v3<f32> xaxis = zaxis.cross(up).normalize();
        v3<f32> yaxis = xaxis.cross(zaxis);

        m4 res = {
            xaxis.x, xaxis.y, xaxis.z, -xaxis.dot(eye),
            yaxis.x, yaxis.y, yaxis.z, -yaxis.dot(eye),
            zaxis.x, zaxis.y, zaxis.z, -zaxis.dot(eye),
        };
        return res;
    }
};




#ifdef MF_MATH_IMPLEMENTATION


template <typename T>
inline f32 v2<T>::distance(v2<T> b) {
    float res = sqrt(this->distance_squared(b));
    return res;
}

MF_Generic inline f32 v2<T>::distance_squared(v2<T> b) {
    float sx = (b.x - this->x) * (b.x - this->x);
    float sy = (b.y - this->y) * (b.y - this->y);
    float res = sx + sy;
    return res;
}


MF_Generici f32 v2<T>::length() {
    float res = sqrt(this->x * this->x + this->y * this->y);
    return res;
}


MF_Generici v2<T> v2<T>::normalize() {
    v2 res = {0};
    f32 length = this->length();
    res.x = this->x / length;
    res.y = this->y / length;
    return res;
}

MF_Generici v2<T> v2<T>::operator+(v2<T> b) {
    v2 res = {0};
    res.x = this->x + b.x;
    res.y = this->y + b.y;
    return res;
}

MF_Generici v2<T> v2<T>::operator-(v2<T> b) {
    v2 res = {0};
    res.x = this->x - b.x;
    res.y = this->y - b.y;
    return res;
}

MF_Generici v2<T> v2<T>::operator*(v2<T> b) {
    v2 res = {0};
    res.x = this->x * b.x;
    res.y = this->x * b.y;
    return res;
}

MF_Generici v2<T> v2<T>::operator*(T b) {
    v2 res = {0};
    res.x = this->x * b;
    res.y = this->x * b;
    return res;
}

MF_Generici v2<T> v2<T>::operator/(v2<T> b) {
    v2 res = {0};
    res.x = this->x / b.x;
    res.y = this->y / b.y;
    return res;
}

MF_Generici v2<T> v2<T>::operator/(T b) {
    v2 res = {0};
    res.x = this->x / b;
    res.y = this->y / b;
    return res;
}

MF_Generici bool v2<T>::operator==(v2<T> b) {
    bool res = this->x == b.x && this->y == b.y;
    return res;
}



// v3

MF_Generici v3<T> v3<T>::create_255_to_1(i32 r, i32 g, i32 b) {
    v3<T> res;
    res.x = r / 255.0f;
    res.y = g / 255.0f;
    res.z = b / 255.0f;
    return res;
}

MF_Generici f32 v3<T>::length() {
    f32 res = 0;
    res = sqrt(this->length_squared());
    return res;
}

MF_Generici f32 v3<T>::length_squared() {
    float res = (this->x * this->x) + (this->y * this->y) + (this->z * this->z);
    return res;
}

template <typename T>
inline
float v3_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

MF_Generici v3<T> v3<T>::operator+(v3<T> b) {
    v3<T> res;
    res.x = this->x + b.x;
    res.y = this->y + b.y;
    res.z = this->z + b.z;
    return res;
}

MF_Generici v3<T> v3<T>::operator-(v3<T> b) {
    v3<T> res;
    res.x = this->x - b.x;
    res.y = this->y - b.y;
    res.z = this->z - b.z;
    return res;
}

MF_Generici v3<T> v3<T>::operator*(v3<T> b) {
    v3<T> res;
    res.x = this->x * b.x;
    res.y = this->y * b.y;
    res.z = this->z * b.z;
    return res;
}

MF_Generici v3<T> v3<T>::operator*(T b) {
    v3<T> res;
    res.x = this->x * b;
    res.y = this->y * b;
    res.z = this->z * b;
    return res;
}

MF_Generici v3<T> v3<T>::operator/(T b) {
    v3<T> res;
    res.x = this->x / x;
    res.y = this->y / y;
    res.z = this->z / z;
    return res;
}

MF_Generici f32 v3<T>::dot(v3<T> b) {
    float res = 0;
    res = this->x * b.x + this->y * b.y + this->z * b.z;
    return res;
}

MF_Generici v3<T> v3<T>::normalize()
{
    v3<T> res = {};
    float length = this->length();
    res.x = this->x / length;
    res.y = this->y / length;
    res.z = this->z / length;
    return res;
}

MF_Generici v3<T> v3<T>::cross(v3<T> b) {
    v3<T> res = {};
    res.x = this->y * b.z - this->z * b.y;
    res.y = this->z * b.x - this->x * b.z;
    res.z = this->x * b.y - this->y * b.x;
    return res;
}

MF_Generici v3<T> v3<T>::negate() {
    v3<T> res = {};
    res.x = -this->x;
    res.y = -this->y;
    res.z = -this->z;
    return res;
}

MF_Generici bool v3<T>::near_zero() {
    const f64 s = 1e-8;
    return ((fabs(this->x) < s) && (fabs(this->y) < s) && (fabs(this->z) < s));
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
    f32 res = v1.distance(v2);
    MFT_CHECK_FLOAT(res, 26.196374);
}

TEST("v2::normalize/length") {
    v2<f32> v1{1, 2};
    v2<f32> res = v1.normalize();
    MFT_CHECK_FLOAT(res.length(), 1.0f);
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
    v3<f32> res = v3<f32>::create_255_to_1(128, 0, 255);

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

TEST("v3::operator*")
{
    v3<f32> v1{1.0f, 2.0f, 3.0f};

    v3<f32> res = v1 * 2.0f;
    MFT_CHECK_FLOAT(res.x, 2.0f);
    MFT_CHECK_FLOAT(res.y, 4.0f);
    MFT_CHECK_FLOAT(res.z, 6.0f);
}

#endif // MF_TEST_ACTIVE

}} // mf::math
