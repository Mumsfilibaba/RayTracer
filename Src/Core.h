#ifndef CORE_H
#define CORE_H

#include "Types.h"
#include "Math.h"

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

struct Image
{
    ~Image()
    {
        free(Pixels);
    }

    UInt32* Pixels;
    UInt32  Width;
    UInt32  Height;
};

#pragma pack(push, 1)
struct BitmapHeader
{
    // File Header
    Char   Header[2];       // 2
    UInt32 FileSize;        // 6
    UInt32 Reserved;        // 10
    UInt32 ImageOffset;     // 14
    // Image Information
    UInt32 HeaderSize;      // 4
    UInt32 Width;           // 8
    UInt32 Height;          // 12
    UInt16 Planes;          // 14
    UInt16 BPP;             // 16
    UInt32 Compression;     // 20
    UInt32 ImageSize;       // 24
    UInt32 XpixelsPerM;     // 28
    UInt32 YpixelsPerM;     // 32
    UInt32 ColorsUsed;      // 36
    UInt32 ImportantColors; // 40
};
#pragma pack(pop)

struct Sphere
{
    Sphere(Float3 InPosition, Float InRadius, UInt32 InMaterialIndex)
        : Position(InPosition)
        , Radius(InRadius)
        , MaterialIndex(InMaterialIndex)
    {
    }

    Float3 Position;
    Float  Radius;
    UInt32 MaterialIndex;
};

struct Plane
{
    Plane(Float3 InNormal, Float InOffset, UInt32 InMaterialIndex)
        : Normal(Normalize(InNormal))
        , Offset(InOffset)
        , MaterialIndex(InMaterialIndex)
    {
    }

    Float3 Normal;
    Float  Offset;
    UInt32 MaterialIndex;
};

struct Material
{
    Float4 Albedo    = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    Float  Roughness = 1.0f;
    Float  Metallic  = 1.0f;
};

struct PointLight
{
    PointLight(Float4 InColor, Float3 InPosition)
        : Color(InColor)
        , Position(InPosition)
    {
    }

    Float4 Color = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    Float3 Position;
};

struct World
{
    Float3 CameraPosition;

    Sphere* Spheres    = nullptr;
    UInt32  NumSpheres = 0;

    Plane* Planes    = nullptr;
    UInt32 NumPlanes = 0;

    Material* Materals     = nullptr;
    UInt32    NumMaterials = 0;

    PointLight* PointLights    = nullptr;
    UInt32      NumPointLights = 0;
};

struct PayLoad
{
    Float  t;
    UInt32 MaterialIndex;
    Float3 Normal;
};

#endif