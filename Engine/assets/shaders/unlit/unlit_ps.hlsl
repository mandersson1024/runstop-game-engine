
#include "../include/common_include.hlsl"
#include "../include/ps_include.hlsl"


float4 main(PSInDefault input) : SV_TARGET
{
	float4 texcol = tex.Sample(smpler, input.uv);
	float4 col = texcol * float4(baseColorTint, 1);

	return col;
}
