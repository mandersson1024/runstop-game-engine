
#include "../include/common_include.hlsl"
#include "../include/vs_include.hlsl"


PSInDefault main(VSInDefault input)
{
	PSInDefault output;

	output.pos = mul(float4(input.pos, 1.0f), mul(m, cameraVP));
	output.normal = normalize(mul(input.normal, (float3x3)m));
	output.uv = input.uv * uvRepeat;

	return output;
}

