#include "PBRFunctions.hlsli"

struct InputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 world : WORLDPOS;
    float3 local : LOCALPOS;
};

struct Light
{
    int lightType;
    float3 lightColor;
    float lightIntensity;
		
		//Directional Lights only
    float3 lightDirection;

		//Point Lights only
    float3 lightPosition;
    float lightRadius;
        
        //Spot Lights only
    float lightConeRatio;
    float lightInnerConeRatio;
    float lightOuterConeRatio;
    float buffer;
};

cbuffer CameraBuffer : register(b0)
{
    float4 cameraPosition;
}

cbuffer LightBuffer : register(b1)
{
    Light lightArray[5];
}

TextureCube convolutedMap : register(t0);
TextureCube environmentMap : register(t1);
Texture2D brdf : register(t2);

Texture2D diffuseTexture : register(t3);
Texture2D aoTexture : register(t4);
Texture2D normalTexture : register(t5);
Texture2D metalTexture : register(t6);
Texture2D roughTexture : register(t7);

SamplerState SimpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{    
    float3 albedo = pow(diffuseTexture.Sample(SimpleSampler, v.uv), GAMMA);
    float ao = aoTexture.Sample(SimpleSampler, v.uv).r;
    float metallic = metalTexture.Sample(SimpleSampler, v.uv).r;
    float roughness = roughTexture.Sample(SimpleSampler, v.uv).r;
    
    float3 N = getNormalFromTexture(normalTexture, SimpleSampler, v.uv, v.normal, v.world);
    float3 V = normalize(cameraPosition.xyz - v.world);    
    float3 R = reflect(-V, N);
    
    float3 F0 = lerp(0.04, albedo, metallic);
    
    float3 Lo = 0.0;
    for (int i = 0; i < 5; ++i)
    {
        float3 L = 0.0;
        float3 H = 0.0;
        float3 radiance = 0.0;
        
        //Directional Light
        if (lightArray[i].lightType == 1)
        {
            L = normalize(-lightArray[i].lightDirection);
            H = normalize(V + L);
            radiance = lightArray[i].lightColor * dot(L, N) * lightArray[i].lightIntensity;
        }
        //Point Light
        else if (lightArray[i].lightType == 2)
        {
            L = normalize(lightArray[i].lightPosition - v.world);
            H = normalize(V + L);
            //float atten = 1.0 / pow(length(l1.lightPosition - v.world), 2);
            float atten = 1.0 - length(lightArray[i].lightPosition - v.world) / lightArray[i].lightRadius;
            atten *= atten;
            radiance = lightArray[i].lightColor * atten * lightArray[i].lightIntensity;
        }
        //Spot Light
        else if (lightArray[i].lightType == 3)
        {
            L = normalize(lightArray[i].lightPosition - v.world);
            H = normalize(V + L);
            
            float theta = dot(L, -lightArray[i].lightDirection);
            float epsilon = lightArray[i].lightInnerConeRatio - lightArray[i].lightOuterConeRatio;
            float atten = clamp((theta - lightArray[i].lightOuterConeRatio) / epsilon, 0.0, 1.0);
            atten *= atten;
            radiance = lightArray[i].lightColor * dot(L, N) * atten * lightArray[i].lightIntensity;
        }
        else
        {
            continue;
        }
        
        
        
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
    
    float3 F = fresnelSchlickRoughness(dot(N, V), F0, roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    float3 irradiance = pow(convolutedMap.Sample(SimpleSampler, N).rgb, GAMMA);
    float3 diffuse = irradiance * albedo;
    diffuse = 0.0;
    
    //Indirect specular reflections
    const float MAX_REFLECTION_LOD = 4.0;
    
    float3 prefilteredColor = environmentMap.SampleLevel(SimpleSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    float2 envBRDF = brdf.Sample(SimpleSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    specular = 0.0;
    
    float3 ambient = (kD * diffuse + specular) * ao;
    
    float3 color = ambient + Lo;
    
    //HDR tonemapping
    color = color / (color + 1.0);
    
    //Gamma Correction
    color = pow(color, 1.0 / GAMMA);
    
    
    return float4(color, 1);
}