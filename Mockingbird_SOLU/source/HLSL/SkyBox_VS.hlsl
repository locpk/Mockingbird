#pragma pack_matrix(row_major)
struct V_IN
{
	float3 posL : POSITION;
	float3 tex : TEXCOORD;
	float3 normal : NORMAL;
};
struct V_OUT
{
	float4 posH : SV_POSITION;
	float4 texOut : TEXCOORD;
	float3 normalOut  : NORMAL;
};
cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

V_OUT main(V_IN input)
{

	V_OUT output = (V_OUT)0;
	float4 localH = float4(input.posL, 1);
	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	output.posH = localH;

	output.texOut = float4(input.posL,1.0);
	float4 nor = float4(input.normal, 0);
	nor = mul(nor, worldMatrix);
	output.normalOut = float3(nor.xyz);
	return output;
}