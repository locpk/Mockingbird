#pragma pack_matrix(row_major)

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	uint vierport:  SV_ViewportArrayIndex;
};

cbuffer SCENE : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

cbuffer VIEW : register(b1)
{
	float4x4 viewMatrix1;
	float4x4 viewMatrix2;
}


[maxvertexcount(8)]
void main(
	point float4 input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
	)
{
	GSOutput verts[4] =
	{
		float4(0,0,0,1), float2(0,0),0,
		float4(0,0,0,1), float2(0.5,0),0,
		float4(0,0,0,1), float2(0,1),0,
		float4(0,0,0,1), float2(0.5,1),0
	};
	verts[0].pos = input[0];
	verts[1].pos = input[0];
	verts[2].pos = input[0];
	verts[3].pos = input[0];


	uint i = 0;
	for ( i = 0; i < 4; i++)
	{
		verts[i].pos = mul(verts[i].pos, viewMatrix1);
	}

	verts[0].pos.x -= 1;
	verts[1].pos.x += 1;
	verts[2].pos.x -= 1;
	verts[2].pos.y -= 2;
	verts[3].pos.x += 1;
	verts[3].pos.y -= 2;
	
	

	for ( i = 0; i < 4; i++)
	{
		verts[i].pos = mul(verts[i].pos, projectionMatrix);
	}
	output.Append(verts[0]);
	output.Append(verts[1]);
	output.Append(verts[2]);
	output.Append(verts[3]);
	output.RestartStrip();


	verts[0].pos = input[0];
	verts[1].pos = input[0];
	verts[2].pos = input[0];
	verts[3].pos = input[0];
	for ( i = 0; i < 4; i++)
	{
		verts[i].pos = mul(verts[i].pos, viewMatrix2);
	}

	verts[0].vierport = 1;
	verts[0].pos.x -= 1;
	verts[0].tex.x = 0.5;
	verts[1].pos.x += 1;
	verts[1].vierport = 1;
	verts[1].tex.x = 1;
	verts[2].pos.x -= 1;
	verts[2].pos.y -= 2;
	verts[2].vierport = 1;
	verts[2].tex.x = 0.5;
	verts[3].pos.x += 1;
	verts[3].pos.y -= 2;
	verts[3].vierport = 1;
	verts[3].tex.x = 1;
	for ( i = 0; i < 4; i++)
	{
		verts[i].pos = mul(verts[i].pos, projectionMatrix);
	}
	output.Append(verts[0]);
	output.Append(verts[1]);
	output.Append(verts[2]);
	output.Append(verts[3]);
	output.RestartStrip();

}