#pragma once
#include "Common_Header.h"
#include "Define.h"
#include "XTime.h"
#include "DDSTextureLoader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Lighting.h"





struct Lights
{
	Lighting::AMLight amLight;
	Lighting::DLight dLight;
	Lighting::PLight pLight;
	Lighting::SLight sLight;
};

class DEMO
{

	//Windows
	static HINSTANCE					application;
	static WNDPROC						appWndProc;
	static HWND							window;

	bool resizing = false;

	POINT lastPos{ -1,-1 };
	POINT CurPos{ -1,-1 };

	ID3D11ShaderResourceView* nullSRV = nullptr;
	//D3D11 Interfaces Pointers
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapchain = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;
	ID3D11Texture2D* pZBuffer = nullptr;
	ID3D11Buffer* pConstantObjectBuffer = nullptr;
	ID3D11Buffer* pConstantSceneBuffer = nullptr;
	ID3D11Buffer* pConstantTimeBuffer = nullptr;
	ID3D11BlendState* pBlendState = nullptr;
	ID3D11VertexShader* pProjection_VSShader = nullptr;
	//D3D11 Stack Variables
	D3D11_VIEWPORT viewport;
	D3D11_VIEWPORT another_viewport;
	D3D_FEATURE_LEVEL featureLevel;


	//Game Objects
	ID3D11ShaderResourceView* pGroundNormalMap = nullptr;
	ID3D11ShaderResourceView* pSkyboxSunset = nullptr;

	XMMATRIX cube_matrix;
	ID3D11Buffer* pCube = nullptr;
	ID3D11InputLayout* pCube_inputLayout = nullptr;
	ID3D11Buffer* pCube_indexBuffer = nullptr;
	ID3D11PixelShader* pCube_PSShader = nullptr;
	ID3D11SamplerState* pCubeTextureSampler = nullptr;
	ID3D11ShaderResourceView* pCubeShaderResourceView = nullptr;
	ID3D11RasterizerState* pCubeRSf = nullptr;
	ID3D11RasterizerState* pCubeRSb = nullptr;
	ID3D11Buffer* pCubeInstanceBuffer = nullptr;
	vector<XMFLOAT4X4> cubeInstancedData;

	XMMATRIX star_matrix;
	ID3D11Buffer* pStar = nullptr;
	ID3D11InputLayout* pStar_inputLayout = nullptr;
	ID3D11Buffer* pStar_indexBuffer = nullptr;
	ID3D11VertexShader* pStar_VSShader = nullptr;
	ID3D11PixelShader* pStar_PSShader = nullptr;
	ID3D11Buffer* pConstantStarBuffer = nullptr;


	GameObject heli;
	ID3D11VertexShader* pHeli_VSShader = nullptr;
	ID3D11PixelShader* pHeli_PSShader = nullptr;
	GameObject ground;
	GameObject parkLight;

	GameObject skybox;
	ID3D11VertexShader* pskybox_VSShader = nullptr;
	ID3D11PixelShader* pskybox_PSShader = nullptr;


	//Quad
	XMMATRIX quad_worldMatrix;
	XMFLOAT4 quad_pos = { 0.0f,4.0f,0.0f,0.0f };
	D3D11_SHADER_RESOURCE_VIEW_DESC quadDesc;
	ID3D11RenderTargetView* pQuadRTV = nullptr;
	ID3D11Texture2D* pQuad_texture = nullptr;
	ID3D11Buffer* pQuadBuffer = nullptr;
	ID3D11InputLayout* pQuadInputLayout = nullptr;
	ID3D11VertexShader* pQuad_VSShader = nullptr;
	ID3D11GeometryShader* pQuad_GSShader = nullptr;
	ID3D11PixelShader* pQuad_PSShader = nullptr;
	ID3D11ShaderResourceView* pQuadSRV = nullptr;
	ID3D11Buffer* pConstantQuadBuffer = nullptr;
	//Scene
	Scene scene;
	Camera camera;
	Camera another_camera;
	Camera* current_camera = nullptr;
	XTime xTime;
	Lights allLights;
	ID3D11Buffer* pLightingBuffer = nullptr;
	XMFLOAT4 DLSpos = { 0.0f,0.0f,30.0f,1.0f };
	XMFLOAT4 DLEpos = { 0.0f,0.0f,-30.0f,1.0f };


	static DEMO* s_pInstance;

	DEMO(HINSTANCE hinst, WNDPROC proc);
	~DEMO();
	DEMO(const DEMO&) = delete;
	DEMO& operator=(const DEMO&) = delete;



public:
	ID3D11DeviceContext* pDeferredDeviceContext = nullptr;
	ID3D11CommandList* pCommandList = nullptr;
	static DEMO* GetInstance(HINSTANCE hinst, WNDPROC proc);

	void ResizeWindow(UINT _width, UINT _height);
	bool Run();
	bool ShutDown();

	void Draw();


	void Load();

};

