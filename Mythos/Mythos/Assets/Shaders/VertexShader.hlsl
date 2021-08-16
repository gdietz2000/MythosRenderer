#pragma pack_matrix(row_major)

struct TempVertex
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct OutputVertex
{
    float4 position : SV_Position;
    float4 color : COLOR;
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
    output.color = temp.color;
	return output;
}