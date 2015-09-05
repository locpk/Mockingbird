
#pragma pack_matrix(row_major)

TextureCube baseTexture : register(t0);


SamplerState filters : register(s0);

struct P_IN
{
	float4 posH : SV_POSITION;
	float3 tex : TEXCOORD;
	float3 normal  : NORMAL;
};
float4 main(P_IN input) : SV_TARGET
{

	return baseTexture.Sample(filters,input.tex);
}