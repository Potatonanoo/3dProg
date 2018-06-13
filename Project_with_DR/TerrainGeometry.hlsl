
cbuffer GS_CONATANT_BUFFER : register(b0) {
	matrix world;
	matrix view;
	matrix proj;
	matrix trans;
	matrix lightViewProj;
};

struct GS_IN {
	float3 Pos : POSITON;
	float2 Tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 BoundsY : TEXCOORD1;
};

struct GS_OUT {
	float4 Pos		: SV_POSITION;
	float4 PosW		: POSITION;
	float2 Tex		: TEXCOORD0;
	float4 lPos		: TEXCOORD1;
	float3 normal	: NORMAL;
};

[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output) {
	GS_OUT element = (GS_OUT)0;
	// Calculate the normal to determine the direction for the new triangle to be created 
	// (closer to the camera)

	for (uint i = 0; i < 3; i++)
	{
		// the input position must be multiplied with the World matrix for the World position to Pixelshader
		element.PosW = mul(float4(input[i].Pos.xyz, 1.0f), world);
		// the input position must be multiplied with the WorldViewProj matrix for the WorldViewProj
		element.Pos = mul(float4(input[i].Pos.xyz, 1.0f), trans);
		element.lPos = mul(float4(input[i].Pos.xyz, 1.0f), mul(world, lightViewProj));

		element.normal = mul(float4(input[i].normal, 1.0f), (float3x3)world);
		element.Tex = input[i].Tex;

		output.Append(element); 
		// output is seen as list and adds the most recent vertex to the last pos
	}
}