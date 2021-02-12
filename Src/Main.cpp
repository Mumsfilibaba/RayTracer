#include <stdlib.h>
#include <stdio.h>

#include "Core.h"

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

Float4 CastRay(World* World, Float3 RayOrigin, Float3 RayDirection)
{
    Float4 Result;
    Float SmallestT = FLT_MAX;

    for (UInt32 i = 0; i < World->NumSpheres; i++)
    {
        Sphere* Sp = &World->Spheres[i];

        Float3 Distance = RayOrigin - Sp->Position;
        Float a = Dot(RayDirection, RayDirection);
        Float b = 2.0f * Dot(RayDirection, Distance);
        Float c = Dot(Distance, Distance) - pow(Sp->Radius, 2.0f);
    }

    return Float4(1.0f, 0.0f, 0.0f, 1.0f);
}

void RenderImage(Image* Output, World* World)
{
    Float PixelWidth  = 2.0f / Output->Width;
    Float PixelHeight = 2.0f / Output->Height;

    Float3 RayOrigin = World->CameraPosition;
    Float3 Target    = Float3(0.0f, 0.0f, 0.0f);

    for (UInt32 y = 0; y < Output->Height; y++)
    {
        for (UInt32 x = 0; x < Output->Width; x++)
        {
            Float3 To        = Target + Float3(-1.0f + Float(x) * PixelWidth, -1.0f + Float(y) * PixelWidth, 0.0f);
            Float3 Direction = To - RayOrigin;

            Float4 Color = CastRay(World, RayOrigin, Direction);
            Output->Pixels[y * Output->Width + x] = ConvertFloat4ToUInt(Color);
        }
    }
}

int main(int Argc, const char* Args[])
{
    UInt32 Width  = 1920;
    UInt32 Height = 1080;

    Image* Output = CreateImage(Width, Height);
    
    Sphere Spheres[] = 
    {
        Sphere(Float3(0.0f, 0.0f, 0.0f), 0.5f)
    };
    
    World World;
    World.Spheres        = Spheres;
    World.NumSpheres     = ArrayCount(Spheres);
    World.CameraPosition = Float3(0.0f, 0.0f, -2.0f);
    
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