#pragma pack_matrix(row_major)


cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
}

float4 main(float3 posL: POSITION) : SV_POSITION
{
	return mul(float4(posL, 1), worldMatrix);
}