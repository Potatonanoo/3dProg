//Texture2D shaderTexture			: register(t0);
//SamplerState SampleType			: register(s0);


struct PS_IN
{
	float4 Pos		: SV_POSITION;
	float4 PosW		: POSITION;
	//float3 ViewPos	: POSITION1;
	float2 Tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct PS_OUT
{
	float4 normal			: SV_Target0;
	//float4 diffuse			: SV_Target1;
	float4 Pos				: SV_Target2;
};

PS_OUT PS_main(in PS_IN input)
{
	PS_OUT output;

	float4 normal = float4(normalize(input.normal), 1.f);

	//texColor = shaderTexture.Sample(SampleType, input.Tex).xyz;

	output.normal = normal;
	//output.diffuse = shaderTexture.Sample(SampleType, input.Tex);
	output.Pos = input.PosW;
	//output.lightViewPos = input.lightViewPos;

	return output;
};


