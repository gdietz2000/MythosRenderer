struct InputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
};

Texture2D simpleTexture : register(t0);

SamplerState simpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{
    return simpleTexture.Sample(simpleSampler, v.uv);
}