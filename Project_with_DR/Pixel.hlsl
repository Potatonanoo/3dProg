// Textures
Texture2D NormalTexture			: register (t0);
Texture2D DiffuseTexture		: register (t1);
texture2D PositionTexture		: register (t2);
texture2D lposTexture			: register (t3);
texture2D ShadowMappingTexture	: register (t4);

// Sampler
SamplerState SampleType			: register(s0);
//SamplerState ComparisonSampler	: register(s1);

struct PS_IN
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 texCoord		: TEXCOORD;
};

float4 PS_main( in PS_IN input) : SV_Target
{
	float3 normal = NormalTexture.Sample(SampleType, input.texCoord).xyz;
	float3 diffuse = DiffuseTexture.Sample(SampleType, input.texCoord).xyz;
	float3 position = PositionTexture.Sample(SampleType, input.texCoord).xyz;
	float4 lpos = lposTexture.Sample(SampleType, input.texCoord);

	float2 projectTexCoord;
	projectTexCoord.x = lpos.x / lpos.w / 2.0f + 0.5f;
	projectTexCoord.y = -lpos.y / lpos.w / 2.0f + 0.5f;

	if (saturate(projectTexCoord.x) == projectTexCoord.x && saturate(projectTexCoord.y) == projectTexCoord.y)
	{
		float depthValue = ShadowMappingTexture.Sample(SampleType, projectTexCoord).r;

		float lightDepthValue = lpos.z / lpos.w;
		lightDepthValue -= 0.001f;

		if (lightDepthValue > depthValue && depthValue == 0) // this is not good! depthvalue needs to be something > 0 sometimes (which it isn't atm)
		{
			float3 lightPos = float3(0.f, 0.f, -5.f);
			float3 lightRay = lightPos - position;

			float lightIntensity;

			if (dot(normalize(normal), normalize(lightRay)) > 0)
			{
				lightIntensity = max(8.f / length(lightRay), 0.2f);
			}
			else
				lightIntensity = 0.2f;

			return float4(diffuse, 1.0f) *lightIntensity;
		}
	}

	return float4(diffuse, 1.0f) * 0.2f;




	//lpos.xyz /= lpos.w; // re-homogenize position
	////if (lpos.x < -1.0f || lpos.x > 1.0f ||
	////	lpos.y < -1.0f || lpos.y > 1.0f ||
	////	lpos.z < 0.0f	|| lpos.z > 1.0f) return float4(diffuse, 1.0f) * 0.2f; // outside of the spotlight
	//lpos.x = lpos.x / 2 + 0.5f;
	//lpos.y = lpos.y / -2 + 0.5f;	/*if (shadowMapDepth < lpos.z)		return float4(diffuse, 1.0f);*/
}