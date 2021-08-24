struct InputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
    float3 normal : NORMAL;
};

Texture2D Diffuse : register(t0);

SamplerState SimpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{
    return Diffuse.Sample(SimpleSampler, v.uv);
}