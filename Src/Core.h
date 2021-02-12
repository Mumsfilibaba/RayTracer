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
    Sphere(Float3 InPosition, Float InRadius)
        : Position(InPosition)
        , Radius(InRadius)
    {
    }

    Float3 Position;
    Float  Radius;
};

struct World
{
    Float3 CameraPosition;

    Sphere* Spheres    = nullptr;
    UInt32  NumSpheres = 0;
};

#endif