#include "PBRFunctions.hlsli"

struct PixelVertex
{
    float4 pos : SV_Position;
    float2 tex : UV;
};

float2 main(PixelVertex v) : SV_TARGET
{
    float2 integrated = integrateBRDF(v.tex.x, abs(1.0 - v.tex.y));
    return integrated;
}