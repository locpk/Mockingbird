#pragma pack_matrix(row_major)

texture2D baseTexture : register(t0);


SamplerState filters : register(s0);

struct P_IN
{
	float4 posH : SV_POSITION;
	float3 tex : TEXCOORD;
	float3 normal  : NORMAL;
};

float4 main(P_IN input) : SV_TARGET
{
	input.tex.x = input.tex.x * 0.25;
	return  baseTexture.Sample(filters, input.tex.xy);

}