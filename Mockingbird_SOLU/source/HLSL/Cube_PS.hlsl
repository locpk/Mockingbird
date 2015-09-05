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
	float3 posW : POSITION;
	float2 tex : TEXCOORD;
	float3 normal  : NORMAL;
};

float4 main(P_IN input) : SV_TARGET
{

	float4 ori = baseTexture.Sample(filters, input.tex);
	//Dir light


	float3 dir = float3(0.58, -0.58, 0.58);
	float4 lightColor = float4(1, 0, 0, 1);
	float r = saturate(dot(-dir, input.normal));
	float4 DIRColor = r  *lightColor * ori;

	////point Light
	//float3 plightPos = float3(0, 1, 0);
	//float4 plightColor = float4(0, 1, 0, 1);
	//float3 plightDir = normalize(plightPos - input.posW);
	//float plightR = saturate(dot(-plightDir, input.normal));
	//float4 PointLightColor = plightR  *plightColor * ori;



	return    /*PointLightColor + */DIRColor + ori;
}