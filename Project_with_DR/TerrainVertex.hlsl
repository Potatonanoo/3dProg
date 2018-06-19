Texture2D shaderTexture : register(t0);

SamplerState heightMapSampler {
	Filter = MIN_MAG_LINNEAR_MIP_POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VS_IN {
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 BoundsY : TEXCOORD1;
};

struct VS_OUT {
	float3 Pos : POSITON;
	float2 Tex : TEXCOORD0;
	float3 normal : NORMAL;

	//float3 BoundsY : TEXCOORD1;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.normal = input.normal;
	output.Pos = input.Pos;

	output.Tex = input.Tex;
	//output.BoundsY = input.BoundsY;

	return output;
}