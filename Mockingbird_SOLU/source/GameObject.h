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
	ID3D11ShaderResourceView* pGO_ShaderResourceView = nullptr;
	ID3D11RasterizerState* pGORS = nullptr;
	ID3D11RasterizerState* pGORSf = nullptr;
	ID3D11RasterizerState* pGORSb = nullptr;
	unsigned int Stride = sizeof(VERTEX);
	string textureName;

	ID3D11Device* DEVICE = nullptr;
	GameObject();
	void CreateGameObject(ID3D11Device  * _device, string _filepath, const BYTE * _VS, unsigned int _VSize);
	~GameObject();
};

