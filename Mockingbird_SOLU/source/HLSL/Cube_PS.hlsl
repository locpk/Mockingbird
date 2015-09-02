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
	float3 tex : TEXCOORD;
	float3 normal  : NORMAL;
};

float4 main(P_IN input) : SV_TARGET
{
	float4 diffuse = baseTexture.Sample(filters, input.tex.xy);
	float4 colorSwap;
	colorSwap.a = diffuse.b;
	colorSwap.r = diffuse.g;
	colorSwap.g = diffuse.r;
	colorSwap.b = diffuse.a;
	return colorSwap;
}