#pragma once
//General Header files include
#include <d3d11.h>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

struct VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

struct Object
{
	XMMATRIX _translation;
};

struct MyVertex
{
	XMFLOAT4  pos;
	XMFLOAT4  color;
	unsigned int u;
	unsigned int v;
};



struct Scene
{
	XMMATRIX _view;
	XMMATRIX _proj;
};