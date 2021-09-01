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

Texture2D equirectangularMap : register(t0);
TextureCube cubeMap : register(t1);

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
    //float2 uv = SampleSphericalMap(normalize(v.local));
    return cubeMap.Sample(SimpleSampler, v.local);
}