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
	float3 posW : POSITION;
	float2 texOut : TEXCOORD;
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
	// ensures translation is preserved during matrix multiply  
	float4 localH = float4(input.posL, 1);
	localH = mul(localH, worldMatrix);
	output.posW = localH.xyz;
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	output.posH = localH;
	input.tex.y = 1 - input.tex.y;
	output.texOut = input.tex.xy;
	float4 nor = float4(input.normal, 0);
	nor = mul(nor, worldMatrix);
	output.normalOut = float3(nor.xyz);
	return output; // send projected vertex to the rasterizer stage
}
