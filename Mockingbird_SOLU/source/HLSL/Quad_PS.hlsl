
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
	// 0.21 R + 0.72 G + 0.07 B.
	tex.b = tex.g = tex.r = 0.21 * tex.r + 0.72 * tex.g + 0.07 * tex.b;
	//tex.b = tex.g = tex.r = (tex.b + tex.g + tex.r) / 3;
	return tex;
}