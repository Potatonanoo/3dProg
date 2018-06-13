Texture2D shaderTexture				: register(t0);
Texture2D DiffuseTexture			: register (t1);
Texture2D ShadowMappingTexture		: register(t4);

SamplerState SampleType				: register(s0);
SamplerState shadowSampler			: register(s1);

cbuffer ContantBuffer : register(b0)
{
	float4x4 WorldMatrix;
};

struct PS_IN
{
	float4 Pos		: SV_POSITION;
	float4 Posw		: POSITION;
	float2 Tex		: TEXCOORD0;
	float4 lPos		: TEXCOORD1;
	float3 normal	: NORMAL;
};

float4 PS_main(in PS_IN input) : SV_Target
{
	
	float3 lightPos = float3(0.f, 0.f, -5.f);
	float3 lightRay = (lightPos - input.Pos);
	float4 normal = normalize(float4(input.normal,1.0f));

	float lightDepth = input.lPos.z / input.lPos.w; // pixeldepth for shadows
	input.lPos.xy /= input.lPos.w; //light pos in NDC

	// light pos in ndc
	float2 sMapTexture = float2(0.5f * input.lPos.x + 0.5f, -0.5f * input.lPos.y + 0.5f);

	float shdw;
	if ((ShadowMappingTexture.Sample(SampleType, sMapTexture).r + 0.0001f) < lightDepth) {
		shdw = 0.0f;
	}
	else {
		shdw = 1.0f;
	}
	
	float4 diffuse = shaderTexture.Sample(SampleType, input.Tex);
	float3 texColor = shaderTexture.Sample(SampleType, input.Tex).xyz;
	float4 color = float4(texColor, 1.0f);
	//pos = input.posW;

	return color*diffuse*shdw;
	//return float4(texColor, 1);

};