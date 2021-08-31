#include "PBRFunctions.hlsli"

struct InputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 world : WORLDPOS;
};

cbuffer CameraBuffer : register(b0)
{
    float4 cameraPosition;
}

Texture2D Diffuse : register(t0);
Texture2D Metal : register(t1);
Texture2D Rough : register(t2);

SamplerState SimpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{
    float3 albedo = float3(1, 0, 0);
    float metallic = 1.0;
    float roughness = 0.1;
    
    float3 N = normalize(v.normal);
    float3 V = normalize(cameraPosition.xyz - v.world);
    
    float3 F0 = lerp(0.04, albedo, metallic);
    
    float3 lightPositions[] =
    {
        float3(-3, 3, -1.75),
        float3(-2, -2, -1.75),
        float3(1.5, -1.5, -1.75),
        float3(2, 1, -1.75)
    };
    
    float3 Lo = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        float3 L = normalize(lightPositions[i] - v.world);
        float3 H = normalize(V + L);
        float atten = 1.0 / pow(length(lightPositions[i] - v.world), 2);
        float3 radiance = float3(23.47, 21.31, 20.79) * atten;
        
        //Cook-Torrence BRDF
        float NdotV = max(dot(N, V), 0.0000001);
        float NdotL = max(dot(N, L), 0.0000001);
        float HdotV = max(dot(H, V), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        
        float NDF = distributionGGX(NdotH, roughness);
        float G = geometrySmith(N, V, L, roughness);
        float3 F = fresnelSchlick(HdotV, F0);
        
        float3 specular = NDF * G * F;
        specular /= max(4.0 * NdotV * NdotL, 0.0000001);
        
        float3 kD = 1.0 - F;
        
        kD *= 1.0 - metallic;
        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = 0.03 * albedo;
    
    float3 color = ambient + Lo;
    
    //HDR tonemapping
    color = color / (color + 1.0);
    
    //Gamma Correction
    color = pow(color, 1.0 / GAMMA);
    
    
    return float4(color, 1);
}