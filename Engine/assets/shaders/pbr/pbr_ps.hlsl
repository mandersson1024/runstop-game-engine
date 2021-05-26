
#include "../include/common_include.hlsl"
#include "../include/ps_include.hlsl"

static const float PI = 3.14159265359;

float3 GetBaseColor(float2 uv)
{
    return baseColorTint * tex.Sample(smpler, uv);
}

float3 GetNormal(float3x3 TBN, float2 uv)
{

    float3 tangentSpaceNormal = NormalMap.Sample(smpler, uv);
    tangentSpaceNormal = normalize(tangentSpaceNormal * 2 - 1);
    return mul(tangentSpaceNormal, TBN);

}

float GetRoughness(float2 uv)
{
    return lerp(roughnessRange.x, roughnessRange.y, RoughnessMap.Sample(smpler, uv).r);
}

float GetMetallic(float2 uv)
{
    return lerp(metallicRange.x, metallicRange.y, MetallicMap.Sample(smpler, uv).r);
}

float GetAmbientOcclusion(float2 uv)
{
    return lerp(ambientOcclusionRange.x, ambientOcclusionRange.y, AmbientOcclusionMap.Sample(smpler, uv).r);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1) + 1);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1);
    float k = (r * r) / 8;

    float num = NdotV;
    float denom = NdotV * (1 - k) + k;

    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0);
    float NdotL = max(dot(N, L), 0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1 - F0) * pow(max(1 - cosTheta, 0), 5);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    float3 smoothness = 1 - float3(roughness, roughness, roughness);
    return F0 + (max(smoothness, F0) - F0) * pow(max(1 - cosTheta, 0), 5);
}

float3 ReflectanceEquation(float3 normal, float3 worldPos, float3 albedo, float roughness, float metallic)
{
    float3 N = normalize(normal);
    float3 V = normalize(cameraPosition - worldPos);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    // calculate per-light radiance
    float3 L = normalize(lightPosition - worldPos);
    float3 H = normalize(V + L);
    float distance = length(lightPosition - worldPos);
    float attenuation = 1 / (distance * distance);
    float3 radiance = lightColor * attenuation;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0), F0);

    float3 kS = F;
    float3 kD = float3(1, 1, 1) - kS;
    kD *= 1 - metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0) * max(dot(N, L), 0);
    float3 specular = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0);
    
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float4 main(PSInPBR input) : SV_TARGET
{
    //return float4(baseColorTint, 1);
    //return float4(roughnessRange.x, roughnessRange.y, 0, 1);
    //return float4(metallicRange.x, metallicRange.y, 0, 1);
    //return float4(ambientOcclusionRange.x, ambientOcclusionRange.y, 0, 1);

    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    float3 normal = GetNormal(TBN, input.uv);
    float3 basecolor = GetBaseColor(input.uv);
    float roughness = GetRoughness(input.uv);
    float metallic = GetMetallic(input.uv);
    float ao = GetAmbientOcclusion(input.uv);

    float3 N = normalize(normal);
    float3 V = normalize(cameraPosition - input.worldPos);
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), basecolor, metallic);

    float3 R = reflect(-V, N);
    float3 F = FresnelSchlickRoughness(max(dot(N, V), 0), F0, roughness);

    float3 kS = F;
    float3 kD = 1 - kS;
    kD *= 1 - metallic;

    float3 irradiance = irradianceMap.Sample(SkyboxSampler, N).rgb;
    float3 diffuse = irradiance * basecolor;

    const float MAX_REFLECTION_LOD = 4;
    float3 prefilteredColor = environmentMap.SampleLevel(SkyboxSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    
    // TODO: This is a hack, but it removes specular aliasing almost completely for the monkey scene
    prefilteredColor = min(prefilteredColor, 0.5);

    float2 brdf = BrdfLUT.Sample(BrdfSampler, float2(max(dot(N, V), 0), roughness)).rg;
    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    //
    // AMBIENT LIGHT
    //

    if (debugIgnoreDiffuseIBL) { diffuse = 0; }
    if (debugIgnoreSpecularIBL) { specular = 0; }

    float3 ambient = (kD * diffuse + specular) * ao;

    // 
    // LIGHT SOURCE
    //

    float3 Lo = 0;

    if (!IsInShadow(input.lightViewPos))
    {
        Lo = ReflectanceEquation(normal, input.worldPos, basecolor, roughness, metallic);
    }

    if (debugIgnoreLightSources) { Lo = 0; }

    //
    // TOTAL LIGHT
    //

    float3 color = ambient + Lo;

    color = Tonemap(color);

    return float4(color, 1);
}
