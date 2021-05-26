
#include "../include/common_include.hlsl"
#include "../include/vs_include.hlsl"


PSInMinimal main(VSInMinimal input)
{
	PSInMinimal output;

	matrix mvp = mul(m, lightVP);
	output.pos = mul(float4(input.pos, 1.0f), mvp);

	return output;
}

