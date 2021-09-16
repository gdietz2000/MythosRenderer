#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 pos : POSITION;
    float2 tex : UV;
};

struct PixelVertex
{
    float4 pos : SV_Position;
    float2 tex : UV;
};

cbuffer WVP : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
}

PixelVertex main( InputVertex v )
{
    PixelVertex output = (PixelVertex) 0;
    output.pos = mul(float4(v.pos, 1), World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Proj);
    output.tex = v.tex;
	return output;
}