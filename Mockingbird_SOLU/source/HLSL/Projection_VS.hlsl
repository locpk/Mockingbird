#pragma pack_matrix(row_major)

struct V_IN
{
	float3 posL : POSITION;
	float3 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4x4 world : WORLD;
	uint InstanceID: SV_InstanceID;
};
struct V_OUT
{
	float4 posH : SV_POSITION;
	float3 texOut : TEXCOORD;
	float3 normalOut  : NORMAL;
};

cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}
V_OUT main(V_IN input)
{

	V_OUT output = (V_OUT)0;
	float4 localH = float4(input.posL,1);
	localH = mul(localH, input.world);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	output.posH = localH;
	output.texOut = input.tex;
	output.normalOut = (float3)mul(float4(input.normal,0), input.world);
	return output;
}
