
cbuffer ConstantBuffer : register(b0) {
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 LightViewMatrix;
	float4x4 LightProjectionMatrix;
};

struct GS_IN {
	float4 Pos : POSITON;
	float2 Tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 BoundsY : TEXCOORD1;
};

struct GS_OUT {
	float4 Pos		: SV_POSITION;
	float4 PosW		: POSITION;
	float3 ViewPos	: POSITION1;
	float2 Tex		: TEXCOORD0;
	float4 lPos		: TEXCOORD1;
	float3 normal	: NORMAL;
};

[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output) {
	GS_OUT element = (GS_OUT)0;
	// Calculate the normal to determine the direction for the new triangle to be created 
	// (closer to the camera)

	//element.Pos = input.Pos;
	

	for (uint i = 0; i < 3; i++)
	{
		// the input position must be multiplied with the World matrix for the World position to Pixelshader
		element.PosW = mul(float4(input[i].Pos.xyz, 1.0f), WorldMatrix);
		// the input position must be multiplied with the WorldViewProj matrix for the WorldViewProj
		element.Pos = mul(float4(input[i].Pos.xyz, 1.0f), mul(WorldMatrix, mul(ViewMatrix, ProjectionMatrix)));
		//element.lPos = mul(float4(input[i].Pos.xyz, 1.0f), mul(WorldMatrix, LightProjectionMatrix));

		element.normal = mul(float4(input[i].normal, 1.0f), (float3x3)WorldMatrix);
		//element.normal = normalize(element.normal);
		element.Tex = input[i].Tex;

		output.Append(element); 
		// output is seen as list and adds the most recent vertex to the last pos
	}
	
}