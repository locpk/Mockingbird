

TextureCube skybox;
SamplerState samplerstate;

struct P_IN
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

float4 main(P_IN input) : SV_TARGET
{

	return skybox.Sample(samplerstate,input.texCoord);
}