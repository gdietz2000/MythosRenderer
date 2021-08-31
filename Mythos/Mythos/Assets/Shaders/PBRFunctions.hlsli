#define PI 3.1415926535
#define GAMMA 2.6

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float distributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float geometrySchlick(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float geometrySmith(float3 n, float3 v, float3 l, float roughness)
{
    float NdotV = max(dot(n, v), 0.0);
    float NdotL = max(dot(n, l), 0.0);
    float ggx1 = geometrySchlick(NdotV, roughness);
    float ggx2 = geometrySchlick(NdotL, roughness);
    return ggx1 * ggx2;
}