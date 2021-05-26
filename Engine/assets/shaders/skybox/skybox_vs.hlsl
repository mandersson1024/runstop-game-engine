
#include "../include/common_include.hlsl"
#include "../include/vs_include.hlsl"


PSInSkybox main(VSInMinimal input)
{
	PSInSkybox output;

	output.pos = mul(float4(input.pos, 1), mul(m, cameraVP));
	output.direction = input.pos;

	return output;
}

