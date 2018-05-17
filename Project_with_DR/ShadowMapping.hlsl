cbuffer ContantBuffer : register(b0)
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;  // The view matrix of the light
	float4x4 ProjectionMatrix;
	float4x4 LightViewMatrix;
};

struct VS_IN
{
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 texCoord		: TEXCOORD;
};

float4 ShadowMapVS(VS_IN input) : SV_POSITION
{
	float posL = float4(input.pos, 1.f);

	posL = mul(WorldMatrix, posL);
	posL = mul(LightViewMatrix, posL);
	posL = mul(ProjectionMatrix, posL);

	return posL;
}