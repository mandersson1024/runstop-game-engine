

Texture2D tex : register(t0);
SamplerState smpler : register(s0);

Texture2D shadowMap : register(t1);
SamplerState shadowMapSampler : register(s1);

TextureCube skybox : register(t2);
TextureCube irradianceMap : register(t3);
TextureCube environmentMap : register(t4);
SamplerState SkyboxSampler : register(s2);

Texture2D BrdfLUT : register(t5);
SamplerState BrdfSampler : register(s3);

Texture2D NormalMap : register(t6);
Texture2D RoughnessMap : register(t7);
Texture2D MetallicMap : register(t8);
Texture2D AmbientOcclusionMap : register(t9);

cbuffer SceneConstantsPS : register(b0)
{
    float3 cameraPosition;
    float3 lightPosition;
    float3 lightColor;
	float3 lightDirection;
    float debugSkyboxMipMapLevel;
    bool debugIgnoreLightSources;
    bool debugIgnoreDiffuseIBL;
    bool debugIgnoreSpecularIBL;
}

cbuffer ModelConstantsPS : register(b1)
{
    float3 baseColorTint;
    float _pad_0;
    float2 roughnessRange;
    float2 metallicRange;
    float2 ambientOcclusionRange;
    bool debugInvertColors;
    float _pad_1;
};


float4 InvertColors(float4 color, bool b)
{
	if (b)
	{
		color.rgb = 1 - color.rgb;
	}

	return color;
}

bool IsInShadow(float4 lightViewPos)
{
    float lightDepthValue = lightViewPos.z / lightViewPos.w;

    float2 projectTexCoord;
    projectTexCoord.x = lightViewPos.x / lightViewPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPos.y / lightViewPos.w / 2.0f + 0.5f;

    if (lightDepthValue < 1 && (saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        float depthValue = shadowMap.Sample(shadowMapSampler, projectTexCoord).r;
        float bias = 0.001f;

        if (lightDepthValue - bias > depthValue)
        {
            return true;
        }
    }

    return false;
}

float3 Tonemap_Reinhard(float3 color)
{
    return color / (color + float3(1, 1, 1));
}

float3 Tonemap_Reinhard2(float3 color)
{
    const float L_white = 4.0;
    return (color * (1.0 + color / (L_white * L_white))) / (1.0 + color);
}

float3 Tonemap_ACES(float3 color) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (color * (a * color + b)) / (color * (c * color + d) + e);
}

float3 Tonemap(float3 color)
{
    return Tonemap_ACES(color);
    //return Tonemap_Reinhard(color);
}

