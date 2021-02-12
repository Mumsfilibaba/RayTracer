#ifndef MATH_H
#define MATH_H

#include "Types.h"

#include <Float.h>

struct Float3
{
    Float3() = default;

    Float3(Float InX, Float InY, Float InZ)
        : x(InX)
        , y(InY)
        , z(InZ)
    {
    }

    Float x = 0.0f;
    Float y = 0.0f;
    Float z = 0.0f;
};

Float Dot(Float3 LHS, Float3 RHS)
{
    return LHS.x * RHS.x + LHS.y * RHS.y + LHS.z * RHS.z;
}

Float3 operator+(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x + RHS.x, LHS.y + RHS.y, LHS.z + RHS.z);
}

Float3 operator-(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x - RHS.x, LHS.y - RHS.y, LHS.z - RHS.z);
}

Float3 operator*(Float3 LHS, Float3 RHS)
{
    return Float3(LHS.x * RHS.x, LHS.y * RHS.y, LHS.z * RHS.z);
}

struct Float4
{
    Float4() = default;

    Float4(Float InX, Float InY, Float InZ, Float InW)
        : x(InX)
        , y(InY)
        , z(InZ)
        , w(InW)
    {
    }

    Float x = 0.0f;
    Float y = 0.0f;
    Float z = 0.0f;
    Float w = 0.0f;
};

Float4 operator+(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x + RHS.x, LHS.y + RHS.y, LHS.z + RHS.z, LHS.w + RHS.w);
}

Float4 operator-(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x - RHS.x, LHS.y - RHS.y, LHS.z - RHS.z, LHS.w - RHS.w);
}

Float4 operator*(Float4 LHS, Float4 RHS)
{
    return Float4(LHS.x * RHS.x, LHS.y * RHS.y, LHS.z * RHS.z, LHS.w * RHS.w);
}

Float Dot(Float4 LHS, Float4 RHS)
{
    return LHS.x * RHS.x + LHS.y * RHS.y + LHS.z * RHS.z + LHS.w * RHS.w;
}

#endif