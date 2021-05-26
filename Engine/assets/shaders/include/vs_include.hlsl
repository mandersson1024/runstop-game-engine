

struct VSInDefault
{
	float3 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BINORMAL;
	float2 uv : TEXCOORD;
};

struct VSInMinimal
{
	float3 pos : SV_POSITION;
};


cbuffer SceneConstantsVS : register(b0)
{
	matrix cameraVP;
	matrix lightVP;
	float3 lightPosition;
}

cbuffer ModelConstantsVS : register(b1)
{
	matrix m;
	float uvRepeat;
};

