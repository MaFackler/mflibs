#ifndef MF_MATH_H
#define MF_MATH_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <stdint.h>


#define MFM_Sin sin
#define MFM_Cos cos
#define MFM_Pi M_PI
#define MFM_Pi2 (2 * M_PI)
#define API static inline
API float MFM_DegToRad(float degree) {
    return degree * (M_PI / 180);
}

API float MFM_RadToDeg(float rad) {
    return rad * (180 / M_PI);
}

API float MFM_Wrap(float value, float min, float max) {
    if (value > max) {
        value -= (max - min);
    }
    return value;
}


// {{{ basic

API float MFM_Clamp(float value, float min, float max) {
    float res = value;
    if (value < min) {
        res = min;
    } else if (value > max) {
        res = max;
    }
    return res;
}

// }}}

// {{{ MFM_Vec2

typedef union MFM_Vec2 {
   float m[2];
   struct {
       float x;
       float y;
   };
} MFM_Vec2;

// API bool MFM_EqualV2(MFM_Vec2 a, MFM_Vec2 b);
API MFM_Vec2 MFM_Vec2Add(MFM_Vec2 a, MFM_Vec2 b);

API float MFM_Vec2Distance(MFM_Vec2 a, MFM_Vec2 b);
API float MFM_Vec2DistanceSquared(MFM_Vec2 a, MFM_Vec2 b);
API float MFM_Vec2Length(MFM_Vec2 a);
API MFM_Vec2 MFM_Vec2Normalize(MFM_Vec2 a);

// Implementations

API bool MFM_Vec2Equal(MFM_Vec2 a, MFM_Vec2 b) {
    return a.x == b.x && a.y == b.y;
}

API MFM_Vec2 MFM_Vec2Add(MFM_Vec2 a, MFM_Vec2 b) {
    MFM_Vec2 res = {0};
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    return res;
}

API float MFM_Vec2Distance(MFM_Vec2 a, MFM_Vec2 b) {
    float res = sqrt(MFM_Vec2DistanceSquared(a, b));
    return res;
}

API float MFM_Vec2DistanceSquared(MFM_Vec2 a, MFM_Vec2 b) {
    float sx = (b.x - a.x) * (b.x - a.x);
    float sy = (b.y - a.y) * (b.y - a.y);
    float res = sx + sy;
    return res;
}

API float MFM_Vec2Length(MFM_Vec2 a) {
    float res = sqrt(a.x * a.x + a.y * a.y);
    return res;
}

API MFM_Vec2 MFM_Vec2Normalize(MFM_Vec2 a) {
    MFM_Vec2 res = {0};
    float length = MFM_Vec2Length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    return res;
}

// }}}

// {{{ MFM_Vec2ui

typedef struct MFM_Vec2i {
    int x;
    int y;
} MFM_Vec2i;

// }}}

// {{{ MFM_Vec2ui

typedef struct MFM_Vec2ui {
    unsigned int x;
    unsigned int y;
} MFM_Vec2ui;

// }}}

// {{{ MFM_Vec3

typedef union MFM_Vec3 {
    struct {
        float x;
        float y;
        float z;
    };
} MFM_Vec3;

API float MFM_Vec3LengthSquared(MFM_Vec3 a);
API float MFM_Vec3Length(MFM_Vec3 a);
API MFM_Vec3 MFM_Vec3All(float value);
API MFM_Vec3 MFM_Vec3Negate(MFM_Vec3 a);
API MFM_Vec3 MFM_Vec3Normalize(MFM_Vec3 a);
API MFM_Vec3 MFM_Vec3Cross(MFM_Vec3 a, MFM_Vec3 b);
API float MFM_Vec3Dot(MFM_Vec3 a, MFM_Vec3 b);
API MFM_Vec3 MFM_Vec3Add(MFM_Vec3 a, MFM_Vec3 b);
API MFM_Vec3 MFM_Vec3Sub(MFM_Vec3 a, MFM_Vec3 b);
API MFM_Vec3 MFM_Vec3Mul(MFM_Vec3 a, MFM_Vec3 b);
API MFM_Vec3 MFM_Vec3Div(MFM_Vec3 a, MFM_Vec3 b);
API MFM_Vec3 MFM_Vec3Scale(MFM_Vec3 a, float b);
API bool MFM_Vec3NearZero(MFM_Vec3 a);

// }}}

// {{{ MFM_Vec3i
typedef struct MFM_Vec3i {
    int x;
    int y;
    int z;
} MFM_Vec3i;
// }}} MFM_Vec3i


// {{{ MFM_Vec4

typedef union MFM_Vec4 {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float m[4];
    };
} MFM_Vec4;

// }}}

// {{{ MFM_Mat4
typedef union MFM_Mat4 {
    struct {
        float m[16];
    };
    struct {
        MFM_Vec4 rows[4];
    };
} MFM_Mat4;

API MFM_Mat4 MFM_Mat4Identity();
API MFM_Mat4 MFM_Mat4Translate(float x, float y, float z);
API MFM_Mat4 MFM_Mat4TranslateV(MFM_Vec3 offset);
API MFM_Mat4 MFM_Mat4RotateX(float angle);
API MFM_Mat4 MFM_Mat4RotateY(float angle);
API MFM_Mat4 MFM_Mat4RotateZ(float angle);
API MFM_Mat4 MFM_Mat4Scale(MFM_Vec3 diagonal);
API MFM_Mat4 MFM_Mat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
API MFM_Mat4 MFM_Mat4Perspective(float left, float right, float bottom, float top, float zNear, float zFar);
API MFM_Mat4 MFM_Mat4PerspectiveFov(float fov, float aspectRatio, float zNear, float zFar);
API MFM_Mat4 MFM_Mat4LookAt(MFM_Vec3 eye, MFM_Vec3 at, MFM_Vec3 up);
API MFM_Mat4 MFM_Mat4Mul(MFM_Mat4 a, MFM_Mat4 b);
API MFM_Vec3 MFM_Mat4MulVec3(MFM_Mat4 m, MFM_Vec3 v);

// }}}

// {{{ Implementation

#if defined(MF_MATH_IMPLEMENTATION) || defined(MF_IMPLEMENTATION)

// {{{ MFM_Vec3
//

API float MFM_Vec3LengthSquared(MFM_Vec3 a) {
    float res = (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
    return res;
}

API float MFM_Vec3Length(MFM_Vec3 a) {
    float res = 0;
    res = sqrt(MFM_Vec3LengthSquared(a));
    return res;
}

API MFM_Vec3 MFM_Vec3All(float value) {
    return (MFM_Vec3) {value, value, value};
}

API MFM_Vec3 MFM_Vec3Negate(MFM_Vec3 a) {
    MFM_Vec3 res = {};
    res.x = -a.x;
    res.y = -a.y;
    res.z = -a.z;
    return res;
}

API MFM_Vec3 MFM_Vec3Normalize(MFM_Vec3 a) {
    MFM_Vec3 res = {};
    float length = MFM_Vec3Length(a);
    res.x = a.x / length;
    res.y = a.y / length;
    res.z = a.z / length;
    return res;
}

API MFM_Vec3 MFM_Vec3Cross(MFM_Vec3 a, MFM_Vec3 b) {
    MFM_Vec3 res = {};
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}

API float MFM_Vec3Dot(MFM_Vec3 a, MFM_Vec3 b) {
    float res = 0;
    res = a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
}

API MFM_Vec3 MFM_Vec3Add(MFM_Vec3 a, MFM_Vec3 b) {
    MFM_Vec3 res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

API MFM_Vec3 MFM_Vec3Sub(MFM_Vec3 a, MFM_Vec3 b) {
    MFM_Vec3 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

API MFM_Vec3 MFM_Vec3Mul(MFM_Vec3 a, MFM_Vec3 b) {
    MFM_Vec3 res;
    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;
    return res;
}

API MFM_Vec3 MFM_Vec3Div(MFM_Vec3 a, MFM_Vec3 b) {
    MFM_Vec3 res;
    res.x = a.x / b.x;
    res.y = a.y / b.y;
    res.z = a.z / b.z;
    return res;
}

API MFM_Vec3 MFM_Vec3Scale(MFM_Vec3 a, float b) {
    MFM_Vec3 res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}

API bool MFM_Vec3NearZero(MFM_Vec3 a) {
    const double s = 1e-8;
    return ((fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s));
}

// }}}

// {{{ MFM_Mat4

API MFM_Mat4 MFM_Mat4Identity() {
    MFM_Mat4 res = {0};
    res.m[0 * 4 + 0] = 1.0f;
    res.m[1 * 4 + 1] = 1.0f;
    res.m[2 * 4 + 2] = 1.0f;
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

API MFM_Mat4 MFM_Mat4Translate(float x, float y, float z) {
    MFM_Mat4 res = MFM_Mat4Identity();
    res.m[0 * 4 + 3] = x;
    res.m[1 * 4 + 3] = y;
    res.m[2 * 4 + 3] = z;
    return res;
}

API MFM_Mat4 MFM_Mat4TranslateV(MFM_Vec3 offset) {
    MFM_Mat4 res = MFM_Mat4Translate(offset.x,
                                     offset.y,
                                     offset.z);
    return res;
}

API MFM_Mat4 MFM_Mat4RotateAxis(MFM_Vec3 axis, float angle) {
    MFM_Mat4 res = MFM_Mat4Identity();

    axis = MFM_Vec3Normalize(axis);

    float sinv = MFM_Sin(angle);
    float cosv = MFM_Cos(angle);
    float cosi = 1.0f - cosv;

    res.rows[0].m[0] = cosv + (axis.x * axis.x * cosi);
    res.rows[0].m[1] = (axis.x * axis.y * cosi) - (axis.z * sinv);
    res.rows[0].m[2] = (axis.x * axis.z * cosi) + (axis.y * sinv);

    res.rows[1].m[0] = (axis.y * axis.x * cosi) + (axis.z * sinv);
    res.rows[1].m[1] = cosv + (axis.y * axis.y * cosi);
    res.rows[1].m[2] = (axis.y * axis.z * cosi) - (axis.x * sinv);

    res.rows[2].m[0] = (axis.z * axis.x * cosi) - (axis.y * sinv);
    res.rows[2].m[1] = (axis.z * axis.y * cosi) + (axis.x * sinv);
    res.rows[2].m[2] = cosv + (axis.z * axis.z * cosi);

    return res;
}

API MFM_Mat4 MFM_Mat4RotateX(float angle) {
    MFM_Mat4 res = {0};
    res.m[0 * 4 + 0] = 1.0f;
    res.m[1 * 4 + 1] = MFM_Cos(angle);
    res.m[1 * 4 + 2] = -MFM_Sin(angle);
    res.m[2 * 4 + 1] = MFM_Sin(angle);
    res.m[2 * 4 + 2] = MFM_Cos(angle);
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

API MFM_Mat4 MFM_Mat4RotateY(float angle) {
    MFM_Mat4 res = {0};
    res.m[0 * 4 + 0] = MFM_Cos(angle);
    res.m[0 * 4 + 2] = MFM_Sin(angle);
    res.m[1 * 4 + 1] = 1.0f;
    res.m[2 * 4 + 0] = -MFM_Sin(angle);
    res.m[2 * 4 + 2] = MFM_Cos(angle);
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

API MFM_Mat4 MFM_Mat4RotateZ(float angle) {
    MFM_Mat4 res = {0};
    res.m[0 * 4 + 0] = MFM_Cos(angle);
    res.m[0 * 4 + 1] = -MFM_Sin(angle);
    res.m[1 * 4 + 0] = MFM_Sin(angle);
    res.m[1 * 4 + 1] = MFM_Cos(angle);
    res.m[2 * 4 + 2] = 1.0f;
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

API MFM_Mat4 MFM_Mat4Scale(MFM_Vec3 diagonal) {
    MFM_Mat4 res = {0};
    res.m[0 * 4 + 0] = diagonal.x;
    res.m[1 * 4 + 1] = diagonal.y;
    res.m[2 * 4 + 2] = diagonal.z;
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

API MFM_Mat4 MFM_Mat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
	MFM_Mat4 res = {0};
    res.m[0 * 4 + 0] = 2.0f / (right - left);
    res.m[0 * 4 + 3] = - (right + left) / (right - left);
    res.m[1 * 4 + 1] = 2.0f / (top - bottom);
    res.m[1 * 4 + 3] = - (top + bottom) / (top - bottom);
    res.m[2 * 4 + 2] = - 2.0f / (zFar - zNear);  // NOTE: OpenGL negation
    res.m[2 * 4 + 3] = - (zFar + zNear) / (zFar - zNear);
    res.m[3 * 4 + 3] = 1.0f;
    return res;
}

API MFM_Mat4 MFM_Mat4Perspective(float left, float right, float bottom, float top, float zNear, float zFar) {
    MFM_Mat4 res = {};
#if 1
    res.m[0 * 4 + 0] = (2 * zNear) / (right - left);
    res.m[0 * 4 + 2] = (right + left) / (right - left);
    res.m[1 * 4 + 1] = (2 * zNear) / (top - bottom);
    res.m[1 * 4 + 2] = (top + bottom) / (top - bottom);
    res.m[2 * 4 + 2] = (zNear + zFar) / (zNear - zFar);
    res.m[2 * 4 + 3] = (2 * zNear * zFar) / (zNear - zFar);
    res.m[3 * 4 + 2] = -1.0f;
#else
    res.m[0 * 4 + 0] = (2 * zNear) / (right - left);
    res.m[0 * 4 + 2] = (right + left) / (right - left);
    res.m[1 * 4 + 1] = (2 * zNear) / (top - bottom);
    res.m[1 * 4 + 2] = (top + bottom) / (top - bottom);
    res.m[2 * 4 + 2] = (zNear + zFar) / (zNear - zFar);
    res.m[3 * 4 + 2] = -1.0f;
#endif


    return res;
}

API MFM_Mat4 MFM_Mat4PerspectiveFov(float fov, float aspectRatio, float zNear, float zFar) {
    MFM_Mat4 res = {0};
    float c = 1.0f / tanf(fov / 2.0f);
    res.m[0 * 4 + 0] = c / aspectRatio;
    res.m[1 * 4 + 1] = c;

    res.m[2 * 4 + 2] = (zNear + zFar) / (zNear - zFar);
    res.m[2 * 4 + 3] = (2.0f * zNear * zFar) / (zNear - zFar);

    res.m[3 * 4 + 2] = -1.0f;

    return res;
}

API MFM_Mat4 MFM_Mat4LookAt(MFM_Vec3 eye, MFM_Vec3 at, MFM_Vec3 up) {
    MFM_Vec3 zaxis = MFM_Vec3Normalize(MFM_Vec3Sub(at, eye));
    MFM_Vec3 xaxis = MFM_Vec3Normalize(MFM_Vec3Cross(zaxis, up));
    MFM_Vec3 yaxis = MFM_Vec3Normalize(MFM_Vec3Cross(xaxis, zaxis));

    MFM_Mat4 res = (MFM_Mat4) {
        xaxis.x, xaxis.y, xaxis.z, -MFM_Vec3Dot(xaxis, eye),
        yaxis.x, yaxis.y, yaxis.z, -MFM_Vec3Dot(yaxis, eye),
        -zaxis.x, -zaxis.y, -zaxis.z, MFM_Vec3Dot(zaxis, eye),
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return res;
}

API MFM_Mat4 MFM_Mat4Mul(MFM_Mat4 a, MFM_Mat4 b) {
    MFM_Mat4 res = {0};

#define __Mat4_Calc(x, y) \
    res.m[y * 4 + x] = a.m[y * 4 + 0] * b.m[0 * 4 + x] + \
                       a.m[y * 4 + 1] * b.m[1 * 4 + x] + \
                       a.m[y * 4 + 2] * b.m[2 * 4 + x] + \
                       a.m[y * 4 + 3] * b.m[3 * 4 + x]

    __Mat4_Calc(0, 0);
    __Mat4_Calc(1, 0);
    __Mat4_Calc(2, 0);
    __Mat4_Calc(3, 0);
    __Mat4_Calc(0, 1);
    __Mat4_Calc(1, 1);
    __Mat4_Calc(2, 1);
    __Mat4_Calc(3, 1);
    __Mat4_Calc(0, 2);
    __Mat4_Calc(1, 2);
    __Mat4_Calc(2, 2);
    __Mat4_Calc(3, 2);
    __Mat4_Calc(0, 3);
    __Mat4_Calc(1, 3);
    __Mat4_Calc(2, 3);
    __Mat4_Calc(3, 3);

#undef __Mat4_Calc
    return res;
}

API MFM_Vec3 MFM_Mat4MulVec3(MFM_Mat4 m, MFM_Vec3 v) {
    MFM_Vec3 res = {0};
    res.x = m.m[0 * 4 + 0] * v.x +
            m.m[0 * 4 + 1] * v.y + 
            m.m[0 * 4 + 2] * v.z +
            m.m[0 * 4 + 3] * 1.0f;
    res.y = m.m[1 * 4 + 0] * v.x +
            m.m[1 * 4 + 1] * v.y + 
            m.m[1 * 4 + 2] * v.z +
            m.m[1 * 4 + 3] * 1.0f;
    res.z = m.m[2 * 4 + 0] * v.x +
            m.m[2 * 4 + 1] * v.y + 
            m.m[2 * 4 + 2] * v.z +
            m.m[2 * 4 + 3] * 1.0f;
    return res;
}

// }}}

#endif
// }}}

#if 0


float mfm_lerp(float a, float b, float t);

typedef struct
{
    float x;
    float y;
} mfm_MFM_Vec2;


mfm_MFM_Vec2 mfm_MFM_Vec2_subtract(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b);
mfm_MFM_Vec2 mfm_MFM_Vec2_mul(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b);
mfm_MFM_Vec2 mfm_MFM_Vec2_scale(mfm_MFM_Vec2 a, float b);
mfm_MFM_Vec2 mfm_MFM_Vec2_div(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b);
mfm_MFM_Vec2 mfm_MFM_Vec2_iscale(mfm_MFM_Vec2 a, float b);



typedef struct
{
    int x;
    int y;

} mfm_MFM_Vec2i;


bool mfm_MFM_Vec2_is_equal(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b);

mfm_MFM_Vec2i mfm_MFM_Vec2i_add(mfm_MFM_Vec2i a, mfm_MFM_Vec2i b);

union MFM_Vec3f {
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
} mfm_MFM_Vec4;





union mfm_rect
{
    struct {
        float x;
        float y;
        float width;
        float height;
    };
    struct {
        mfm_MFM_Vec2 xy;
        float __ignore3;
        float __ignore4;
    };
    float e[4];
};




#ifdef MF_MATH_IMPLEMENTATION

float mfm_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}




mfm_MFM_Vec2 mfm_MFM_Vec2_subtract(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    mfm_MFM_Vec2 res = {0};
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    return res;
}

mfm_MFM_Vec2 mfm_MFM_Vec2_mul(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    mfm_MFM_Vec2 res = {0};
    res.x = a.x * b.x;
    res.y = a.x * b.y;
    return res;
}

mfm_MFM_Vec2 mfm_MFM_Vec2_mul(mfm_MFM_Vec2 a, float b)
{
    mfm_MFM_Vec2 res = {0};
    res.x = a.x * b;
    res.y = a.y * b;
    return res;
}

mfm_MFM_Vec2 mfm_MFM_Vec2_div(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    mfm_MFM_Vec2 res = {0};
    res.x = a.x / b.x;
    res.y = a.y / b.y;
    return res;
}

mfm_MFM_Vec2 mfm_MFM_Vec2_div(mfm_MFM_Vec2 a, float b)
{
    mfm_MFM_Vec2 res = {0};
    res.x = a.x / b;
    res.y = a.y / b;
    return res;
}

// mfm_MFM_Vec2
inline
bool mfm_MFM_Vec2_is_equal(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    bool res = a.x == b.x && a.y == b.y;
    return res;
}



// mfm_MFM_Vec3

template <typename T>
mfm_MFM_Vec3<T> mfm_MFM_Vec3_255_to_1(int r, int g, int b) {
    mfm_MFM_Vec3<T> res;
    res.x = r / 255.0f;
    res.y = g / 255.0f;
    res.z = b / 255.0f;
    return res;
}



template <typename T>
inline
float mfm_MFM_Vec3_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

template <typename T>
inline
mfm_MFM_Vec3<T> mfm_MFM_Vec3_add(mfm_MFM_Vec3<T> a, mfm_MFM_Vec3<T> b)
{
    mfm_MFM_Vec3<T> res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}


template <typename T>
inline
mfm_MFM_Vec3<T> mfm_MFM_Vec3_mul(mfm_MFM_Vec3<T> a, float b)
{
    mfm_MFM_Vec3<T> res;
    res.x = a.x * b;
    res.y = a.y * b;
    res.z = a.z * b;
    return res;
}

template <typename T>
mfm_MFM_Vec3<T> mfm_MFM_Vec3_mul(mfm_MFM_Vec3<T> a, mfm_MFM_Vec3<T> b)
{
    mfm_MFM_Vec3<T> res;
    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;
    return res;
}

template <typename T>
mfm_MFM_Vec3<T> mfm_MFM_Vec3_div(mfm_MFM_Vec3<T> a, float b)
{
    mfm_MFM_Vec3<T> res;
    res.x = a.x / b;
    res.y = a.y / b;
    res.z = a.z / b;
    return res;
}




template <typename T>
inline
mfm_MFM_Vec3<T> mfm_MFM_Vec3_lerp(mfm_MFM_Vec3<T> a, mfm_MFM_Vec3<T> b, float t)
{
    mfm_MFM_Vec3<T> res = mfm_MFM_Vec3_add(a, mfm_MFM_Vec3_mul(mfm_MFM_Vec3_sub(b, a), t));
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
mfm_MFM_Vec2 operator-(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    return mfm_MFM_Vec2_subtract(a, b);
}

inline
mfm_MFM_Vec2 operator*(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    return mfm_MFM_Vec2_mul(a, b);
}

inline
mfm_MFM_Vec2 operator*(mfm_MFM_Vec2 a, float b)
{
    return mfm_MFM_Vec2_mul(a, b);
}

inline
mfm_MFM_Vec2 operator/(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    return mfm_MFM_Vec2_div(a, b);
}

inline
mfm_MFM_Vec2 operator/(mfm_MFM_Vec2 a, float b)
{
    return mfm_MFM_Vec2_div(a, b);
}

inline
bool operator==(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    return mfm_MFM_Vec2_is_equal(a, b);
}

inline
bool operator!=(mfm_MFM_Vec2 a, mfm_MFM_Vec2 b)
{
    return !mfm_MFM_Vec2_is_equal(a, b);
}

inline
mfm_MFM_Vec2i operator+(mfm_MFM_Vec2i a, mfm_MFM_Vec2i b)
{
    return mfm_MFM_Vec2i_add(a, b);
}

inline
mfm_MFM_Vec2i& operator+=(mfm_MFM_Vec2i& a, mfm_MFM_Vec2i& b)
{
    a = a + b;
    return a;
}

// Vector 3
template <typename T>
inline
mfm_MFM_Vec3<T> operator+(mfm_MFM_Vec3<T> a, mfm_MFM_Vec3<T> b)
{
    return mfm_MFM_Vec3_add(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator-(mfm_MFM_Vec3<T> a, mfm_MFM_Vec3<T> b)
{
    return mfm_MFM_Vec3_sub(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator*(mfm_MFM_Vec3<T> a, float b)
{
    return mfm_MFM_Vec3_mul(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator*(float b, mfm_MFM_Vec3<T> a)
{
    return mfm_MFM_Vec3_mul(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator*(mfm_MFM_Vec3<T> a, double b)
{
    return mfm_MFM_Vec3_mul(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator*(double b, mfm_MFM_Vec3<T> a)
{
    return mfm_MFM_Vec3_mul(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator*(mfm_MFM_Vec3<T> a, mfm_MFM_Vec3<T> b)
{
    return mfm_MFM_Vec3_mul(a, b);
}

template <typename T>
inline
mfm_MFM_Vec3<T> operator/(mfm_MFM_Vec3<T> a, T b)
{
    return mfm_MFM_Vec3_div(a, b);
}


#endif
#endif
#endif

#undef API

#ifdef MF_MATH_SHORT_NAMES
typedef MFM_Vec2 Vec2;
typedef MFM_Vec3 Vec3;
typedef MFM_Vec4 Vec4;
typedef MFM_Mat4 Mat4;
#endif

#endif // MF_MATH_H
