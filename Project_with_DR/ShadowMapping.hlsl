cbuffer ContantBuffer : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;  // The view matrix of the light
	float4x4 ProjectionMatrix;
};

struct VS_IN
{
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 texCoord		: TEXCOORD;
};

struct PS_IN
{
	float4 pos			: SV_POSITION;
};

PS_IN ShadowMapVS(VS_IN input)
{
	PS_IN output;

	output.pos = mul(input.pos, WorldMatrix);
	output.pos = mul(output.pos, ViewMatrix);
	output.pos = mul(output.pos, ProjectionMatrix);

	return output;
}

void ShadowMapPS(PS_IN input)
{

}