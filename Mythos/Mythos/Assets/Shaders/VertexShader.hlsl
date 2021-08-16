#pragma pack_matrix(row_major)

struct TempVertex
{
    float4 position : POSITION;
    float2 uv : UV;
};

struct OutputVertex
{
    float4 position : SV_Position;
    float2 uv : UV;
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
    output.position = temp.position;
    output.position = mul(output.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    output.uv = temp.uv;
	return output;
}