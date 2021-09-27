struct InputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 world : WORLDPOS;
    float3 local : LOCALPOS;
};

Texture2D SimpleTexture : register(t0);
SamplerState SimpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{
    return SimpleTexture.Sample(SimpleSampler, v.uv);
}