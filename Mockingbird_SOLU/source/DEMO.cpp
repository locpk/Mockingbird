#include "DEMO.h"

#include "Cube.h"
#include "numbers_test.h"
#include "CSH/Projection_VS.csh"
#include "CSH/Trivial_PS.csh"
#include "CSH\Star_VS.csh"
#include "CSH\Star_PS.csh"
#include "vld.h"

//Helper Fuctions
MyVertex* CreateStar()
{
	//Create Star
	MyVertex* star = new MyVertex[12];
	star[0].pos.x = 0.0f;
	star[0].pos.y = 0.0f;
	star[0].pos.z = -0.1f;
	star[0].pos.w = 1.0f;
	star[0].color = { 0.0f,0.0f,1.0f,1.0f };
	star[11].pos.x = 0.0f;
	star[11].pos.y = 0.0f;
	star[11].pos.z = 0.1f;
	star[11].pos.w = 1.0f;
	star[11].color = { 0.0f,0.0f,1.0f,1.0f };
	size_t j = 1;
	for (size_t i = 0; i < 360; i += 36, j++)
	{
		if (j % 2 == 1)
		{
			star[j].pos.x = 0.5f*sinf(XMConvertToRadians((float)i));
			star[j].pos.y = 0.5f*cosf(XMConvertToRadians((float)i));
			star[j].pos.z = 0.0f;
			star[j].pos.w = 1.0f;
			star[j].color = { 0.0f,1.0f,0.0f,1.0f };
		}
		else
		{
			star[j].pos.x = 0.2f*sinf(XMConvertToRadians((float)i));
			star[j].pos.y = 0.2f*cosf(XMConvertToRadians((float)i));
			star[j].pos.z = 0.0f;
			star[j].pos.w = 1.0f;
			star[j].color = { 1.0f,0.0f,0.0f,1.0f };
		}

	}
	return	star;
}














DEMO* DEMO::s_pInstance = nullptr;

DEMO* DEMO::GetInstance(HINSTANCE hinst, WNDPROC proc)
{
	if (s_pInstance == nullptr)
		s_pInstance = new DEMO(hinst,proc);

	return s_pInstance;
}

DEMO::DEMO(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, (long)BACKBUFFER_WIDTH, (long)BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Mockingbird", WS_OVERLAPPEDWINDOW /*& ~(WS_THICKFRAME | WS_MAXIMIZEBOX)*/,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//


	//D3D Init
	DXGI_SWAP_CHAIN_DESC swapchain_DESC;
	ZeroMemory(&swapchain_DESC, sizeof(swapchain_DESC));
	swapchain_DESC.BufferCount = 1;
	swapchain_DESC.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchain_DESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchain_DESC.BufferDesc.Width = (UINT)BACKBUFFER_WIDTH;
	swapchain_DESC.BufferDesc.Height = (UINT)BACKBUFFER_HEIGHT;
	swapchain_DESC.BufferDesc.RefreshRate.Numerator = 60;
	swapchain_DESC.BufferDesc.RefreshRate.Denominator = 1;
	swapchain_DESC.OutputWindow = window;
	swapchain_DESC.SampleDesc.Count = 1;
	swapchain_DESC.SampleDesc.Quality = 0;
	swapchain_DESC.Windowed = TRUE;

	//Create Device and Swapchain DEBUG
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		&FeatureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapchain_DESC,
		&pSwapchain,
		&pDevice,
		&featureLevel,
		&pDeviceContext);


	//Create Render Target View from back buffer
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer, 0, &pRenderTargetView);
	SecureRelease(pBackBuffer);

	
	//Set up view port
	ZeroMemory(&viewport, sizeof(viewport));
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	viewport.Width = (float)swapchain_DESC.BufferDesc.Width;
	viewport.Height = (float)swapchain_DESC.BufferDesc.Height;

	//Set up Depth Buffer
	D3D11_TEXTURE2D_DESC ZBufferdesc;
	ZBufferdesc.Width = swapchain_DESC.BufferDesc.Width;
	ZBufferdesc.Height = swapchain_DESC.BufferDesc.Height;
	ZBufferdesc.MipLevels = 1;
	ZBufferdesc.ArraySize = 1;
	ZBufferdesc.Format = DXGI_FORMAT_D32_FLOAT;
	ZBufferdesc.SampleDesc.Count = 1;
	ZBufferdesc.SampleDesc.Quality = 0;
	ZBufferdesc.Usage = D3D11_USAGE_DEFAULT;
	ZBufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ZBufferdesc.CPUAccessFlags = 0;
	ZBufferdesc.MiscFlags = 0;
	pDevice->CreateTexture2D(&ZBufferdesc, 0, &pZBuffer);
	pDevice->CreateDepthStencilView(pZBuffer, NULL, &pDepthStencilView);
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

	//Load Star

	MyVertex* star = CreateStar();
	
	
	D3D11_BUFFER_DESC starBufferDesc;
	ZeroMemory(&starBufferDesc, sizeof(starBufferDesc));
	starBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	starBufferDesc.ByteWidth = sizeof(MyVertex) * 12;
	starBufferDesc.CPUAccessFlags = NULL;
	starBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	starBufferDesc.MiscFlags = 0;
	starBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA starVerticesData;
	ZeroMemory(&starVerticesData, sizeof(starVerticesData));
	starVerticesData.pSysMem = star;
	pDevice->CreateBuffer(&starBufferDesc, &starVerticesData, &pStar);

	delete star;
	//Create Star Shaders
	pDevice->CreateVertexShader(Star_VS, sizeof(Star_VS), NULL, &pStar_VSShader);
	pDevice->CreatePixelShader(Star_PS, sizeof(Star_PS), NULL, &pStar_PSShader);

	//Create Star InputLayout
	D3D11_INPUT_ELEMENT_DESC starInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	pDevice->CreateInputLayout(starInputLayout, 2, Star_VS, sizeof(Star_VS), &pStar_inputLayout);


	unsigned int index[60];
	for (size_t i = 0; i < 30; i++)
	{
		if (i % 3 == 0)
		{
			index[i] = 0;
		}
	}
	for (size_t i = 29; i < 60; i++)
	{
		if (i % 3 == 0)
		{
			index[i] = 11;
		}
	}
	index[1] = 1;
	index[2] = 2;
	index[4] = 2;
	index[5] = 3;
	index[7] = 3;
	index[8] = 4;
	index[10] = 4;
	index[11] = 5;
	index[13] = 5;
	index[14] = 6;
	index[16] = 6;
	index[17] = 7;
	index[19] = 7;
	index[20] = 8;
	index[22] = 8;
	index[23] = 9;
	index[25] = 9;
	index[26] = 10;
	index[28] = 10;
	index[29] = 1;

	index[31] = 1;
	index[32] = 10;
	index[34] = 10;
	index[35] = 9;
	index[37] = 9;
	index[38] = 8;
	index[40] = 8;
	index[41] = 7;
	index[43] = 7;
	index[44] = 6;
	index[46] = 6;
	index[47] = 5;
	index[49] = 5;
	index[50] = 4;
	index[52] = 4;
	index[53] = 3;
	index[55] = 3;
	index[56] = 2;
	index[58] = 2;
	index[59] = 1;


	//Create Star Index Buffer
	D3D11_BUFFER_DESC starIndexBufferDesc;
	ZeroMemory(&starIndexBufferDesc, sizeof(starIndexBufferDesc));
	starIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	starIndexBufferDesc.ByteWidth = sizeof(unsigned int) * 60;
	starIndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	starIndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	starIndexBufferDesc.MiscFlags = 0;
	starIndexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA starIndexBufferData;
	ZeroMemory(&starIndexBufferData, sizeof(starIndexBufferData));
	starIndexBufferData.pSysMem = index;
	pDevice->CreateBuffer(&starIndexBufferDesc, &starIndexBufferData, &pStar_indexBuffer);


	//Load Cube
	D3D11_BUFFER_DESC cubeBufferDesc;
	ZeroMemory(&cubeBufferDesc, sizeof(cubeBufferDesc));
	cubeBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	cubeBufferDesc.ByteWidth = sizeof(_OBJ_VERT_) * 776;
	cubeBufferDesc.CPUAccessFlags = NULL;
	cubeBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	cubeBufferDesc.MiscFlags = 0;
	cubeBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cubeVerticesData;
	ZeroMemory(&cubeVerticesData, sizeof(cubeVerticesData));
	cubeVerticesData.pSysMem = Cube_data;
	pDevice->CreateBuffer(&cubeBufferDesc, &cubeVerticesData, &pCube);


	//Create Cube Shaders
	pDevice->CreateVertexShader(Projection_VS, sizeof(Projection_VS), NULL, &pProjection_VSShader);
	pDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pCube_PSShader);

	//Create Cube InputLayout
	D3D11_INPUT_ELEMENT_DESC cubeInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	pDevice->CreateInputLayout(cubeInputLayout, 3, Projection_VS, sizeof(Projection_VS), &pCube_inputLayout);



	//Create Cube Index Buffer
	D3D11_BUFFER_DESC cubeIndexBufferDesc;
	ZeroMemory(&cubeIndexBufferDesc, sizeof(cubeIndexBufferDesc));
	cubeIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	cubeIndexBufferDesc.ByteWidth = sizeof(unsigned int) * 1692;
	cubeIndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cubeIndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cubeIndexBufferDesc.MiscFlags = 0;
	cubeIndexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA cubeIndexBufferData;
	ZeroMemory(&cubeIndexBufferData, sizeof(cubeIndexBufferData));
	cubeIndexBufferData.pSysMem = Cube_indicies;
	pDevice->CreateBuffer(&cubeIndexBufferDesc, &cubeIndexBufferData, &pCube_indexBuffer);


	//Create Constant buffer
	D3D11_BUFFER_DESC constbufferObjectDesc;
	ZeroMemory(&constbufferObjectDesc, sizeof(constbufferObjectDesc));
	constbufferObjectDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferObjectDesc.ByteWidth = sizeof(XMMATRIX);
	constbufferObjectDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferObjectDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferObjectDesc.MiscFlags = 0;
	constbufferObjectDesc.StructureByteStride = 0;
	pDevice->CreateBuffer(&constbufferObjectDesc, NULL, &pConstantObjectBuffer);

	D3D11_BUFFER_DESC constbufferStarDesc;
	ZeroMemory(&constbufferStarDesc, sizeof(constbufferStarDesc));
	constbufferStarDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferStarDesc.ByteWidth = sizeof(XMMATRIX);
	constbufferStarDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferStarDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferStarDesc.MiscFlags = 0;
	constbufferStarDesc.StructureByteStride = 0;
	pDevice->CreateBuffer(&constbufferStarDesc, NULL, &pConstantStarBuffer);

	D3D11_BUFFER_DESC constbufferSceneDesc;
	ZeroMemory(&constbufferSceneDesc, sizeof(constbufferSceneDesc));
	constbufferSceneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferSceneDesc.ByteWidth = sizeof(Scene);
	constbufferSceneDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferSceneDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferSceneDesc.MiscFlags = 0;
	constbufferSceneDesc.StructureByteStride = 0;
	pDevice->CreateBuffer(&constbufferSceneDesc, NULL, &pConstantSceneBuffer);

	//Create Cube texture
	D3D11_TEXTURE2D_DESC cubeTextureDESC;
	cubeTextureDESC.Width = numbers_test_width;
	cubeTextureDESC.Height = numbers_test_height;
	cubeTextureDESC.MipLevels = numbers_test_numlevels;
	cubeTextureDESC.ArraySize = 1;
	cubeTextureDESC.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	cubeTextureDESC.SampleDesc.Count = 1;
	cubeTextureDESC.SampleDesc.Quality = 0;
	cubeTextureDESC.Usage = D3D11_USAGE_DEFAULT;
	cubeTextureDESC.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cubeTextureDESC.CPUAccessFlags = 0;
	cubeTextureDESC.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA cubeTextureData[numbers_test_numlevels];
	for (size_t i = 0; i < numbers_test_numlevels; i++)
	{
		cubeTextureData[i].pSysMem = &numbers_test_pixels[numbers_test_leveloffsets[i]];
		cubeTextureData[i].SysMemPitch = (numbers_test_width >> i) * sizeof(unsigned int);
	}

	pDevice->CreateTexture2D(&cubeTextureDESC, cubeTextureData, &pCubeTexture);


	//Create Cube Texture Sampler
	D3D11_SAMPLER_DESC cubeTextureSamplerDESC;
	ZeroMemory(&cubeTextureSamplerDESC, sizeof(cubeTextureSamplerDESC));
	cubeTextureSamplerDESC.Filter = D3D11_FILTER_ANISOTROPIC;
	cubeTextureSamplerDESC.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeTextureSamplerDESC.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeTextureSamplerDESC.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeTextureSamplerDESC.ComparisonFunc = D3D11_COMPARISON_NEVER;
	cubeTextureSamplerDESC.MinLOD = 0;
	cubeTextureSamplerDESC.MaxLOD = numbers_test_numlevels;

	pDevice->CreateSamplerState(&cubeTextureSamplerDESC, &pCubeTextureSampler);

	//Create Cube Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC CubeSRVDesc;
	ZeroMemory(&CubeSRVDesc, sizeof(CubeSRVDesc));
	CubeSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	CubeSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	CubeSRVDesc.Texture2D.MipLevels = numbers_test_numlevels;
	pDevice->CreateShaderResourceView(pCubeTexture, NULL, &pCubeShaderResourceView);


	//Create Cube RasterState
	D3D11_RASTERIZER_DESC cubeRasterDESC;
	ZeroMemory(&cubeRasterDESC, sizeof(cubeRasterDESC));
	cubeRasterDESC.AntialiasedLineEnable = true;
	cubeRasterDESC.FrontCounterClockwise = true;
	cubeRasterDESC.FillMode = D3D11_FILL_SOLID;
	cubeRasterDESC.CullMode = D3D11_CULL_NONE;
	pDevice->CreateRasterizerState(&cubeRasterDESC, &pCubeRS);


	//Create Blend State
	D3D11_BLEND_DESC blendDESC;
	ZeroMemory(&blendDESC, sizeof(blendDESC));
	blendDESC.AlphaToCoverageEnable = true;
	blendDESC.RenderTarget[0].BlendEnable = true;
	blendDESC.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDESC.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDESC.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDESC.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDESC.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDESC.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDESC.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState(&blendDESC, &pBlendState);


	GetCursorPos(&lastPos);
	cube_matrix._translation = /*XMMatrixTranslation(0.0f, 2.0f, 0.0f) * */XMMatrixIdentity();
	star_matrix._translation = XMMatrixScaling(2.0f, 3.0f, 2.0f) * XMMatrixTranslation(0.0f, 2.0f, 0.0f) ;

}

DEMO::~DEMO()
{
	ShutDown();
}



bool DEMO::Run()
{
	xTime.Signal();
	static double timer = 0.0;
	timer += xTime.Delta();
	if (timer > ANIMATION_SPEED)
	{
		timer = 0;
		XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians(1.0f));
		XMMATRIX rotYN = XMMatrixRotationY(XMConvertToRadians(-1.0f));
		cube_matrix._translation = rotY * cube_matrix._translation;
		star_matrix._translation = rotYN * star_matrix._translation;
	}
	SHORT left, right, up, down, shift,w,a,s,d;
	left = GetAsyncKeyState(VK_LEFT) & 0x1;
	a = GetAsyncKeyState('A') & 0x1;
	right = GetAsyncKeyState(VK_RIGHT) & 0x1;
	d = GetAsyncKeyState('D') & 0x1;
	up = GetAsyncKeyState(VK_UP) & 0x1;
	w = GetAsyncKeyState('W') & 0x1;
	down = GetAsyncKeyState(VK_DOWN) & 0x1;
	s = GetAsyncKeyState('S') & 0x1;
	shift = GetAsyncKeyState(VK_SHIFT);
	if (left || a)
	{
		camera.Stafe(-(float)xTime.Delta() * 500);
	}

	if (right ||d)
	{
		camera.Stafe((float)xTime.Delta() * 500);
	}

	if (up||w)
	{
		camera.Walk((float)xTime.Delta() * 500);
	}

	if (down||s)
	{
		camera.Walk(-(float)xTime.Delta() * 500);
	}

	if (shift && up)
	{
		camera.Climb((float)xTime.Delta() * 500);
	}

	if (shift && down)
	{
		camera.Climb(-(float)xTime.Delta() * 500);
	}

	GetCursorPos(&CurPos);
	if (GetAsyncKeyState(VK_LBUTTON) && (lastPos.x != CurPos.x || lastPos.y != CurPos.y))
	{
		camera.Pitch(0.15f*(CurPos.y - lastPos.y));
		camera.RotateY(0.15f*(CurPos.x - lastPos.x));
		lastPos.x = CurPos.x;
		lastPos.y = CurPos.y;
	}
	


	//Clear background
	float clearColours[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColours);
	pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Setup Scene object
	scene._proj = camera.GetProj();
	scene._view = camera.GetView();

	D3D11_MAPPED_SUBRESOURCE mapObjectSubresource;
	pDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &cube_matrix, sizeof(cube_matrix));
	pDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	

	D3D11_MAPPED_SUBRESOURCE mapSceneSubresource;
	pDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(1, 1, &pConstantSceneBuffer);

	D3D11_MAPPED_SUBRESOURCE mapStarSubresource;
	pDeviceContext->Map(pConstantStarBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapStarSubresource);
	memcpy(mapStarSubresource.pData, &star_matrix, sizeof(star_matrix));
	pDeviceContext->Unmap(pConstantStarBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(2, 1, &pConstantStarBuffer);

	pDeviceContext->RSSetViewports(1, &viewport);


	//Star
	UINT offset = 0;
	UINT starStride = sizeof(MyVertex);
	pDeviceContext->RSSetState(NULL);
	pDeviceContext->IASetVertexBuffers(0, 1, &pStar, &starStride, &offset);
	pDeviceContext->VSSetShader(pStar_VSShader, NULL, 0);
	pDeviceContext->PSSetShader(pStar_PSShader, NULL, 0);
	pDeviceContext->IASetIndexBuffer(pStar_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetInputLayout(pStar_inputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->DrawIndexed(60, 0, 0);


	//Cube
	pDeviceContext->OMSetBlendState(pBlendState, NULL, 0xFFFFFFFF);
	pDeviceContext->PSSetShaderResources(0, 1, &pCubeShaderResourceView);
	pDeviceContext->PSSetSamplers(0, 1, &pCubeTextureSampler);
	UINT stride = sizeof(_OBJ_VERT_);
	
	pDeviceContext->IASetVertexBuffers(0, 1, &pCube, &stride, &offset);
	pDeviceContext->VSSetShader(pProjection_VSShader, NULL, 0);
	pDeviceContext->PSSetShader(pCube_PSShader, NULL, 0);
	pDeviceContext->IASetIndexBuffer(pCube_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetInputLayout(pCube_inputLayout);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeviceContext->RSSetState(pCubeRS);
	pDeviceContext->DrawIndexed(1692, 0, 0);


	

	pSwapchain->Present(0, 0);
	return true;
}


bool DEMO::ShutDown()
{
	SecureRelease(pCube);
	SecureRelease(pCube_inputLayout);
	SecureRelease(pCube_indexBuffer);
	SecureRelease(pCube_PSShader);
	SecureRelease(pProjection_VSShader);
	SecureRelease(pCubeTexture);
	SecureRelease(pCubeTextureSampler);
	SecureRelease(pCubeShaderResourceView);
	SecureRelease(pCubeRS);
	

	SecureRelease(pStar);
	SecureRelease(pStar_inputLayout);
	SecureRelease(pStar_indexBuffer);
	SecureRelease(pStar_VSShader);
	SecureRelease(pStar_PSShader);

	SecureRelease(pRenderTargetView);
	SecureRelease(pDepthStencilView);
	SecureRelease(pZBuffer);
	SecureRelease(pConstantObjectBuffer);
	SecureRelease(pConstantSceneBuffer);
	SecureRelease(pBlendState);

	SecureRelease(pDeviceContext);
	SecureRelease(pSwapchain);
	SecureRelease(pDevice);

	return true;
}


void DEMO::ResizeWindow(UINT _width, UINT _height)
{
	if (pSwapchain != nullptr)
	{

		pDeviceContext->OMSetRenderTargets(0, NULL, NULL);
		SecureRelease(pRenderTargetView);
		pDeviceContext->ClearState();
		pSwapchain->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
		ID3D11Texture2D* backBuffer;
		pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		pDevice->CreateRenderTargetView(backBuffer, 0, &pRenderTargetView);
		SecureRelease(backBuffer);
		SecureRelease(pDepthStencilView);
		SecureRelease(pZBuffer);
		D3D11_TEXTURE2D_DESC ZBufferdesc;
		ZBufferdesc.Width = _width;
		ZBufferdesc.Height = _height;
		ZBufferdesc.MipLevels = 1;
		ZBufferdesc.ArraySize = 1;
		ZBufferdesc.Format = DXGI_FORMAT_D32_FLOAT;
		ZBufferdesc.SampleDesc.Count = 1;
		ZBufferdesc.SampleDesc.Quality = 0;
		ZBufferdesc.Usage = D3D11_USAGE_DEFAULT;
		ZBufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ZBufferdesc.CPUAccessFlags = 0;
		ZBufferdesc.MiscFlags = 0;
		pDevice->CreateTexture2D(&ZBufferdesc, 0, &pZBuffer);
		pDevice->CreateDepthStencilView(pZBuffer, NULL, &pDepthStencilView);
		camera.UpdateProjection(60.0f, (float)_width, (float)_height, NEAR_PLANE, FAR_PLANE);
		scene._proj = camera.GetProj();
		ZeroMemory(&viewport, sizeof(viewport));
		viewport.MaxDepth = 1;
		viewport.MinDepth = 0;
		viewport.Width = (float)_width;
		viewport.Height = (float)_height;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pDeviceContext->RSSetViewports(1, &viewport);
		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	}
}