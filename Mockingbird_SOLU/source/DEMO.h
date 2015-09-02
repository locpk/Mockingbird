#pragma once
#include "Define.h"
#include "Cube.h"
#include "numbers_test.h"
#include "CSH/Projection_VS.csh"
#include "CSH/Trivial_PS.csh"
#include "Camera.h"

struct Object
{
	XMMATRIX _translation;
};

struct Scene
{
	float index;
	float pad1;
	float pad2;
	float pad3;
	XMMATRIX _view;
	XMMATRIX _proj;
};


class DEMO
{
	//Windows
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;

	POINT lastPos{ -1,-1 };
	POINT CurPos{ -1,-1 };

	//D3D11 Interfaces Pointers
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapchain = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;
	ID3D11Texture2D* pZBuffer = nullptr;
	ID3D11Buffer* pConstantObjectBuffer = nullptr;
	ID3D11Buffer* pConstantSceneBuffer = nullptr;
	ID3D11BlendState* pBlendState = nullptr;
	ID3D11VertexShader* pProjection_VSShader = nullptr;
	//D3D11 Stack Variables
	D3D11_VIEWPORT viewport;
	D3D_FEATURE_LEVEL featureLevel;


	//Game Objects
	Object cube_matrix;
	ID3D11Buffer* pCube = nullptr;
	ID3D11InputLayout* pCube_inputLayout = nullptr;
	ID3D11Buffer* pCube_indexBuffer = nullptr;
	ID3D11PixelShader* pCube_PSShader = nullptr;
	ID3D11Texture2D* pCubeTexture = nullptr;
	ID3D11SamplerState* pCubeTextureSampler = nullptr;
	ID3D11ShaderResourceView* pCubeShaderResourceView = nullptr;
	ID3D11RasterizerState* pCubeRS = nullptr;




	//Scene
	Scene scene;
	Camera camera;

	XTime xTime;

	static DEMO* s_pInstance;

	DEMO(HINSTANCE hinst, WNDPROC proc);
	~DEMO();
	DEMO(const DEMO&) = delete;	
	DEMO& operator=(const DEMO&) = delete;	
public:
	static DEMO* GetInstance(HINSTANCE hinst, WNDPROC proc);
	
	void ResizeWindow(UINT _width, UINT _height);
	bool Run();
	bool ShutDown();

};

