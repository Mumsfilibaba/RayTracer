#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "Core.h"
#include "Lighting.h"

#define NUM_SAMPLES 64

Image* CreateImage(UInt32 Width, UInt32 Height)
{
    Image* NewImage = new Image();
    NewImage->Pixels = (UInt32*)malloc(Width * Height * sizeof(UInt32));
    NewImage->Width  = Width;
    NewImage->Height = Height;
    return NewImage;
};

Bool WriteImage(Image* Output)
{
    FILE* OutputFile = fopen("Image.bmp", "wb");
    if (!OutputFile)
    {
        return false;
    }

    UInt32 ImageSize = sizeof(UInt32) * Output->Width * Output->Height;
    UInt32 FileSize  = ImageSize + sizeof(BitmapHeader);

    BitmapHeader Header;
    Header.Header[0]       = 'B';
    Header.Header[1]       = 'M';
    Header.FileSize        = FileSize;
    Header.Reserved        = 0;
    Header.ImageOffset     = sizeof(BitmapHeader);
    Header.HeaderSize      = sizeof(BitmapHeader) - 14;
    Header.Width           = Output->Width;
    Header.Height          = Output->Height;
    Header.Planes          = 1;
    Header.BPP             = 32;
    Header.Compression     = 0;
    Header.ImageSize       = 0;
    Header.XpixelsPerM     = 1024;
    Header.YpixelsPerM     = 1024;
    Header.ColorsUsed      = 0;
    Header.ImportantColors = 0;

    fwrite(&Header, sizeof(BitmapHeader), 1, OutputFile);
    fwrite(Output->Pixels, ImageSize, 1, OutputFile);

    fclose(OutputFile);

    return true;
}

UInt32 ConvertFloat4ToUInt(Float4 Color)
{
    UInt32 Result = 0;
    Result |= UInt8(Color.x * 255.0f) << 16;
    Result |= UInt8(Color.y * 255.0f) << 8;
    Result |= UInt8(Color.z * 255.0f) << 0;
    Result |= UInt8(Color.w * 255.0f) << 24;
    return Result;
}

void TraceRay(World* World, PayLoad* PayLoad, Float3 RayOrigin, Float3 RayDirection)
{
    for (UInt32 i = 0; i < World->NumSpheres; i++)
    {
        Sphere* Sp = &World->Spheres[i];

        Float3 Distance = RayOrigin - Sp->Position;
        Float a = Dot(RayDirection, RayDirection);
        Float b = 2.0f * Dot(RayDirection, Distance);
        Float c = Dot(Distance, Distance) - pow(Sp->Radius, 2.0f);

        Float Disc = (b * b) - (4.0f * a * c);
        if (Disc >= 0)
        {
            Float Root = sqrt(Disc);
            Float t0 = (-b + Root) / (2.0f * a);
            Float t1 = (-b - Root) / (2.0f * a);
            
            if (t0 > t1)
            {
                Float temp = t0;
                t0 = t1;
                t1 = t0;
            }

            if (t0 < 0)
            {
                t0 = t1;
            }

            if (t0 >= 0 && t0 < PayLoad->t)
            {
                PayLoad->MaterialIndex = Sp->MaterialIndex;
                PayLoad->t = t0;

                Float3 Position = RayOrigin + RayDirection * t0;
                PayLoad->Normal = Normalize(Position - Sp->Position);
            }
        }
    }

    for (UInt32 i = 0; i < World->NumPlanes; i++)
    {
        Plane* Plane = &World->Planes[i];

        Float DDotN = Dot(RayDirection, Plane->Normal);
        if (fabs(DDotN) > 0.0001f)
        {
            Float3 Center = Plane->Normal * Plane->Offset;
            Float3 Diff = Center - RayOrigin;

            Float t = Dot(Diff, Plane->Normal) / DDotN;
            if (t >= 0)
            {
                if (t < PayLoad->t)
                {
                    PayLoad->MaterialIndex = Plane->MaterialIndex;
                    PayLoad->t = t;
                    PayLoad->Normal = Normalize(Plane->Normal);
                }
            }
        }
    }
}

Float4 CastRay(World* World, Float3 RayOrigin, Float3 RayDirection, UInt32 Depth)
{
    if (Depth > 32)
    {
        return Float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    PayLoad PayLoad;
    PayLoad.t             = FLT_MAX;
    PayLoad.MaterialIndex = UINT_MAX;
    TraceRay(World, &PayLoad, RayOrigin, RayDirection);

    if (PayLoad.MaterialIndex != UINT_MAX)
    {
        Material* Mat = &World->Materals[PayLoad.MaterialIndex];
        Float4 Albedo = Mat->Albedo;

        Float3 N = Normalize(PayLoad.Normal);
        Float3 Position  = RayOrigin + RayDirection * PayLoad.t;
        Float3 NewOrigin = Position + (N * 0.00001f);
        
        if (!Mat->Metal)
        {
            Float3 Target = Position + RandomHemisphereFloat3(N);
            Float3 NewDir = Normalize(Target - Position);
            return Albedo * CastRay(World, NewOrigin, NewDir, Depth + 1);
        }
        else
        {
            Float3 Reflection = Reflect(Normalize(RayDirection), N);
            Reflection = Reflection + (Mat->Roughness * RandomUnitFloat3());
            Reflection = Normalize(Reflection);

            if (Dot(Reflection, N) > 0)
            {
                return Albedo * CastRay(World, NewOrigin, Reflection, Depth + 1);
            }
            else
            {
                return Float4(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
    }
    
    return Float4(0.39f, 0.58f, 0.92f, 1.0f);
}

void RenderImage(Image* Output, World* World)
{
    Float3 CameraZ = Normalize(Float3(0.0f, 0.0f, 1.0f));
    Float3 CameraX = Normalize(Cross(Float3(0.0f, 1.0f, 0.0f), CameraZ));
    Float3 CameraY = Normalize(Cross(CameraZ, CameraX));

    Float FilmDistance = 1.0f;
    Float FilmWidth  = 2.0f;
    Float FilmHeight = 2.0f;
    if (Output->Width > Output->Height)
    {
        FilmHeight = FilmWidth * (Float(Output->Height) / Float(Output->Width));
    }
    else if (Output->Height > Output->Width)
    {
        FilmWidth = FilmHeight * (Float(Output->Width) / Float(Output->Height));
    }

    Float HalfFilmWidth  = 0.5f * FilmWidth;
    Float HalfFilmHeight = 0.5f * FilmHeight;

    Float3 RayOrigin  = World->CameraPosition;
    Float3 FilmCenter = World->CameraPosition + (FilmDistance * CameraZ);

    const UInt32 SamplesPerPixel = NUM_SAMPLES;
    for (UInt32 y = 0; y < Output->Height; y++)
    {
        for (UInt32 x = 0; x < Output->Width; x++)
        {
            Float4 Color;
            for (UInt32 i = 0; i < SamplesPerPixel; i++)
            {
                // Jitter pixels for AA
                Float OffsetX = RandomFloat() / 2.0f;
                Float OffsetY = RandomFloat() / 2.0f;
                Float PixelY  = Float(y) + 0.5f + OffsetX;
                Float PixelX  = Float(x) + 0.5f + OffsetY;
                Float FilmX   = -1.0f + (2.0f * (PixelX / Output->Width));
                Float FilmY   = -1.0f + (2.0f * (PixelY / Output->Height));

                Float3 PixelLocation = FilmCenter + (FilmX * HalfFilmWidth) * CameraX + (FilmY * HalfFilmHeight) * CameraY;
                Float3 RayDirection  = Normalize(PixelLocation - RayOrigin);
                Color = Color + CastRay(World, RayOrigin, RayDirection, 0);
            }

            Color = Color / Float(SamplesPerPixel);
            //Color = Color / (Color + Float4(1.0f));
            Color = Saturate(Pow(Color, Float4(1.0f / 2.2f)));
            Output->Pixels[y * Output->Width + x] = ConvertFloat4ToUInt(Color);
        }

        printf("\rRayTracing Row %d", y);
    }

    printf("\n");
}

int main(int Argc, const char* Args[])
{
    UInt32 Width  = 3840;
    UInt32 Height = 2160;

    Image* Output = CreateImage(Width, Height);
    
    Sphere Spheres[] = 
    {
        Sphere(Float3( 0.0f,  0.2f,  0.0f),  0.5f,  0),
        Sphere(Float3( 1.25f, 0.2f,  1.25f), 0.5f,  1),
        Sphere(Float3(-1.25f, 0.2f,  1.25f), 0.5f,  2),
        Sphere(Float3(-2.0f,  1.25f,-2.0f),  0.75f, 0),
        Sphere(Float3( 2.0f,  1.25f,-2.0f),  0.75f, 3),
        Sphere(Float3( 0.0f,  3.0f,  2.0f),  2.0f,  1),
    };

    Plane Planes[] =
    {
        Plane(Float3(0.0f, 1.0f, 0.0f), -0.25f, 4),
    };

    Material Materials[] =
    {
        Material(Float4(1.0f,  0.05f, 0.05f, 1.0f), false, 0.0f),
        Material(Float4(1.0f,  1.0f,  1.0f,  1.0f), true,  0.0f),
        Material(Float4(1.0f,  1.0f,  1.0f,  1.0f), true,  0.1f),
        Material(Float4(1.0f,  1.0f,  1.0f,  1.0f), true,  0.2f),
        Material(Float4(0.56f, 0.93f, 0.56f, 1.0f), true,  0.1f),
    };
    
    World World;
    World.Spheres        = Spheres;
    World.NumSpheres     = ArrayCount(Spheres);
    World.Planes         = Planes;
    World.NumPlanes      = ArrayCount(Planes);
    World.Materals       = Materials;
    World.NumMaterials   = ArrayCount(Materials);
    World.CameraPosition = Float3(0.0f, 2.0f, -7.0f);
    
    RenderImage(Output, &World);

    if (WriteImage(Output))
    {
        printf("Wrote image to file\n");
    }
    else
    {
        printf("Failed to write image to file\n");
    }
 
    delete Output;
    return 0;
}