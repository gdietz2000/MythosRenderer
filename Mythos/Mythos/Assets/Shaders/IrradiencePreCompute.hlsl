#include "PBRFunctions.hlsli"

struct InputVertex
{
    float4 position : SV_Position;
    float4 local : LOCALPOS;
};

TextureCube samplerCube : register(t0);

SamplerState SimpleSampler : register(s0);

float4 main(InputVertex v) : SV_TARGET
{
    float3 normal = normalize(v.local);
    
    float3 irradiance = 0.0;
    
    float3 right = cross(float3(0.0, 1.0, 0.0), normal);
    float3 up = cross(normal, right);
    
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 
            
            irradiance += samplerCube.Sample(SimpleSampler, sampleVector.xyz).rgb * cos(theta) * sin(theta);
            nrSamples++;

        }
    }
    
    irradiance = PI * irradiance * (1.0 / nrSamples);
    
    return float4(irradiance, 1);
}