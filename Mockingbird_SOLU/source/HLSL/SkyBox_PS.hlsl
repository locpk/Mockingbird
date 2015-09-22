
#pragma pack_matrix(row_major)

TextureCube sunrise : register(t0);
TextureCube sunset : register(t1);


SamplerState filters : register(s0);

cbuffer SCENE : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	bool hasNormal;
	float3 cameraPos;
	float percent;
}

struct P_IN
{
	float4 posH : SV_POSITION;
	float3 tex : TEXCOORD;
	float3 normal  : NORMAL;
};
float4 main(P_IN input) : SV_TARGET
{
	

	float4 morning = sunrise.Sample(filters,input.tex);
	float4 evening = sunset.Sample(filters, input.tex);

	return lerp(morning, evening, percent);
}