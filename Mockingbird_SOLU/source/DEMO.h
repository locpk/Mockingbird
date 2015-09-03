#pragma once
#include "Define.h"
#include "Camera.h"

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


struct VERTEX
{
	union
	{
		float xyzw[4];
		struct
		{
			float _x, _y, _z, _w;
		};
	};
	float _u;
	float _v;

};

struct Triangle
{
	VERTEX v[3];
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
	D3D11_VIEWPORT another_viewport;
	D3D11_VIEWPORT* current_viewport;
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
	ID3D11RasterizerState* pCubeRSf = nullptr;
	ID3D11RasterizerState* pCubeRSb = nullptr;

	Object star_matrix;
	ID3D11Buffer* pStar = nullptr;
	ID3D11InputLayout* pStar_inputLayout = nullptr;
	ID3D11Buffer* pStar_indexBuffer = nullptr;
	ID3D11VertexShader* pStar_VSShader = nullptr;
	ID3D11PixelShader* pStar_PSShader = nullptr;

	ID3D11Buffer* pConstantStarBuffer = nullptr;

	//Skybox
	ID3D11Resource* pSkyBoxTexture = nullptr;
	ID3D11ShaderResourceView* pSkyBoxSRV = nullptr;
	ID3D11InputLayout* pSkyBox_inputLayout = nullptr;
	Object skyCube_matrix;
	ID3D11Buffer* pSkyCube = nullptr;
	ID3D11VertexShader* pSkymap_VS = nullptr;
	ID3D11PixelShader* pSkymap_PS = nullptr;

	//Scene
	Scene scene;
	Camera camera;
	Camera another_camera;
	Camera* current_camera = nullptr;
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

