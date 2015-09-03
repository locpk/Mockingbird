#pragma pack_matrix(row_major)

struct V_IN
{
	float4 posL : POSITION;
	float4 color : COLOR;
};
struct V_OUT
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};
cbuffer OBJECT : register(b2)
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
	float4 localH = input.posL;
	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);
	output.posH = localH;
	output.color = input.color;
	return output; 
}
