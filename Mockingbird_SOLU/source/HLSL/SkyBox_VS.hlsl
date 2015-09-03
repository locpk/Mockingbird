

cbuffer OBJECT : register(b2)
{
	float4x4 worldMatrix;
}
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

VS_OUTPUT main(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 local = float4(inPos);
	local = mul(local, worldMatrix);
	local = mul(local, viewMatrix);
	local = mul(local, projectionMatrix);
	output.Pos = local.xyww;
	output.texCoord = inPos;
	return output;
}