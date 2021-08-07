struct InputVertex
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

float4 main(InputVertex v) : SV_TARGET
{
	return v.color;
}