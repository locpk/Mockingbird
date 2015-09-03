#pragma pack_matrix(row_major)

texture2D baseTexture : register(t0);


SamplerState filters : register(s0); 

struct P_IN
{
	float4 posH : SV_POSITION;
	float3 tex : TEXCOORD;
	float3 normal  : NORMAL;
};

float4 main( P_IN input ) : SV_TARGET
{
	input.tex.x = input.tex.x * 0.25;
	float4 diffuse = baseTexture.Sample(filters, input.tex.xy);
	float4 colorSwap;
	colorSwap.a = diffuse.b;
	colorSwap.a = colorSwap.a - 0.1;
	colorSwap.r = diffuse.g;
	colorSwap.g = diffuse.r;
	colorSwap.b = diffuse.a;
	return colorSwap;
}