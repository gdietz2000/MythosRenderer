struct InputVertex
{
    float4 position : SV_Position;
    float4 local : LOCALPOS;
};

Texture2D equirectangularMap : register(t0);

SamplerState SimpleSampler : register(s0);

static const float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    
    //DirectX has the uv origin set to bottom left, whereas other Renderers set the uv origin to top left, this calculation is needed for proper uvs
    uv.y = abs(1.0 - uv.y);
    
    return uv;
}

float4 main(InputVertex v) : SV_TARGET
{
    float2 uv = SampleSphericalMap(normalize(v.local.xyz));
    return equirectangularMap.Sample(SimpleSampler, uv);
}