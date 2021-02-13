#ifndef MATH_H
#define MATH_H

#include "Types.h"

#include <float.h>
#include <math.h>

#define PI (3.14159265359f)

inline Float Min(Float a, Float b)
{
    return a < b ? a : b;
}

inline Float Max(Float a, Float b)
{
    return a > b ? a : b;
}

inline Float RandomFloat()
{
    return ((Float(rand()) / Float(RAND_MAX)) - 0.5f) * 2.0f;
}

struct Float3
{
    Float3() = default;

    explicit Float3(Float Single)
        : x(Single)
        , y(Single)
        , z(Single)
    {
    }

    explicit Float3(Float InX, Float InY, Float InZ)
        : x(InX)
        , y(InY)
        , z(InZ)
    {
    }

    Float3 operator-()
    {
        return Float3(-x, -y, -z);
    }

    Float x = 0.0f;
    Float y = 0.0f;
    Float z = 0.0f;
};

inline Float3 operator*(Float3 LHS, Float RHS)
{
    return Float3(LHS.x * RHS, LHS.y * RHS, LHS.z * RHS);
}

inline Float3 operator*(Float LHS, Float3 RHS)
{
    return Float3(RHS.x * LHS, RHS.y * LHS, RHS.z * LHS);
}

inline Float3 operator/(Float3 LHS, Float RHS)
{
    return Float3(LHS.x / RHS, LHS.y / RHS, LHS.z / RHS);
}

inline Float3 Min(Float3 a, Float3 b)
{
    return Float3(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z));
}

inline Float3 Max(Float3 a, Float3 b)
{
    return Float3(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z));
}

inline Float3 Pow(Float3 a, Float3 b)
{
    return Float3(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z));
}

inline Float3 Saturate(Float3 v)
{
    return Min(Max(v, Float3(0.0f, 0.0f, 0.0f)), Float3(1.0f, 1.0f, 1.0f));
}

inline Float Dot(Float3 LHS, Float3 RHS)
{
    return LHS.x * RHS.x + LHS.y * RHS.y + LHS.z * RHS.z;
}

inline Float3 Cross(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.y * RHS.z - LHS.z * RHS.y, LHS.z * RHS.x - LHS.x * RHS.z, LHS.x * RHS.y - LHS.y * RHS.x);
}

inline Float Length(Float3 v)
{
    return sqrtf(Dot(v, v));
}

inline Float3 Normalize(Float3 v)
{
    return v / Length(v);
}

inline Float3 operator+(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x + RHS.x, LHS.y + RHS.y, LHS.z + RHS.z);
}

inline Float3 operator-(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x - RHS.x, LHS.y - RHS.y, LHS.z - RHS.z);
}

inline Float3 operator*(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x * RHS.x, LHS.y * RHS.y, LHS.z * RHS.z);
}

inline Float3 operator/(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x / RHS.x, LHS.y / RHS.y, LHS.z / RHS.z);
}

inline Float3 Reflect(Float3 v, Float3 n)
{
    return v - (2.0f * Dot(v, n) * n);
}

inline Float3 RandomFloat3()
{
    return Float3(RandomFloat(), RandomFloat(), RandomFloat());
}

inline Float3 RandomUnitFloat3()
{
    return Normalize(RandomFloat3());
}

inline Float3 RandomHemisphereFloat3(Float3 n)
{
    Float3 v = RandomUnitFloat3();
    return Dot(v, n) > 0.0f ? v : -v;
}

struct Float4
{
    Float4() = default;

    explicit Float4(Float Single)
        : x(Single)
        , y(Single)
        , z(Single)
        , w(Single)
    {
    }

    explicit Float4(Float InX, Float InY, Float InZ, Float InW)
        : x(InX)
        , y(InY)
        , z(InZ)
        , w(InW)
    {
    }

    Float4 operator-()
    {
        return Float4(-x, -y, -z, -w);
    }

    Float x = 0.0f;
    Float y = 0.0f;
    Float z = 0.0f;
    Float w = 0.0f;
};

inline Float4 operator*(Float4 LHS, Float RHS)
{
    return Float4(LHS.x * RHS, LHS.y * RHS, LHS.z * RHS, LHS.w * RHS);
}

inline Float4 operator*(Float LHS, Float4 RHS)
{
    return Float4(RHS.x * LHS, RHS.y * LHS, RHS.z * LHS, RHS.w * LHS);
}

inline Float4 operator/(Float4 LHS, Float RHS)
{
    return Float4(LHS.x / RHS, LHS.y / RHS, LHS.z / RHS, LHS.w / RHS);
}

inline Float Dot(Float4 LHS, Float4 RHS)
{
    return LHS.x * RHS.x + LHS.y * RHS.y + LHS.z * RHS.z + LHS.w * RHS.w;
}

inline Float Length(Float4 v)
{
    return sqrt(Dot(v, v));
}

inline Float4 Min(Float4 a, Float4 b)
{
    return Float4(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z), Min(a.w, b.w));
}

inline Float4 Max(Float4 a, Float4 b)
{
    return Float4(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z), Max(a.w, b.w));
}

inline Float4 Pow(Float4 a, Float4 b)
{
    return Float4(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z), pow(a.w, b.w));
}

inline Float4 Saturate(Float4 v)
{
    return Min(Max(v, Float4(0.0f, 0.0f, 0.0f, 0.0f)), Float4(1.0f, 1.0f, 1.0f, 1.0f));
}

inline Float4 Normalize(Float4 v)
{
    return v / Length(v);
}

inline Float4 operator+(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x + RHS.x, LHS.y + RHS.y, LHS.z + RHS.z, LHS.w + RHS.w);
}

inline Float4 operator-(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x - RHS.x, LHS.y - RHS.y, LHS.z - RHS.z, LHS.w - RHS.w);
}

inline Float4 operator*(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x * RHS.x, LHS.y * RHS.y, LHS.z * RHS.z, LHS.w * RHS.w);
}

inline Float4 operator/(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x / RHS.x, LHS.y / RHS.y, LHS.z / RHS.z, LHS.w / RHS.w);
}

inline Float Lerp(Float v0, Float v1, Float t) 
{
    return (1.0f - t) * v0 + t * v1;
}

inline Float3 Lerp(Float3 v0, Float3 v1, Float t) 
{
    return (1.0f - t) * v0 + t * v1;
}

inline Float4 Lerp(Float4 v0, Float4 v1, Float t) 
{
    return (1.0f - t) * v0 + t * v1;
}

inline Float4 RandomFloat4()
{
    return Float4(RandomFloat(), RandomFloat(), RandomFloat(), RandomFloat());
}

#endif