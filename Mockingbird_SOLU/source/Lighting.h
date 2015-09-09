#pragma once
#include<DirectXMath.h>
using namespace DirectX;
namespace Lighting
{
	struct AMLight // Ambient Light
	{
		XMFLOAT4 lightColor;
	};
	struct DLight //Directional Light
	{
		XMFLOAT4 lightColor;
		XMFLOAT4 lightDirection;
	};
	struct PLight //Point Light
	{
		XMFLOAT4 lightColor;
		XMFLOAT4 lightPosition;

	};
	struct SLight //Spot Light
	{
		XMFLOAT4 lightPosition;
		XMFLOAT4 lightColor;
		XMFLOAT4 coneDirAndRatio;
	};
}