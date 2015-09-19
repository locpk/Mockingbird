#pragma pack_matrix(row_major)

texture2D baseTexture : register(t0);
texture2D normTexture : register(t1);

SamplerState filters : register(s0);

cbuffer SCENE : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	bool hasNormal;
	float3 cameraPos;
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
	float3 biTangent :BITANGENT;
};

float4 main(P_IN input) : SV_TARGET
{
	float4 SPColor = float4(0.1f,0.1f,0.1f,64.0f);



	float4 ori = baseTexture.Sample(filters, input.tex);

	[flatten]
	if (hasNormal == true)
	{
		float4 normalMap = normTexture.Sample(filters, input.tex);
		normalMap = (2.0f*normalMap) - 1.0f;
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);
		float3 biTangent = cross(input.normal, input.tangent);
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);
		input.normal = (mul((float3)normalMap, texSpace));
		/*float4 normalMap = normTexture.Sample(filters, input.tex);
		normalMap = (2.0f*normalMap) - 1.0f;
		float3x3 TBN = float3x3(normalize(input.tangent), normalize(input.biTangent), normalize(input.normal));

		input.normal = mul(normalMap, TBN);*/

	}


	//AM Light
	float4 amColor = ori * AMlightColor;
	//Dir light
	float3 dir = float3(dlightDirection.xyz);
	float dirRatio = saturate(dot(-dir, input.normal));
	float4 DIRColor = dirRatio  * dlightColor * ori;

	/*float3 viewDir0 = normalize(cameraPos - input.posW);
	float4 viewDir = float4(viewDir0.x, viewDir0.y, viewDir0.z,0);
	float4 dir4 = float4(dlightDirection.x, dlightDirection.y, dlightDirection.z, 0);
	float4 halfvector = normalize(-dir4) + viewDir;
	float specFactor = pow(max(saturate(dot(input.normal, normalize(halfvector))), SPColor.w), 0);
	float4 speccolor = dlightColor*specFactor*SPColor;*/
	/*if (dirRatio > 0.0f)
	{
		float3 v = reflect(dlightDirection.xyz, input.normal);
		float specFactor = pow(max(dot(v, cameraPos), 0.0f), SPColor.w);
		DIRColor = saturate(DIRColor + specFactor* SPColor);
	}*/

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
	return  saturate(amColor + DIRColor  + PointLightColor*PointAttenuation + SpotLightColor*SpotAttenuation);
}





