
#include "../include/common_include.hlsl"
#include "../include/ps_include.hlsl"


float4 main(PSInSkybox input) : SV_TARGET
{
	float3 col = skybox.SampleLevel(smpler, input.direction, debugSkyboxMipMapLevel).rgb;

	col = Tonemap(col);

	return float4(col, 1);
}
