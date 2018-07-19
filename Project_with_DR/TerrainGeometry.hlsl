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
	float2 Tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float4 lightViewPos	: TEXCOORD2;
};

[maxvertexcount(12)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output) 
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = input[i].Pos;

		//float height = heightmap.Sample(SampleType, input[i].Tex).r;
		//element.Pos = float4(element.Pos.x, height, element.Pos.z, 1.f); //height;


		element.Pos = mul(WorldMatrix, element.Pos);
		element.PosW = element.Pos;
		element.Pos = mul(ViewMatrix, element.Pos);
		element.Pos = mul(ProjectionMatrix, element.Pos);

		// Used in shadow mapping
		element.lightViewPos = mul(WorldMatrix, input[i].Pos);
		element.lightViewPos = mul(LightViewMatrix, element.lightViewPos);
		element.lightViewPos = mul(LightProjectionMatrix, element.lightViewPos);

		element.Tex = input[i].Tex;

		element.normal = mul(WorldMatrix, input[i].normal);

		output.Append(element);
	}
	output.RestartStrip();
}