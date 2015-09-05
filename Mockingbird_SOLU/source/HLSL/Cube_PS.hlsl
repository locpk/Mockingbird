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

	float3 dir = float3(0.58,-0.58,0.58);
	float4 lightColor = float4(2, 0.95, 0.878, 1);
	float r = saturate(dot(-dir, input.normal));
	return  r  *lightColor * baseTexture.Sample(filters, input.tex) + baseTexture.Sample(filters, input.tex);
}