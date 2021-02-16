#ifndef MF_MATH_H
#define MF_MATH_H


float mfm_lerp(float a, float b, float t);

typedef struct
{
    float x;
    float y;
} mfm_v2;


bool mfm_v2_is_equal(mfm_v2 a, mfm_v2 b);

typedef struct
{
    int x;
    int y;
} mfm_v2i;

typedef struct
{
    float x;
    float y;
    float z;
} mfm_v3;

mfm_v3 mfm_v3_lerp(mfm_v3 a, mfm_v3 b, float t);
mfm_v3 mfm_v3_add(mfm_v3 a, mfm_v3 b);
mfm_v3 mfm_v3_sub(mfm_v3 a, mfm_v3 b);
mfm_v3 mfm_v3_mul(mfm_v3 a, float b);


typedef struct
{
    float x;
    float y;
    float width;
    float height;
} mfm_rect;

#ifdef MF_MATH_IMPLEMENTATION

float mfm_lerp(float a, float b, float t)
{
    float res = a + (b - a) * t;
    return res;
}

// mfm_v2
inline
bool mfm_v2_is_equal(mfm_v2 a, mfm_v2 b)
{
    bool res = a.x == b.x && a.y == b.y;
    return mfm_v2_is_equal;
}


// mfm_v3

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
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
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

inline
mfm_v3 mfm_v3_lerp(mfm_v3 a, mfm_v3 b, float t)
{
    mfm_v3 res = mfm_v3_add(a, mfm_v3_mul(mfm_v3_sub(b, a), t));
    return res;
}



#ifdef __cplusplus

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

#endif



#endif





#endif // MF_MATH_H
