#pragma pack_matrix(row_major)

struct TempVertex
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
};

struct OutputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 world : WORLDPOS;
};

cbuffer WVP : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
}

OutputVertex main( TempVertex temp )
{
    OutputVertex output = (OutputVertex) 0;
    output.position = float4(temp.position, 1);
    output.position = mul(output.position, World);
    output.world = output.position.xyz;
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.uv = temp.uv;
    output.normal = mul(float4(temp.normal,1), World);
	return output;
}