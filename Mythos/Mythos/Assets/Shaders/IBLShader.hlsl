#include "PBRFunctions.hlsli"

struct InputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 world : WORLDPOS;
    float3 local : LOCALPOS;
};

cbuffer CameraBuffer : register(b0)
{
    float4 cameraPosition;
}

TextureCube cubeMap : register(t0);

SamplerState SimpleSampler : register(s0);

static const float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    
    uv.y = abs(1.0 - uv.y);
    
    return uv;
}

float4 main(InputVertex v) : SV_TARGET
{
    float3 color = pow(cubeMap.Sample(SimpleSampler, v.local), GAMMA);
    
    color = color / (color + 1.0);
    
    color = pow(color, 1.0 / GAMMA);
    
    return float4(color, 1);
}