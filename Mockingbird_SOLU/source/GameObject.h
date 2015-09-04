#pragma once
#include "Common_Header.h"

class GameObject
{
public:
	vector<VERTEX> GOrawData;
	XMMATRIX GO_worldMatrix;
	ID3D11Buffer* pGOvertices = nullptr;
	ID3D11InputLayout* pGO_inputLayout = nullptr;
	ID3D11PixelShader* pGO_PSShader = nullptr;
	ID3D11VertexShader* pGO_VSShader = nullptr;
	ID3D11Texture2D* pGO_Texture = nullptr;
	ID3D11SamplerState* pGO_TextureSampler = nullptr;
	ID3D11ShaderResourceView* pGO_ShaderResourceView = nullptr;
	ID3D11RasterizerState* pGORSf = nullptr;
	ID3D11RasterizerState* pGORSb = nullptr;

	ID3D11Device* DEVICE = nullptr;
	GameObject(ID3D11Device  * _device, const BYTE* _VS);
	~GameObject();
};

