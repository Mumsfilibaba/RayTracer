#ifndef MATH_H
#define MATH_H

#include "Types.h"

#include <Float.h>
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

    Float3(Float Single)
        : x(Single)
        , y(Single)
        , z(Single)
    {
    }

    Float3(Float InX, Float InY, Float InZ)
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

struct Float4
{
    Float4() = default;

    Float4(Float Single)
        : x(Single)
        , y(Single)
        , z(Single)
        , w(Single)
    {
    }

    Float4(Float InX, Float InY, Float InZ, Float InW)
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

Float DistributionGGX(Float3 N, Float3 H, float Roughness)
{
    Float Alpha  = Roughness * Roughness;
    Float Alpha2 = Alpha * Alpha;
    Float NDotH  = Max(Dot(N, H), 0.0000001f);
    Float Denominator = NDotH * NDotH * (Alpha2 - 1.0f) + 1.0f;
    return Alpha2 / Max(PI * Denominator * Denominator, 0.0000001f);
}

Float3 FresnelSchlick(Float3 F0, Float3 V, Float3 H)
{
    Float VDotH = Max(Dot(V, H), 0.0000001f);
    Float Exp   = (-5.55473f * VDotH - 6.98316f) * VDotH;
    return F0 + (1.0f - F0) * exp2(Exp);
}

Float GeometrySmithGGX1(Float3 N, Float3 V, Float3 H, float Roughness)
{
    Float Roughness1 = Roughness + 1;
    Float K     = (Roughness1 * Roughness1) / 8.0f;
    Float NDotV = Max(Dot(N, V), 0.0000001f);
    return NDotV / Max(NDotV * (1.0f - K) + K, 0.0000001f);
}

Float GeometrySmithGGX(Float3 N, Float3 L, Float3 V, Float3 H, float Roughness)
{
    return GeometrySmithGGX1(N, L, H, Roughness) * GeometrySmithGGX1(N, V, H, Roughness);
}

#endif