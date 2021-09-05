#pragma pack_matrix(row_major)

struct InputVertex
{
    float3 pos : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
};

struct PixelVertex
{
    float4 pos : SV_Position;
    float4 local : LOCALPOS;
};

cbuffer WVP : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};

PixelVertex main( InputVertex v )
{
    PixelVertex pixel = (PixelVertex) 0;
    pixel.local = float4(v.pos, 1);
    pixel.pos = mul(pixel.local, World);
    pixel.pos = mul(pixel.pos, View);
    pixel.pos = mul(pixel.pos, Proj);
	return pixel;
}