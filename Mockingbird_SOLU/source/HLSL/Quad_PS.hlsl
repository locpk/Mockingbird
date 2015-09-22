
#pragma pack_matrix(row_major)

Texture2D baseTexture : register(t0);

cbuffer TIME : register(b0)
{
	float totalTime;
}
SamplerState filters : register(s0);

struct P_IN
{
	float4 posH : SV_POSITION;
	float2 tex : TEXCOORD;
};

float rand_1_05(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

float4 main(P_IN input) : SV_TARGET
{
	
	[flatten]
	if (cos(totalTime * 2) >= 0.9)
	{
		input.tex.x = rand_1_05(input.tex);
		input.tex.y = rand_1_05(input.tex);
	}

	float4 tex = baseTexture.Sample(filters, input.tex);
	//tex.a = sin(totalTime * 2);
	// 0.21 R + 0.72 G + 0.07 B.
	tex.b = tex.g = tex.r = 0.21 * tex.r + 0.72 * tex.g + 0.07 * tex.b;
	//tex.b = tex.g = tex.r = (tex.b + tex.g + tex.r) / 3;
	return saturate(tex);
}