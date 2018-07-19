Texture2D heightmap			: register(t0);
SamplerState SampleType		: register(s0);

//SamplerState heightMapSampler {
//	Filter = MIN_MAG_LINNEAR_MIP_POINT;
//
//	AddressU = CLAMP;
//	AddressV = CLAMP;
//};

struct VS_IN {
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 BoundsY : TEXCOORD1;
};

struct VS_OUT {
	float4 Pos : POSITON;
	float2 Tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 BoundsY : TEXCOORD1;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1.f);

	//float height = heightmap.SampleLevel(SampleType, input.Tex, 0).r;
	///output.Pos = float4(input.Pos.x, input.Pos.y + height, input.Pos.z, 1.f);
	/*if (height == 0.f)
	{
		output.Pos.y = 2.f;
	}*/

	output.Tex = input.Tex;
	output.normal = input.normal;
	//output.BoundsY = input.BoundsY;

	return output;
}