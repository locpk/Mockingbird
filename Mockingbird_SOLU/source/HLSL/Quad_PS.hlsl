
#pragma pack_matrix(row_major)

Texture2D baseTexture : register(t1);


SamplerState filters : register(s0);

struct P_IN
{
	float4 posH : SV_POSITION;
	float2 tex : TEXCOORD;
};
float4 main(P_IN input) : SV_TARGET
{
	float4 tex = baseTexture.Sample(filters,input.tex);
	tex.r = 0;
	return tex;
}