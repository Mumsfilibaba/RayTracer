#ifndef LIGHTING_H
#define LIGHTING_H

#include "Math.h"

#define Epsilon (0.000001f)

Float DistributionGGX(Float3 N, Float3 H, Float Roughness)
{
    Float Alpha  = Roughness * Roughness;
    Float Alpha2 = Alpha * Alpha;
    Float NDotH  = Max(Dot(N, H), 0.0);
    Float Denom  = NDotH * NDotH * (Alpha2 - 1.0f) + 1.0f;
    return Alpha2 / Max(PI * Denom * Denom, Epsilon);
}

Float4 FresnelSchlick(Float4 F0, Float CosTheta)
{
    return F0 + ((Float4(1.0f) - F0) * pow(Max(1.0f - CosTheta, 0.0f), 5.0f));
}

Float GeometrySmithGGX1(Float3 N, Float3 V, Float Roughness)
{
    Float Roughness1 = Roughness + 1;
    Float K     = (Roughness1 * Roughness1) / 8.0f;
    Float NDotV = Max(Dot(N, V), 0.0f);
    return NDotV / Max(NDotV * (1.0f - K) + K, Epsilon);
}

Float GeometrySmithGGX(Float3 N, Float3 L, Float3 V, Float Roughness)
{
    return GeometrySmithGGX1(N, L, Roughness) * GeometrySmithGGX1(N, V, Roughness);
}

#endif