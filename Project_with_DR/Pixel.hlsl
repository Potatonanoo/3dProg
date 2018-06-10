// Textures
Texture2D NormalTexture			: register (t0);
Texture2D DiffuseTexture		: register (t1);
texture2D PositionTexture		: register (t2);
texture2D lightViewPosTexture	: register (t3);
texture2D ShadowMappingTexture	: register (t4);

// Samplers
SamplerState SampleType		: register(s0); // Wrap
SamplerState ClampSampler	: register(s1); // Clamp

struct PS_IN
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 texCoord		: TEXCOORD;
};

float4 PS_main( in PS_IN input) : SV_Target
{
	float4 normal = NormalTexture.Sample(SampleType, input.texCoord);
	float4 diffuse = DiffuseTexture.Sample(SampleType, input.texCoord);
	float4 position = PositionTexture.Sample(SampleType, input.texCoord);
	float4 lpos = lightViewPosTexture.Sample(SampleType, input.texCoord);

	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	float bias = 0.001f;

	float4 color = float4(0.f, 1.f, 1.f, 1.f);

	projectTexCoord.x = lpos.x / lpos.w / 2.0f + 0.5f;
	projectTexCoord.y = -lpos.y / lpos.w / 2.0f + 0.5f;

	if (saturate(projectTexCoord.x) == projectTexCoord.x && saturate(projectTexCoord.y) == projectTexCoord.y)
	{
		depthValue = ShadowMappingTexture.Sample(ClampSampler, projectTexCoord).r;

		float lightDepthValue = lpos.z / lpos.w;
		lightDepthValue -= 0.001f;

		if (lightDepthValue < depthValue)
		{
			float3 lightPos = float3(0.f, 0.f, -5.f);
			float3 lightRay = lightPos - position;

			float lightIntensity;

			if (dot(normalize(normal), normalize(lightRay)) > 0)
			{
				lightIntensity = max(6.f / length(lightRay), 0.2f);
			}
			else
				lightIntensity = 0.2f;

			return diffuse * lightIntensity;
		}
	}

	return diffuse * 0.2f;
}