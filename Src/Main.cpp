#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <thread>
#include <atomic>
#include <vector>

#include "Core.h"
#include "Lighting.h"

#define WIDTH       (2560)
#define HEIGHT      (1440)
#define NUM_SAMPLES (128)

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
        Float b = Dot(RayDirection, Distance);
        Float c = Dot(Distance, Distance) - (Sp->Radius * Sp->Radius);

        Float Disc = (b * b) - (a * c);
        if (Disc >= 0)
        {
            Float Root = sqrt(Disc);
            Float t0 = (-b + Root) / a;
            Float t1 = (-b - Root) / a;
            
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

                Float3 Position      = RayOrigin + RayDirection * t0;
                Float3 OutsideNormal = Normalize((Position - Sp->Position) / Sp->Radius);
                if (Dot(RayDirection, OutsideNormal) > 0.0f)
                {
                    PayLoad->Normal    = -OutsideNormal;
                    PayLoad->FrontFace = false;
                }
                else
                {
                    PayLoad->Normal    = OutsideNormal;
                    PayLoad->FrontFace = true;
                }
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
                    PayLoad->t             = t;
                    PayLoad->Normal        = Normalize(Plane->Normal);
                    PayLoad->FrontFace     = true;
                }
            }
        }
    }
}

Float4 CastRay(World* World, Float3 RayOrigin, Float3 RayDirection, UInt32 Depth)
{
    if (Depth > 32)
    {
        return Float4(0.5f, 0.5f, 0.5f, 1.0f);
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
        Float3 Offset    = N * 0.00001f;
        Float3 NewOrigin = Position + Offset;
        
        if (Mat->Refracting)
        {
            const Float ir = 1.5f;
            if (PayLoad.FrontFace)
            {
                NewOrigin = Position - Offset;
            }

            Float  RefractionRatio = PayLoad.FrontFace ? (1.0 / ir) : ir;
            Float3 UnitDirection   = Normalize(RayDirection);
            Float3 Refracted       = Normalize(Refract(UnitDirection, N, RefractionRatio));
            return Float4(1.0f) * CastRay(World, NewOrigin, Refracted, Depth + 1);
        }
        else if (!Mat->Metal)
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

std::atomic_int32_t gCurrentY;

Image* gOutput = nullptr;
World* gWorld  = nullptr;

void RenderImage()
{
    printf("Test");

    Float3 CameraZ = Normalize(Float3(0.0f, 0.0f, 1.0f));
    Float3 CameraX = Normalize(Cross(Float3(0.0f, 1.0f, 0.0f), CameraZ));
    Float3 CameraY = Normalize(Cross(CameraZ, CameraX));

    Float FilmDistance = 1.0f;
    Float FilmWidth    = 2.0f;
    Float FilmHeight   = 2.0f;
    if (gOutput->Width > gOutput->Height)
    {
        FilmHeight = FilmWidth * (Float(gOutput->Height) / Float(gOutput->Width));
    }
    else if (gOutput->Height > gOutput->Width)
    {
        FilmWidth = FilmHeight * (Float(gOutput->Width) / Float(gOutput->Height));
    }

    Float HalfFilmWidth  = 0.5f * FilmWidth;
    Float HalfFilmHeight = 0.5f * FilmHeight;

    Float3 RayOrigin  = gWorld->CameraPosition;
    Float3 FilmCenter = gWorld->CameraPosition + (FilmDistance * CameraZ);

    const UInt32 SamplesPerPixel = NUM_SAMPLES;
    for (UInt32 y = gCurrentY++; y < gOutput->Height;)
    {
        printf("\rRayTracing Row %d", y);

        for (UInt32 x = 0; x < gOutput->Width; x++)
        {
            Float4 Color;
            for (UInt32 i = 0; i < SamplesPerPixel; i++)
            {
                // Jitter pixels for AA
                Float OffsetX = RandomFloat() / 2.0f;
                Float OffsetY = RandomFloat() / 2.0f;
                Float PixelY  = Float(y) + 0.5f + OffsetX;
                Float PixelX  = Float(x) + 0.5f + OffsetY;
                Float FilmX   = -1.0f + (2.0f * (PixelX / gOutput->Width));
                Float FilmY   = -1.0f + (2.0f * (PixelY / gOutput->Height));

                Float3 PixelLocation = FilmCenter + (FilmX * HalfFilmWidth) * CameraX + (FilmY * HalfFilmHeight) * CameraY;
                Float3 RayDirection  = Normalize(PixelLocation - RayOrigin);
                Color = Color + CastRay(gWorld, RayOrigin, RayDirection, 0);
            }

            Color = Color / Float(SamplesPerPixel);
            //Color = Color / (Color + Float4(1.0f));
            Color = Saturate(Pow(Color, Float4(1.0f / 2.2f)));
            gOutput->Pixels[y * gOutput->Width + x] = ConvertFloat4ToUInt(Color);
        }

        y = gCurrentY++;
    }

    printf("\n");
}

int main(int Argc, const char* Args[])
{
    UInt32 Width  = WIDTH;
    UInt32 Height = HEIGHT;

    Image* Output = CreateImage(Width, Height);
    
    Sphere Spheres[] = 
    {
        Sphere(Float3( 0.0f,  0.2f,  0.0f),  0.5f,  0),
        Sphere(Float3( 1.25f, 0.2f,  1.25f), 0.5f,  1),
        Sphere(Float3(-1.25f, 0.2f,  1.25f), 0.5f,  2),
        Sphere(Float3(-2.0f,  1.25f,-2.0f),  0.75f, 5),
        Sphere(Float3( 2.0f,  1.25f,-2.0f),  0.75f, 3),
        Sphere(Float3( 0.0f,  3.0f,  2.0f),  2.0f,  1),
    };

    Plane Planes[] =
    {
        Plane(Float3(0.0f, 1.0f, 0.0f), -0.25f, 4),
    };

    Material Materials[] =
    {
        Material(Float4(1.0f,  0.05f, 0.05f, 1.0f), false, 0.0f, false),
        Material(Float4(1.0f,  1.0f,  1.0f,  1.0f), true,  0.0f, false),
        Material(Float4(1.0f,  1.0f,  1.0f,  1.0f), true,  0.1f, false),
        Material(Float4(1.0f,  1.0f,  1.0f,  1.0f), true,  0.2f, false),
        Material(Float4(0.56f, 0.93f, 0.56f, 1.0f), true,  0.1f, false),
        Material(Float4(0.56f, 0.93f, 0.56f, 1.0f), false,  0.1f, true),
    };
    
    World World;
    World.Spheres        = Spheres;
    World.NumSpheres     = ArrayCount(Spheres);
    World.Planes         = Planes;
    World.NumPlanes      = ArrayCount(Planes);
    World.Materals       = Materials;
    World.NumMaterials   = ArrayCount(Materials);
    World.CameraPosition = Float3(0.0f, 2.0f, -7.0f);
    
    gCurrentY = -1;

    UInt32 ThreadCount = std::thread::hardware_concurrency() - 1;

    printf("Starting Threads\n");

    gOutput = Output;
    gWorld  = &World;

    // Start workers
    std::vector<std::thread> Threads;
    for (UInt32 i = 0; i < ThreadCount; i++)
    {
        printf("Starting Thread %d\n", i);
        Threads.push_back(std::thread(RenderImage));
    }
    
    // Let the main thread also render
    RenderImage();

    // Stop workers
    for (UInt32 i = 0; i < ThreadCount; i++)
    {
        printf("Joining Thread %d\n", i);

        if (Threads[i].joinable())
        {
            Threads[i].join();
        }
    }

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