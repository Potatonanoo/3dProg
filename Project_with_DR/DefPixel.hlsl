Texture2D shaderTexture		: register( t0 );
SamplerState SampleType		: register( s0 );

struct PS_IN
{
	float4 pos_SV		: SV_POSITION;
	float4 pos_WS		: POSITION;
	float2 texCoord		: TEXCOORD;
	float3 normal		: NORMAL;
	float4 lightViewPos	: TEXCOORD2;
};

struct PS_OUT
{
	float4 normal			: SV_Target0;
	float4 diffuse			: SV_Target1;
	float4 pos				: SV_Target2;
	float4 lightViewPos		: SV_Target3;
};

PS_OUT PS_main( in PS_IN input)
{
	PS_OUT output;

	float4 normal = float4(normalize( input.normal ), 1.f);

	// Output G-buffer values
	output.normal = normal;
	output.diffuse = shaderTexture.Sample(SampleType, input.texCoord);
	output.pos = input.pos_WS;
	output.lightViewPos = input.lightViewPos;

	return output;
};