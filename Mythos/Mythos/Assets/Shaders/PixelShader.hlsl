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
Texture2D Ambient : register(t1);

Texture2D Metal : register(t2);
Texture2D Roughness : register(t3);

SamplerState SimpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{
    float3 albedo = pow(Diffuse.Sample(SimpleSampler, v.uv).rgb, GAMMA);
    float ao = Ambient.Sample(SimpleSampler, v.uv);
    float3 normalMap;
    float metalness = Metal.Sample(SimpleSampler, v.uv);
    float roughness = Roughness.Sample(SimpleSampler, v.uv);
    
    //BRDF
    
    float3 lightDirection = normalize(-float3(0, -1, 1));
    float3 lightColor = float3(2.0, 2.0, 2.0);
    float3 lightIntensity = 3.0;
    float3 radiance = lightColor * lightIntensity;
    
    float3 N = normalize(v.normal);
    float3 V = normalize(cameraPosition.xyz - v.world);
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metalness);
    
    float3 L = normalize(lightDirection);
    float3 H = normalize(V + L);
        
    //Cook-Torrence BRDF
    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    float3 kS = F;
    float3 kD = float3(1, 1, 1) - kS;
    kD *= 1.0 - metalness;
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);
    float NdotL = max(dot(N, L), 0.0);
    
    float LO = (kD * albedo / PI + specular) * radiance * NdotL;
    
    float3 ambient = albedo * ao;
    float3 color = ambient * LO;
    
    color = color / (color + float3(1.0, 1.0, 1.0));
    color = pow(color, 1.0 / GAMMA);
    
    return float4(color , 1);
}