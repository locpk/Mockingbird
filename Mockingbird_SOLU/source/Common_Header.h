#pragma once
//General Header files include
#include <d3d11.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

struct VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT3 uv;
	XMFLOAT3 normal;
	XMFLOAT4 tangent;
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
	BOOL hasNormal;
};