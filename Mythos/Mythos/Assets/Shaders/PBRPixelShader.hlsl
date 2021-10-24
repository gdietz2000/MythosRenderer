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
Texture2D emissiveTexture : register(t8);

SamplerState SimpleSampler : register(s0);

uint queryEnvironmentMap()
{
    uint width, height, levels;
    environmentMap.GetDimensions(0, width, height, levels);
    return levels;
}

float4 main(InputVertex v) : SV_TARGET
{    
    float3 albedo = pow(diffuseTexture.Sample(SimpleSampler, v.uv), GAMMA);
    float ao = aoTexture.Sample(SimpleSampler, v.uv).r;
    float metallic = metalTexture.Sample(SimpleSampler, v.uv).r;
    float roughness = roughTexture.Sample(SimpleSampler, v.uv).r;
    float3 emissive = emissiveTexture.Sample(SimpleSampler, v.uv);
        
    float3 N = getNormalFromTexture(normalTexture, SimpleSampler, v.uv, v.normal, v.world);
    float3 V = normalize(cameraPosition.xyz - v.world);    
    float3 R = reflect(-V, N);
    
    float3 F0 = lerp(0.04, albedo, metallic);
    
    float3 directLighting = 0.0;
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
            float atten = 1.0 - clamp(length(lightArray[i].lightPosition - v.world) / lightArray[i].lightRadius, 0.0, 1.0);
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
        float NdotV = max(dot(N, V), EPSILON);
        float NdotL = max(dot(N, L), EPSILON);
        float HdotV = max(dot(H, V), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        
        float NDF = distributionGGX(NdotH, roughness);
        float G = geometrySmith(N, V, L, roughness);
        float3 F = fresnelSchlick(HdotV, F0);
        
        
        float3 kD = lerp(1.0 - F, 0.0, metallic);
        
        float3 diffuseBRDF = kD * albedo;
        float3 specularBRDF = NDF * G * F / max(4.0 * NdotV * NdotL, EPSILON);
                
        directLighting += (diffuseBRDF / PI + specularBRDF) * radiance * NdotL;
    }
    
    float3 ambientLighting = 0.0;
    {
        //Diffuse Ambient Lighting
        float3 F = fresnelSchlickRoughness(dot(N, V), F0, roughness);
        float3 kD = lerp(1.0 - F, 0.0, metallic);

        float3 irradiance = pow(convolutedMap.Sample(SimpleSampler, N).rgb, GAMMA);
        float3 diffuseIBL = kD * irradiance * albedo * ao;
        
        //Specular Ambient Lighting
        uint reflectionLOD = queryEnvironmentMap();
        
        float3 specularIrradiance = environmentMap.SampleLevel(SimpleSampler, R, roughness * reflectionLOD).rgb;
        float2 specularBRDF = brdf.Sample(SimpleSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
        float3 specularIBL = (F * specularBRDF.x + specularBRDF.y) * specularIrradiance;
        
        ambientLighting = diffuseIBL + specularIBL;
    }
    
    //Indirect specular reflections
    
    //float3 prefilteredColor = environmentMap.SampleLevel(SimpleSampler, R, roughness * reflectionLOD).rgb;
    //float2 envBRDF = brdf.Sample(SimpleSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    //float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    ////specular = 0.0;
    
    //float3 ambient = (kD * diffuse + specular) * ao;
    
    float3 color = directLighting + ambientLighting;
    
    //HDR tonemapping
    color = color / (color + 1.0);
    
    //Gamma Correction
    color = pow(color, 1.0 / GAMMA);
    
    //Add Emissive
    color = color + emissive;
    
    return float4(color, 1);
}