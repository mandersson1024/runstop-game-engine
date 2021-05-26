

struct PSInDefault
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 lightViewPos : TEXCOORD1;
	float3 relativeLightPos : TEXCOORD2;
};

struct PSInMinimal
{
	float4 pos : SV_POSITION;
};

struct PSInPBR
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
	float3 tangent : TANGENT;
	float3 bitangent : BINORMAL;
};

struct PSInSkybox
{
	float4 pos : SV_POSITION;
	float3 direction : TEXCOORD0;
};

