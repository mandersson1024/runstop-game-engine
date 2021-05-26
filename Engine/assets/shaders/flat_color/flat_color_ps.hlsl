
#include "../include/common_include.hlsl"
#include "../include/ps_include.hlsl"


float4 main(PSInMinimal input) : SV_TARGET
{
	float4 col = float4(baseColorTint, 1);

	return col;
}
