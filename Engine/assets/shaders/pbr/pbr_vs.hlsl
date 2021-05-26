
#include "../include/common_include.hlsl"
#include "../include/vs_include.hlsl"


PSInPBR main(VSInDefault input)
{
	PSInPBR output;

	float4 pos = float4(input.pos, 1);

	output.worldPos = mul(pos, m);
	output.pos = mul(pos, mul(m, cameraVP));
	output.uv = input.uv * uvRepeat;
	output.lightViewPos = mul(pos, mul(m, lightVP));

	output.normal    = normalize(mul(input.normal,    (float3x3)m));
	output.tangent   = normalize(mul(input.tangent,   (float3x3)m));
	output.bitangent = normalize(mul(input.bitangent, (float3x3)m));

	return output;
}

