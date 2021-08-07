struct InputVertex
{
    float4 position : SV_Position;
};

float4 main(InputVertex v) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}