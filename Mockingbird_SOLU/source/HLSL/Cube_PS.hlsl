#pragma pack_matrix(row_major)

texture2D baseTexture : register(t0);


SamplerState filters : register(s0);

cbuffer SCENE : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

struct P_IN
{
	float4 posH : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal  : NORMAL;
};

float4 main(P_IN input) : SV_TARGET
{
	return float4(1.0,0.0,0.0,1.0);
}