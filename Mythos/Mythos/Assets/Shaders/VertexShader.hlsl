struct TempVertex
{
    float4 position : POSITION;
};

struct OutputVertex
{
    float4 position : SV_Position;
};

OutputVertex main( TempVertex temp )
{
    OutputVertex output = (OutputVertex) 0;
    output.position = temp.position;
	return output;
}