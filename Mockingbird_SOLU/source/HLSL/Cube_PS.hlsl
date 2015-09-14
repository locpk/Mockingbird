#pragma pack_matrix(row_major)

texture2D baseTexture : register(t0);
texture2D normTexture : register(t1);

SamplerState filters : register(s0);

cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	bool hasNormal;
}

cbuffer LIGHTING : register(b2)
{
	float4 AMlightColor;
	float4 dlightColor;
	float4 dlightDirection;
	float4 plightColor;
	float4 plightPosition;
	float4 slightPosition;
	float4 slightColor;
	float4 sconeDirAndRatio;

}
struct P_IN
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float2 tex : TEXCOORD;
	float3 normal  : NORMAL;
	float3 tangent : TANGENT;
	bool hasNormal : HAS;
};

float4 main(P_IN input) : SV_TARGET
{

	float4 ori = baseTexture.Sample(filters, input.tex);


	if (input.hasNormal == true)
	{
		//Load normal from normal map
		float4 normalMap = normTexture.Sample(filters, input.tex);

		//Change normal map range from [0, 1] to [-1, 1]
		normalMap = (2.0f*normalMap) - 1.0f;

		//Make sure tangent is completely orthogonal to normal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

		//Create the biTangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "Texture Space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store in input.normal
		input.normal = normalize(mul(normalMap, texSpace));
	}


	//AM Light
	float4 amColor = ori * AMlightColor;
	//Dir light
	float3 dir = float3(dlightDirection.xyz);
	float dirRatio = saturate(dot(-dir, input.normal));
	float4 DIRColor = dirRatio  * dlightColor * ori;

	//point Light
	float3 plightPos = float3(plightPosition.xyz);
	float3 plightDir = normalize(plightPos - input.posW);
	float plightRatio = saturate(dot(plightDir, input.normal));
	float4 PointLightColor = plightRatio  *plightColor * ori;
	float PointAttenuation = 1.0f - saturate(length(float3(plightPos.xyz - input.posW)) / 10);

	//Spot Light
	float3 slightPos = float3(slightPosition.xyz);
	float3 slightDir = normalize(slightPos - input.posW);
	float surfaceRatio = saturate(dot(-slightDir, normalize(sconeDirAndRatio.xyz)));
	float SpotFactor = (surfaceRatio > sconeDirAndRatio.w) ? 1 : 0;
	float slightRatio = saturate(dot(slightDir, normalize(input.normal)));
	float4 SpotLightColor = SpotFactor* slightRatio *slightColor * ori;
	float SpotAttenuation = 1.0f - saturate(length(float3(slightPosition.xyz - input.posW)) / 5);

	//return PointLightColor + amColor;
	//return DIRColor + amColor;
	//return SpotLightColor*SpotAttenuation + amColor;
	return  saturate(amColor + DIRColor + PointLightColor*PointAttenuation + SpotLightColor*SpotAttenuation);
}





