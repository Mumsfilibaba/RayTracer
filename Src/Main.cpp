#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "Core.h"
#include "Lighting.h"

#define NUM_SAMPLES 32

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

Float4 CastRay(World* World, Float3 RayOrigin, Float3 RayDirection)
{
    Float4 Result = Float4(0.0f, 1.0f, 1.0f, 1.0f);

    PayLoad InitalPayLoad;
    InitalPayLoad.t             = FLT_MAX;
    InitalPayLoad.MaterialIndex = UINT_MAX;
    TraceRay(World, &InitalPayLoad, RayOrigin, RayDirection);

    if (InitalPayLoad.MaterialIndex != UINT_MAX)
    {
        Material* Mat = &World->Materals[InitalPayLoad.MaterialIndex];

        Float4 Albedo        = Mat->Albedo;
        Float3 WorldPosition = RayOrigin + RayDirection * InitalPayLoad.t;
        Float3 N = Normalize(InitalPayLoad.Normal);
        Float3 V = Normalize(RayOrigin - WorldPosition);

        Float4 F0 = Float4(0.04f);
        F0 = Lerp(F0, Albedo, Mat->Metallic);

        Float4 L0 = Float4(0.0f);
        for (UInt32 i = 0; i < World->NumPointLights; i++)
        {
            PointLight* PointLight = &World->PointLights[i];

            PayLoad ShadowPayLoad;
            ShadowPayLoad.t             = FLT_MAX;
            ShadowPayLoad.MaterialIndex = UINT_MAX;

            Float3 L = PointLight->Position - WorldPosition;
            Float3 ShadowOrigin = WorldPosition + (N * 0.01f);
            Float3 ShadowDir    = Normalize(L);
            TraceRay(World, &ShadowPayLoad, ShadowOrigin, ShadowDir);

            if (ShadowPayLoad.t == FLT_MAX)
            {
                Float Distance = Length(L);
                L = Normalize(L);
                Float3 H = Normalize(V + L);

                Float  Attenuation = 1.0f / (Distance * Distance);
                Float4 Radiance    = PointLight->Color * Attenuation;

                // Lambertian BRDF
                Float4 Diffuse = Albedo / PI;

                // Cook-Torrance Specular BRDF
                Float4 F = FresnelSchlick(F0, Max(Dot(V, H), 0.0));
                Float4 Numerator   = DistributionGGX(N, H, Mat->Roughness) * F * GeometrySmithGGX(N, L, V, Mat->Roughness);
                Float  Denominator = 4.0f * Max(Dot(N, L), 0.0f) * Max(Dot(N, V), 0.0f);
                Float4 Specular    = Numerator / Max(Denominator, Epsilon);

                Float4 Ks = F;
                Float4 Kd = (Float4(1.0f) - Ks) * (1.0f - Mat->Metallic);

                Float NDotL = Max(Dot(L, N), 0.0f);
                L0 = L0 + ((Kd * Diffuse) + (Ks * Specular)) * Radiance * NDotL;
            }
        }

        Float4 Ambient = (Albedo * 0.03f);
        Result = L0 + Ambient;
    }

    return Result;
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
                Color = Color + CastRay(World, RayOrigin, RayDirection);
            }

            Color = Color / Float(SamplesPerPixel);
            Color = Color / (Color + Float4(1.0f));
            Color = Saturate(Pow(Color, Float4(1.0f / 2.0f)));
            Output->Pixels[y * Output->Width + x] = ConvertFloat4ToUInt(Color);
        }

        printf("\rRayTracing Row %d", y);
    }

    printf("\n");
}

int main(int Argc, const char* Args[])
{
    UInt32 Width  = 1920;
    UInt32 Height = 1080;

    Image* Output = CreateImage(Width, Height);
    
    //Sphere Spheres[] = 
    //{
    //    Sphere(Float3( 0.0f, 0.6f, 0.0f), 0.5f, 0),
    //    Sphere(Float3( 1.25f, 0.6f, 1.25f), 0.5f, 1),
    //    Sphere(Float3(-1.25f, 0.6f, 1.25f), 0.5f, 2),
    //    Sphere(Float3(-2.0f, 1.25f,-2.0f), 0.75f, 0)
    //};

    Sphere Spheres[] = 
    {
        Sphere(Float3(-3.0f, 0.5f, 0.0f), 0.25f, 1),
        Sphere(Float3(-2.4f, 0.5f, 0.0f), 0.25f, 2),
        Sphere(Float3(-1.8f, 0.5f, 0.0f), 0.25f, 3),
        Sphere(Float3(-1.2f, 0.5f, 0.0f), 0.25f, 4),
        Sphere(Float3(-0.6f, 0.5f, 0.0f), 0.25f, 5),
        Sphere(Float3( 0.0f, 0.5f, 0.0f), 0.25f, 6),
        Sphere(Float3( 0.6f, 0.5f, 0.0f), 0.25f, 7),
        Sphere(Float3( 1.2f, 0.5f, 0.0f), 0.25f, 8),
        Sphere(Float3( 1.8f, 0.5f, 0.0f), 0.25f, 9),
        Sphere(Float3( 2.4f, 0.5f, 0.0f), 0.25f, 10),

        Sphere(Float3(-3.0f, 1.0f, 1.0f), 0.25f, 11),
        Sphere(Float3(-2.4f, 1.0f, 1.0f), 0.25f, 12),
        Sphere(Float3(-1.8f, 1.0f, 1.0f), 0.25f, 13),
        Sphere(Float3(-1.2f, 1.0f, 1.0f), 0.25f, 14),
        Sphere(Float3(-0.6f, 1.0f, 1.0f), 0.25f, 15),
        Sphere(Float3( 0.0f, 1.0f, 1.0f), 0.25f, 16),
        Sphere(Float3( 0.6f, 1.0f, 1.0f), 0.25f, 17),
        Sphere(Float3( 1.2f, 1.0f, 1.0f), 0.25f, 18),
        Sphere(Float3( 1.8f, 1.0f, 1.0f), 0.25f, 19),
        Sphere(Float3( 2.4f, 1.0f, 1.0f), 0.25f, 20),
    };

    Plane Planes[] =
    {
        Plane(Float3(0.0f, 1.0f, 0.0f), -0.25f, 0),
    };

    //Material Materials[] =
    //{
    //    Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.0f),
    //    Material(Float4(0.0f, 1.0f, 0.0f, 1.0f), 0.5f, 0.0f),
    //    Material(Float4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f),
    //    Material(Float4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0.0f)
    //};

    Material Materials[] =
    {
        Material(Float4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.0f),
        
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.1f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.2f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.3f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.4f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.6f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.7f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.8f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.9f, 0.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f),

        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.1f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.2f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.3f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.4f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.6f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.7f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.8f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 0.9f, 1.0f),
        Material(Float4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, 1.0f),
    };
    
    PointLight PointLights[] =
    {
        PointLight(Float4(100.0f), Float3( 3.5f, 5.0f, 2.0f)),
        PointLight(Float4(100.0f), Float3(-3.5f, 5.0f, 2.0f)),
        PointLight(Float4(100.0f), Float3( 3.5f, 5.0f,-2.0f)),
        PointLight(Float4(100.0f), Float3(-3.5f, 5.0f,-2.0f)),
    };
    
    World World;
    World.Spheres        = Spheres;
    World.NumSpheres     = ArrayCount(Spheres);
    World.Planes         = Planes;
    World.NumPlanes      = ArrayCount(Planes);
    World.Materals       = Materials;
    World.NumMaterials   = ArrayCount(Materials);
    World.PointLights    = PointLights;
    World.NumPointLights = ArrayCount(PointLights);
    World.CameraPosition = Float3(-0.25f, 1.5f, -3.5f);
    
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