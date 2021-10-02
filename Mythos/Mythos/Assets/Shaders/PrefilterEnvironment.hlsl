#include "PBRFunctions.hlsli"

struct PixelVertex
{
    float4 pos : SV_Position;
    float4 local : LOCALPOS;
};

cbuffer Roughness : register(b0)
{
    float4 roughness;
};

TextureCube environmentMap : register(t0);

SamplerState SimpleSampler : register(s0);

float4 main(PixelVertex v) : SV_TARGET
{
    float3 N = normalize(v.local);
    float3 R = N;
    float3 V = R;
    
    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    float3 prefilteredColor = 0.0;
    
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = hammersley(i, SAMPLE_COUNT);
        float3 H = importanceGGX(Xi, N, roughness.x);
        float3 L = normalize(2.0 * dot(V, H) * H - V);
        
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            prefilteredColor += environmentMap.Sample(SimpleSampler, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    
    return float4(prefilteredColor, 1.0f);
}