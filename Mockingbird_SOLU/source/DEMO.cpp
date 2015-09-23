#include "DEMO.h"
#include "Cube.h"
#include "numbers_test.h"
#include "CSH\Projection_VS.csh"
#include "CSH\Trivial_PS.csh"
#include "CSH\Star_VS.csh"
#include "CSH\Star_PS.csh"
#include "CSH\Cube_VS.csh"
#include "CSH\Cube_PS.csh"
#include "CSH\SkyBox_VS.csh"
#include "CSH\SkyBox_PS.csh"
#include "CSH\Quad_VS.csh"
#include "CSH\Quad_GS.csh"
#include "CSH\Quad_PS.csh"

#define MSAA 1
#define MSAA_COUNT 8
//BEGIN Helper Fuctions
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

float ObjectToCamera(XMFLOAT4X4* _objMatrix, XMFLOAT3 _cameraPos)
{
	XMVECTOR obj = XMVectorZero();
	obj = XMVector3Transform(obj, XMLoadFloat4x4(_objMatrix));
	float ObjtoCameraX = XMVectorGetX(obj) - _cameraPos.x;
	float ObjtoCameraY = XMVectorGetY(obj) - _cameraPos.y;
	float ObjtoCameraZ = XMVectorGetZ(obj) - _cameraPos.z;
	return ObjtoCameraX*ObjtoCameraX + ObjtoCameraY*ObjtoCameraY + ObjtoCameraZ*ObjtoCameraZ;
}
//END Helper Fuctions




void DEMO::Draw()
{


	//1st Viewport
	pDeferredDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	pDeferredDeviceContext->RSSetViewports(1, &viewport);
	//Clear background
	float clearColours[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	pDeferredDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColours);
	pDeferredDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeferredDeviceContext->PSSetSamplers(0, 1, &pCubeTextureSampler);



	//Map Light Constant Buffer
	allLights.sLight.lightPosition = { camera.GetPosition().x,camera.GetPosition().y,camera.GetPosition().z,1.0f };
	allLights.sLight.coneDirAndRatio = { camera.GetForward().x,camera.GetForward().y,camera.GetForward().z,0.8f };
	D3D11_MAPPED_SUBRESOURCE mapLightingSubresource;
	pDeferredDeviceContext->Map(pLightingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapLightingSubresource);
	memcpy(mapLightingSubresource.pData, &allLights, sizeof(allLights));
	pDeferredDeviceContext->Unmap(pLightingBuffer, 0);
	pDeferredDeviceContext->PSSetConstantBuffers(2, 1, &pLightingBuffer);


	//Map Time Constant buffer
	float totalTime[4];
	totalTime[0] = (float)xTime.TotalTime();
	D3D11_MAPPED_SUBRESOURCE mapTimeSubresource;
	ZeroMemory(&mapTimeSubresource, sizeof(mapTimeSubresource));
	pDeferredDeviceContext->Map(pConstantTimeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapTimeSubresource);
	memcpy(mapTimeSubresource.pData, totalTime, sizeof(float)*4);
	pDeferredDeviceContext->Unmap(pConstantTimeBuffer, 0);



	scene._cameraPos = camera.GetPosition();

	D3D11_MAPPED_SUBRESOURCE mapSceneSubresource;
	ZeroMemory(&mapSceneSubresource, sizeof(mapSceneSubresource));
	pDeferredDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeferredDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(1, 1, &pConstantSceneBuffer);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantSceneBuffer);


	UINT offset = 0;
	//Heli
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &heli.pGOvertices, &heli.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(heli.pGO_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11_MAPPED_SUBRESOURCE mapObjectSubresource;
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &heli.GO_worldMatrix, sizeof(heli.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	pDeferredDeviceContext->VSSetShader(pHeli_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pHeli_PSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShaderResources(0, 1, &heli.pGO_ShaderResourceView);
	pDeferredDeviceContext->RSSetState(heli.pGORS);
	pDeferredDeviceContext->Draw((UINT)heli.GOrawData.size(), 0);


	//ground
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &ground.GO_worldMatrix, sizeof(ground.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	scene.hasNormal = true;
	ZeroMemory(&mapSceneSubresource, sizeof(mapSceneSubresource));
	pDeferredDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeferredDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantSceneBuffer);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &ground.pGOvertices, &ground.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(ground.pGO_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeferredDeviceContext->VSSetShader(pHeli_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pHeli_PSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShaderResources(0, 1, &ground.pGO_ShaderResourceView);
	pDeferredDeviceContext->PSSetShaderResources(1, 1, &pGroundNormalMap);
	pDeferredDeviceContext->RSSetState(ground.pGORS);
	pDeferredDeviceContext->Draw((UINT)ground.GOrawData.size(), 0);
	pDeferredDeviceContext->PSSetShaderResources(1, 1, &nullSRV);

	scene.hasNormal = false;
	ZeroMemory(&mapSceneSubresource, sizeof(mapSceneSubresource));
	pDeferredDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeferredDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantSceneBuffer);


	//Parklight
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &parkLight.GO_worldMatrix, sizeof(parkLight.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &parkLight.GO_worldMatrix, sizeof(parkLight.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	pDeferredDeviceContext->PSSetShaderResources(0, 1, &parkLight.pGO_ShaderResourceView);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &parkLight.pGOvertices, &parkLight.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(parkLight.pGO_inputLayout);
	pDeferredDeviceContext->VSSetShader(pHeli_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pHeli_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeferredDeviceContext->RSSetState(parkLight.pGORS);
	pDeferredDeviceContext->Draw((UINT)parkLight.GOrawData.size(), 0);



	//Skybox
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &skybox.GO_worldMatrix, sizeof(skybox.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	ID3D11ShaderResourceView* skyboxes[2];
	skyboxes[0] = skybox.pGO_ShaderResourceView;
	skyboxes[1] = pSkyboxSunset;
	pDeferredDeviceContext->PSSetShaderResources(0, 2, skyboxes);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &skybox.pGOvertices, &skybox.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(skybox.pGO_inputLayout);
	pDeferredDeviceContext->VSSetShader(pskybox_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pskybox_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeferredDeviceContext->RSSetState(skybox.pGORSf);
	pDeferredDeviceContext->Draw((UINT)skybox.GOrawData.size(), 0);
	pDeferredDeviceContext->PSSetShaderResources(1, 1, &nullSRV);




	//Star
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &star_matrix, sizeof(star_matrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	UINT starStride = sizeof(MyVertex);
	pDeferredDeviceContext->RSSetState(NULL);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &pStar, &starStride, &offset);
	pDeferredDeviceContext->VSSetShader(pStar_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pStar_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetIndexBuffer(pStar_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeferredDeviceContext->IASetInputLayout(pStar_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeferredDeviceContext->DrawIndexed(60, 0, 0);




	//Cube
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &cube_matrix, sizeof(cube_matrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	pDeferredDeviceContext->OMSetBlendState(pBlendState, NULL, 0xFFFFFFFF);
	pDeferredDeviceContext->PSSetShaderResources(0, 1, &pCubeShaderResourceView);
	UINT cubestride[2] = { sizeof(_OBJ_VERT_),sizeof(XMFLOAT4X4) };
	UINT cubeOffset[2] = { 0,0 };
	ID3D11Buffer* cubeBufferandCubeInstanceBuffer[2] = { pCube, pCubeInstanceBuffer };

	pDeferredDeviceContext->IASetVertexBuffers(0, 2, cubeBufferandCubeInstanceBuffer, cubestride, cubeOffset);
	pDeferredDeviceContext->VSSetShader(pProjection_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pCube_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetIndexBuffer(pCube_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeferredDeviceContext->IASetInputLayout(pCube_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	pDeferredDeviceContext->RSSetState(pCubeRSf);
	pDeferredDeviceContext->DrawIndexedInstanced(1692, (UINT)cubeInstancedData.size(), 0, 0, 0);
	pDeferredDeviceContext->RSSetState(pCubeRSb);
	pDeferredDeviceContext->DrawIndexedInstanced(1692, (UINT)cubeInstancedData.size(), 0, 0, 0);






	//Second Viewport

	scene._proj = another_camera.GetProj();
	scene._view = another_camera.GetView();
	scene._cameraPos = another_camera.GetPosition();
	pDeferredDeviceContext->RSSetViewports(1, &another_viewport);
	skybox.GO_worldMatrix = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(another_camera.GetPosition().x, another_camera.GetPosition().y, another_camera.GetPosition().z);


	pDeferredDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeferredDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(1, 1, &pConstantSceneBuffer);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantSceneBuffer);



	//Heli
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &heli.GO_worldMatrix, sizeof(heli.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &heli.pGOvertices, &heli.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(heli.pGO_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeferredDeviceContext->VSSetShader(pHeli_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pHeli_PSShader, NULL, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	pDeferredDeviceContext->PSSetShaderResources(0, 1, &heli.pGO_ShaderResourceView);
	pDeferredDeviceContext->RSSetState(heli.pGORS);
	pDeferredDeviceContext->Draw((UINT)heli.GOrawData.size(), 0);
	pDeferredDeviceContext->PSSetShaderResources(1, 1, &nullSRV);



	//ground
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &ground.GO_worldMatrix, sizeof(ground.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	scene.hasNormal = true;
	ZeroMemory(&mapSceneSubresource, sizeof(mapSceneSubresource));
	pDeferredDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeferredDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantSceneBuffer);


	pDeferredDeviceContext->PSSetShaderResources(0, 1, &ground.pGO_ShaderResourceView);
	pDeferredDeviceContext->PSSetShaderResources(1, 1, &pGroundNormalMap);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &ground.pGOvertices, &ground.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(ground.pGO_inputLayout);
	pDeferredDeviceContext->VSSetShader(pHeli_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pHeli_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeferredDeviceContext->RSSetState(ground.pGORS);
	pDeferredDeviceContext->Draw((UINT)ground.GOrawData.size(), 0);
	pDeferredDeviceContext->PSSetShaderResources(1, 1, &nullSRV);

	scene.hasNormal = false;
	ZeroMemory(&mapSceneSubresource, sizeof(mapSceneSubresource));
	pDeferredDeviceContext->Map(pConstantSceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &scene, sizeof(scene));
	pDeferredDeviceContext->Unmap(pConstantSceneBuffer, 0);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantSceneBuffer);


	//Parklight
	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &parkLight.GO_worldMatrix, sizeof(parkLight.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	pDeferredDeviceContext->PSSetShaderResources(0, 1, &parkLight.pGO_ShaderResourceView);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &parkLight.pGOvertices, &parkLight.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(parkLight.pGO_inputLayout);
	pDeferredDeviceContext->VSSetShader(pHeli_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pHeli_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeferredDeviceContext->RSSetState(parkLight.pGORS);
	pDeferredDeviceContext->Draw((UINT)parkLight.GOrawData.size(), 0);

	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &star_matrix, sizeof(star_matrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	//Star
	pDeferredDeviceContext->RSSetState(NULL);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &pStar, &starStride, &offset);
	pDeferredDeviceContext->VSSetShader(pStar_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pStar_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetIndexBuffer(pStar_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeferredDeviceContext->IASetInputLayout(pStar_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeferredDeviceContext->DrawIndexed(60, 0, 0);



	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &cube_matrix, sizeof(cube_matrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	//Cube
	pDeferredDeviceContext->OMSetBlendState(pBlendState, NULL, 0xFFFFFFFF);
	pDeferredDeviceContext->PSSetShaderResources(0, 1, &pCubeShaderResourceView);

	pDeferredDeviceContext->IASetVertexBuffers(0, 2, cubeBufferandCubeInstanceBuffer, cubestride, cubeOffset);
	pDeferredDeviceContext->VSSetShader(pProjection_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pCube_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetIndexBuffer(pCube_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeferredDeviceContext->IASetInputLayout(pCube_inputLayout);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	pDeferredDeviceContext->RSSetState(pCubeRSf);
	pDeferredDeviceContext->DrawIndexedInstanced(1692, (UINT)cubeInstancedData.size(), 0, 0, 0);
	pDeferredDeviceContext->RSSetState(pCubeRSb);
	pDeferredDeviceContext->DrawIndexedInstanced(1692, (UINT)cubeInstancedData.size(), 0, 0, 0);

	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &skybox.GO_worldMatrix, sizeof(skybox.GO_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);

	pDeferredDeviceContext->PSSetShaderResources(0, 2, skyboxes);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &skybox.pGOvertices, &skybox.Stride, &offset);
	pDeferredDeviceContext->IASetInputLayout(skybox.pGO_inputLayout);
	pDeferredDeviceContext->VSSetShader(pskybox_VSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pskybox_PSShader, NULL, 0);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeferredDeviceContext->RSSetState(skybox.pGORSf);
	pDeferredDeviceContext->Draw((UINT)skybox.GOrawData.size(), 0);



	D3D11_VIEWPORT viewports[2];
	viewports[0] = viewport;
	viewports[1] = another_viewport;
	pDeferredDeviceContext->RSSetViewports(2, viewports);

	XMMATRIX view[2];
	view[0] = camera.GetView();
	view[1] = another_camera.GetView();
	pDeferredDeviceContext->Map(pConstantQuadBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSceneSubresource);
	memcpy(mapSceneSubresource.pData, &view, sizeof(XMMATRIX) * 2);
	pDeferredDeviceContext->Unmap(pConstantQuadBuffer, 0);
	pDeferredDeviceContext->GSSetConstantBuffers(1, 1, &pConstantQuadBuffer);
	//Quad
	ID3D11Texture2D* pBackBuffer = nullptr;
	UINT Quadstride = sizeof(XMFLOAT4);
	UINT QuadOffset = 0;


	pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	pDeferredDeviceContext->ResolveSubresource(pQuad_texture, 0, pBackBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
	SecureRelease(pBackBuffer);




	ZeroMemory(&mapObjectSubresource, sizeof(mapObjectSubresource));
	pDeferredDeviceContext->Map(pConstantObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapObjectSubresource);
	memcpy(mapObjectSubresource.pData, &quad_worldMatrix, sizeof(quad_worldMatrix));
	pDeferredDeviceContext->Unmap(pConstantObjectBuffer, 0);
	pDeferredDeviceContext->VSSetConstantBuffers(0, 1, &pConstantObjectBuffer);
	pDeferredDeviceContext->GSSetConstantBuffers(0, 1, &pConstantSceneBuffer);
	pDeferredDeviceContext->PSSetConstantBuffers(0, 1, &pConstantTimeBuffer);
	pDeferredDeviceContext->OMSetBlendState(pBlendState, NULL, 0xFFFFFFFF);

	pDeferredDeviceContext->PSSetShaderResources(0, 1, &pQuadSRV);
	pDeferredDeviceContext->IASetVertexBuffers(0, 1, &pQuadBuffer, &Quadstride, &QuadOffset);
	pDeferredDeviceContext->VSSetShader(pQuad_VSShader, NULL, 0);
	pDeferredDeviceContext->GSSetShader(pQuad_GSShader, NULL, 0);
	pDeferredDeviceContext->PSSetShader(pQuad_PSShader, NULL, 0);
	pDeferredDeviceContext->PSSetSamplers(0, 1, &pCubeTextureSampler);
	pDeferredDeviceContext->IASetInputLayout(pQuadInputLayout);
	pDeferredDeviceContext->RSSetState(NULL);
	pDeferredDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pDeferredDeviceContext->Draw(1, 0);
	pDeferredDeviceContext->GSSetShader(NULL, NULL, 0);
	pDeferredDeviceContext->PSSetShaderResources(0, 1, &nullSRV);




	//Finishing Command
	pDeferredDeviceContext->FinishCommandList(false, &pCommandList);
}


void  DEMO::Load()
{
	heli.CreateGameObject(pDevice, "asset/heli.obj", Cube_VS, sizeof(Cube_VS));
	skybox.CreateGameObject(pDevice, "asset/skybox.obj", SkyBox_VS, sizeof(SkyBox_VS));
	ground.CreateGameObject(pDevice, "asset/Ground.obj", Cube_VS, sizeof(Cube_VS));
	parkLight.CreateGameObject(pDevice, "asset/ParkLight.obj", Cube_VS, sizeof(Cube_VS));
	CreateDDSTextureFromFile(pDevice, L"asset/Ground_norm.dds", NULL, &pGroundNormalMap);
	CreateDDSTextureFromFile(pDevice, L"asset/numbers_test1.dds", NULL, &pCubeShaderResourceView);
	CreateDDSTextureFromFile(pDevice, L"asset/SunsetSkybox.dds", NULL, &pSkyboxSunset);
}




HINSTANCE	DEMO::application;
WNDPROC		DEMO::appWndProc;
HWND		DEMO::window;
DEMO* DEMO::s_pInstance = nullptr;

DEMO* DEMO::GetInstance(HINSTANCE hinst, WNDPROC proc)
{
	if (s_pInstance == nullptr)
		s_pInstance = new DEMO(hinst, proc);

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
	swapchain_DESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapchain_DESC.BufferDesc.Width = (UINT)BACKBUFFER_WIDTH;
	swapchain_DESC.BufferDesc.Height = (UINT)BACKBUFFER_HEIGHT;
	swapchain_DESC.BufferDesc.RefreshRate.Numerator = 60;
	swapchain_DESC.BufferDesc.RefreshRate.Denominator = 1;
	swapchain_DESC.OutputWindow = window;
#if MSAA
	swapchain_DESC.SampleDesc.Count = MSAA_COUNT;
#else
	swapchain_DESC.SampleDesc.Count = 1;
#endif
	swapchain_DESC.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
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



	pDevice->CreateDeferredContext(0, &pDeferredDeviceContext);
	//Create Render Target View from back buffer
	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(pBackBuffer, 0, &pRenderTargetView);
	SecureRelease(pBackBuffer);



	D3D11_TEXTURE2D_DESC renderToTextureDesc;
	ZeroMemory(&renderToTextureDesc, sizeof(renderToTextureDesc));
	renderToTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderToTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderToTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderToTextureDesc.Height = swapchain_DESC.BufferDesc.Height;
	renderToTextureDesc.Width = swapchain_DESC.BufferDesc.Width;
	renderToTextureDesc.SampleDesc.Count = 1;
	renderToTextureDesc.ArraySize = 1;
	renderToTextureDesc.MipLevels = 1;
	pDevice->CreateTexture2D(&renderToTextureDesc, NULL, &pQuad_texture);
	pDevice->CreateRenderTargetView(pQuad_texture, 0, &pQuadRTV);
	quadDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	quadDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	quadDesc.Texture2D.MipLevels = 1;
	quadDesc.Texture2D.MostDetailedMip = 0;
	pDevice->CreateShaderResourceView(pQuad_texture, &quadDesc, &pQuadSRV);





	//Set up view port
	ZeroMemory(&viewport, sizeof(viewport));
	viewport.MaxDepth = 1;
	viewport.MinDepth = 0;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)swapchain_DESC.BufferDesc.Width / 2.0f;
	viewport.Height = (float)swapchain_DESC.BufferDesc.Height;

	ZeroMemory(&another_viewport, sizeof(another_viewport));
	another_viewport.MaxDepth = 1;
	another_viewport.MinDepth = 0;
	another_viewport.TopLeftX = (float)swapchain_DESC.BufferDesc.Width / 2.0f;
	another_viewport.TopLeftY = 0;
	another_viewport.Width = (float)swapchain_DESC.BufferDesc.Width / 2.0f;
	another_viewport.Height = (float)swapchain_DESC.BufferDesc.Height;




	//Set up Depth Buffer
	D3D11_TEXTURE2D_DESC ZBufferdesc;
	ZBufferdesc.Width = swapchain_DESC.BufferDesc.Width;
	ZBufferdesc.Height = swapchain_DESC.BufferDesc.Height;
	ZBufferdesc.MipLevels = 1;
	ZBufferdesc.ArraySize = 1;
	ZBufferdesc.Format = DXGI_FORMAT_D32_FLOAT;
	ZBufferdesc.SampleDesc.Count = swapchain_DESC.SampleDesc.Count;
	ZBufferdesc.SampleDesc.Quality = swapchain_DESC.SampleDesc.Quality;
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

#pragma region starIndex 
	unsigned int starIndex[60];
	for (size_t i = 0; i < 30; i++)
	{
		if (i % 3 == 0)
		{
			starIndex[i] = 0;
		}
	}
	for (size_t i = 29; i < 60; i++)
	{
		if (i % 3 == 0)
		{
			starIndex[i] = 11;
		}
	}
	starIndex[1] = 1;
	starIndex[2] = 2;
	starIndex[4] = 2;
	starIndex[5] = 3;
	starIndex[7] = 3;
	starIndex[8] = 4;
	starIndex[10] = 4;
	starIndex[11] = 5;
	starIndex[13] = 5;
	starIndex[14] = 6;
	starIndex[16] = 6;
	starIndex[17] = 7;
	starIndex[19] = 7;
	starIndex[20] = 8;
	starIndex[22] = 8;
	starIndex[23] = 9;
	starIndex[25] = 9;
	starIndex[26] = 10;
	starIndex[28] = 10;
	starIndex[29] = 1;

	starIndex[31] = 1;
	starIndex[32] = 10;
	starIndex[34] = 10;
	starIndex[35] = 9;
	starIndex[37] = 9;
	starIndex[38] = 8;
	starIndex[40] = 8;
	starIndex[41] = 7;
	starIndex[43] = 7;
	starIndex[44] = 6;
	starIndex[46] = 6;
	starIndex[47] = 5;
	starIndex[49] = 5;
	starIndex[50] = 4;
	starIndex[52] = 4;
	starIndex[53] = 3;
	starIndex[55] = 3;
	starIndex[56] = 2;
	starIndex[58] = 2;
	starIndex[59] = 1;

#pragma endregion
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
	starIndexBufferData.pSysMem = starIndex;
	pDevice->CreateBuffer(&starIndexBufferDesc, &starIndexBufferData, &pStar_indexBuffer);

	//Heli

	//Create Heli Shaders
	pDevice->CreateVertexShader(Cube_VS, sizeof(Cube_VS), NULL, &pHeli_VSShader);
	pDevice->CreatePixelShader(Cube_PS, sizeof(Cube_PS), NULL, &pHeli_PSShader);
	pDevice->CreateVertexShader(SkyBox_VS, sizeof(SkyBox_VS), NULL, &pskybox_VSShader);
	pDevice->CreatePixelShader(SkyBox_PS, sizeof(SkyBox_PS), NULL, &pskybox_PSShader);


	thread load = thread(&DEMO::Load, this);
	load.join();


	//Load Quad
	quad_worldMatrix = XMMatrixIdentity();
	D3D11_BUFFER_DESC quadBufferDesc;
	ZeroMemory(&quadBufferDesc, sizeof(quadBufferDesc));
	quadBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	quadBufferDesc.ByteWidth = sizeof(XMFLOAT4);
	quadBufferDesc.CPUAccessFlags = NULL;
	quadBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	quadBufferDesc.MiscFlags = 0;
	quadBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA quadData;
	ZeroMemory(&quadData, sizeof(quadData));
	quadData.pSysMem = &quad_pos;
	pDevice->CreateBuffer(&quadBufferDesc, &quadData, &pQuadBuffer);
	pDevice->CreateVertexShader(Quad_VS, sizeof(Quad_VS), NULL, &pQuad_VSShader);
	pDevice->CreateGeometryShader(Quad_GS, sizeof(Quad_GS), NULL, &pQuad_GSShader);
	pDevice->CreatePixelShader(Quad_PS, sizeof(Quad_PS), NULL, &pQuad_PSShader);

	D3D11_INPUT_ELEMENT_DESC quadInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	pDevice->CreateInputLayout(quadInputLayout, 1, Quad_VS, sizeof(Quad_VS), &pQuadInputLayout);


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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	pDevice->CreateInputLayout(cubeInputLayout, 7, Projection_VS, sizeof(Projection_VS), &pCube_inputLayout);



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

	//Instancing Cube

	XMFLOAT4X4 temp;
	/*for (size_t i = 0; i < 3; i++)
	{
		XMStoreFloat4x4(&temp, XMMatrixTranslation( 1.5f*cosf(i), 1.0f, 1.5f*sinf(i)));
		cubeInstancedData.push_back(temp);
	}*/

	XMStoreFloat4x4(&temp, XMMatrixTranslation(2.0f, 1.0f, 2.0f));
	cubeInstancedData.push_back(temp);
	XMStoreFloat4x4(&temp, XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	cubeInstancedData.push_back(temp);
	XMStoreFloat4x4(&temp, XMMatrixTranslation(-2.0f, 1.0f, -2.0f));
	cubeInstancedData.push_back(temp);

	D3D11_BUFFER_DESC cubeInstanceBufferDESC;
	cubeInstanceBufferDESC.Usage = D3D11_USAGE_DYNAMIC;
	cubeInstanceBufferDESC.ByteWidth = sizeof(XMFLOAT4X4) * (unsigned int)cubeInstancedData.size();
	cubeInstanceBufferDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	cubeInstanceBufferDESC.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cubeInstanceBufferDESC.MiscFlags = 0;
	cubeInstanceBufferDESC.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA instData;
	ZeroMemory(&instData, sizeof(instData));
	instData.pSysMem = cubeInstancedData.data();
	pDevice->CreateBuffer(&cubeInstanceBufferDESC, &instData, &pCubeInstanceBuffer);


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

	D3D11_BUFFER_DESC constbufferQuadDesc;
	ZeroMemory(&constbufferQuadDesc, sizeof(constbufferQuadDesc));
	constbufferQuadDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferQuadDesc.ByteWidth = sizeof(Scene);
	constbufferQuadDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferQuadDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferQuadDesc.MiscFlags = 0;
	constbufferQuadDesc.StructureByteStride = 0;
	pDevice->CreateBuffer(&constbufferQuadDesc, NULL, &pConstantQuadBuffer);


	D3D11_BUFFER_DESC constbufferTimeDesc;
	ZeroMemory(&constbufferTimeDesc, sizeof(constbufferTimeDesc));
	constbufferTimeDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferTimeDesc.ByteWidth = sizeof(float) * 4;
	constbufferTimeDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferTimeDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferTimeDesc.MiscFlags = 0;
	constbufferTimeDesc.StructureByteStride = 0;
	pDevice->CreateBuffer(&constbufferTimeDesc, NULL, &pConstantTimeBuffer);



	//Create Cube Texture Sampler
	D3D11_SAMPLER_DESC cubeTextureSamplerDESC;
	ZeroMemory(&cubeTextureSamplerDESC, sizeof(cubeTextureSamplerDESC));
	cubeTextureSamplerDESC.Filter = D3D11_FILTER_ANISOTROPIC;
	cubeTextureSamplerDESC.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeTextureSamplerDESC.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeTextureSamplerDESC.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeTextureSamplerDESC.ComparisonFunc = D3D11_COMPARISON_NEVER;
	cubeTextureSamplerDESC.MinLOD = 0;
	cubeTextureSamplerDESC.MaxLOD = D3D11_FLOAT32_MAX;

	pDevice->CreateSamplerState(&cubeTextureSamplerDESC, &pCubeTextureSampler);



	//Create Cube RasterState
	D3D11_RASTERIZER_DESC cubeRasterDESC;
	ZeroMemory(&cubeRasterDESC, sizeof(cubeRasterDESC));
	cubeRasterDESC.AntialiasedLineEnable = true;
	cubeRasterDESC.FillMode = D3D11_FILL_SOLID;
	cubeRasterDESC.CullMode = D3D11_CULL_FRONT;
	pDevice->CreateRasterizerState(&cubeRasterDESC, &pCubeRSf);

	ZeroMemory(&cubeRasterDESC, sizeof(cubeRasterDESC));
	cubeRasterDESC.AntialiasedLineEnable = true;
	cubeRasterDESC.FillMode = D3D11_FILL_SOLID;
	cubeRasterDESC.CullMode = D3D11_CULL_BACK;
	pDevice->CreateRasterizerState(&cubeRasterDESC, &pCubeRSb);


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
	cube_matrix = XMMatrixTranslation(0.0f, 0.0f, 5.0f);
	star_matrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixTranslation(0.0f, 0.5f, 5.0f);

	another_camera.UpdateProjection(60.0f, (float)swapchain_DESC.BufferDesc.Width / (float)2, (float)swapchain_DESC.BufferDesc.Height, NEAR_PLANE, FAR_PLANE);
	another_camera.SetPosition({ 10.0f, 1.0f, 0.0f });
	another_camera.RotateY(-90.0f);
	another_camera.UpdateView();
	camera.UpdateProjection(60.0f, (float)swapchain_DESC.BufferDesc.Width / (float)2, (float)swapchain_DESC.BufferDesc.Height, NEAR_PLANE, FAR_PLANE);
	heli.GO_worldMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) *XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	ground.GO_worldMatrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
	skybox.GO_worldMatrix = XMMatrixIdentity();
	parkLight.GO_worldMatrix = XMMatrixTranslation(0.0f, -1.0f, 0.0f);

	//Lights
	allLights.amLight.lightColor = { 0.58f, 0.58f, 0.58f,1.0f };
	allLights.dLight.lightColor = { 1.0f, 1.0f, 1.0f,1.0f };
	allLights.dLight.lightDirection = { 0.58f, -0.58f, 0.58f,1.0f };
	allLights.pLight.lightColor = { 0.0f,1.0f,0.0f,1.0f };
	allLights.pLight.lightPosition = { 5.0f,3.0f,0.0f,1.0f };
	allLights.sLight.lightPosition = { camera.GetPosition().x,camera.GetPosition().y,camera.GetPosition().z,1.0f };
	allLights.sLight.lightColor = { 0.0f,0.0f,1.0f,1.0f };
	allLights.sLight.coneDirAndRatio = { camera.GetForward().x,camera.GetForward().y,camera.GetForward().z,0.8f };


	//Light Constant Buffer
	D3D11_BUFFER_DESC constbufferLightDesc;
	ZeroMemory(&constbufferLightDesc, sizeof(constbufferLightDesc));
	constbufferLightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constbufferLightDesc.ByteWidth = sizeof(allLights);
	constbufferLightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constbufferLightDesc.Usage = D3D11_USAGE_DYNAMIC;
	constbufferLightDesc.MiscFlags = 0;
	constbufferLightDesc.StructureByteStride = 0;
	pDevice->CreateBuffer(&constbufferLightDesc, NULL, &pLightingBuffer);


	scene.percent = 0.0f;
}

DEMO::~DEMO()
{


}



bool DEMO::Run()
{

	xTime.Signal();
	static double timer = 0.0;
	static float SPEED = 1.0f / 600.0f;
	timer += xTime.Delta();
	if (timer > ANIMATION_SPEED)
	{
		timer = 0;

		XMMATRIX rotYN = XMMatrixRotationY(XMConvertToRadians(-1.0f));
		star_matrix = rotYN * star_matrix;


		if (scene.percent > 1.0f || scene.percent < 0.0f)
		{
			SPEED = -SPEED;
		}
		scene.percent += SPEED;
		//for (size_t i = 0; i < cubeInstancedData.size(); i++)
		//{
		//	XMMATRIX rotY = XMMatrixRotationY(XMConvertToRadians((float)(i+10))) * XMLoadFloat4x4(&cubeInstancedData[i]);
		//	XMStoreFloat4x4(&cubeInstancedData[i], rotY);
		//}


		//Directional Light Movement
		XMVECTOR DLdir = XMLoadFloat4(&allLights.dLight.lightDirection);
		DLdir = XMVector4Transform(DLdir, XMMatrixRotationX(0.005f));
		XMStoreFloat4(&allLights.dLight.lightDirection, DLdir);


		//Point Light Movement
		XMVECTOR PLpos = XMLoadFloat4(&allLights.pLight.lightPosition);
		PLpos = XMVector4Transform(PLpos, XMMatrixRotationY(0.01f));
		XMStoreFloat4(&allLights.pLight.lightPosition, PLpos);

	}
	if (current_camera)
	{
		SHORT left, right, up, down, shift, w, a, s, d;
		left = GetAsyncKeyState(VK_LEFT);
		a = GetAsyncKeyState('A');
		right = GetAsyncKeyState(VK_RIGHT);
		d = GetAsyncKeyState('D');
		up = GetAsyncKeyState(VK_UP);
		w = GetAsyncKeyState('W');
		down = GetAsyncKeyState(VK_DOWN);
		s = GetAsyncKeyState('S');
		shift = GetAsyncKeyState(VK_SHIFT);
		if (left || a)
		{
			current_camera->Stafe(-(float)xTime.Delta() * 10);
		}


		if (right || d)
		{
			current_camera->Stafe((float)xTime.Delta() * 10);

		}

		if (up || w)
		{
			current_camera->Walk((float)xTime.Delta() * 10);
		}

		if (down || s)
		{
			current_camera->Walk(-(float)xTime.Delta() * 10);
		}

		if (shift && up)
		{
			current_camera->Climb((float)xTime.Delta() * 10);
		}

		if (shift && down)
		{
			current_camera->Climb(-(float)xTime.Delta() * 10);
		}

		GetCursorPos(&CurPos);
		if (/*GetAsyncKeyState(VK_LBUTTON) &&*/ (lastPos.x != CurPos.x || lastPos.y != CurPos.y))
		{
			current_camera->Pitch(0.15f*(CurPos.y - lastPos.y));
			current_camera->RotateY(0.15f*(CurPos.x - lastPos.x));


			RECT rect;
			GetWindowRect(window, &rect);
			if (CurPos.x >= rect.right)
			{
				CurPos.x = rect.left;
			}
			else if (CurPos.x <= rect.left)
			{
				CurPos.x = rect.right;
			}
			else if (CurPos.y <= rect.top)
			{
				CurPos.y = rect.bottom;
			}
			else if (CurPos.y >= rect.bottom - 1)
			{
				CurPos.y = rect.top;
			}
			SetCursorPos(CurPos.x, CurPos.y);
			lastPos.x = CurPos.x;
			lastPos.y = CurPos.y;
		}



		float cube1tocamera = ObjectToCamera(&cubeInstancedData[0], current_camera->GetPosition());
		float cube2tocamera = ObjectToCamera(&cubeInstancedData[1], current_camera->GetPosition());
		float cube3tocamera = ObjectToCamera(&cubeInstancedData[2], current_camera->GetPosition());

		if (cube1tocamera < cube2tocamera)
		{
			swap(cubeInstancedData[0], cubeInstancedData[1]);
			if (cube1tocamera < cube3tocamera)
			{
				swap(cubeInstancedData[1], cubeInstancedData[2]);
			}
		}

		SecureRelease(pCubeInstanceBuffer);
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(XMFLOAT4X4) * (unsigned int)cubeInstancedData.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA instData;
		ZeroMemory(&instData, sizeof(instData));
		instData.pSysMem = cubeInstancedData.data();
		pDevice->CreateBuffer(&vbd, &instData, &pCubeInstanceBuffer);


	}
	if (GetAsyncKeyState('K') & 0x1)
	{
		current_camera = nullptr;
	}
	if (GetAsyncKeyState('N') & 0x1)
	{
		GetCursorPos(&lastPos);
		current_camera = &another_camera;

	}
	else if (GetAsyncKeyState('O') & 0x1)
	{
		GetCursorPos(&lastPos);
		current_camera = &camera;

	}

	scene._proj = camera.GetProj();
	scene._view = camera.GetView();
	skybox.GO_worldMatrix = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


	thread draw = thread(&DEMO::Draw,this);
	draw.join();
	if (pCommandList)
	{
		pDeviceContext->ExecuteCommandList(pCommandList, false);
		pCommandList->Release();
	}

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
	SecureRelease(pCubeTextureSampler);
	SecureRelease(pCubeShaderResourceView);
	SecureRelease(pCubeRSf);
	SecureRelease(pCubeRSb);
	SecureRelease(pCubeInstanceBuffer);

	SecureRelease(pConstantTimeBuffer);

	SecureRelease(pQuadBuffer);
	SecureRelease(pQuadInputLayout);
	SecureRelease(pQuad_VSShader);
	SecureRelease(pQuad_GSShader);
	SecureRelease(pQuad_PSShader);
	SecureRelease(pQuadSRV);
	SecureRelease(pQuadRTV);
	SecureRelease(pQuad_texture);
	SecureRelease(pConstantQuadBuffer);


	SecureRelease(pSkyboxSunset);
	SecureRelease(pLightingBuffer);
	SecureRelease(pGroundNormalMap);
	SecureRelease(pHeli_VSShader);
	SecureRelease(pHeli_PSShader);
	SecureRelease(pskybox_VSShader);
	SecureRelease(pskybox_PSShader);
	SecureRelease(pStar);
	SecureRelease(pStar_inputLayout);
	SecureRelease(pStar_indexBuffer);
	SecureRelease(pStar_VSShader);
	SecureRelease(pStar_PSShader);
	SecureRelease(pConstantStarBuffer);
	SecureRelease(pRenderTargetView);
	SecureRelease(pDepthStencilView);
	SecureRelease(pZBuffer);
	SecureRelease(pConstantObjectBuffer);
	SecureRelease(pConstantSceneBuffer);
	SecureRelease(pBlendState);
	SecureRelease(pCommandList);

	SecureRelease(pDeferredDeviceContext);
	SecureRelease(pDeviceContext);
	SecureRelease(pSwapchain);
	SecureRelease(pDevice);

	if (s_pInstance)
	{
		delete s_pInstance;
		s_pInstance = nullptr;
	}
	return true;
}


void DEMO::ResizeWindow(UINT _width, UINT _height)
{
	if (pSwapchain != nullptr)
	{
		if (_width <= 0)
		{
			_width = 1;
		}
		if (_height <= 0)
		{
			_height = 1;
		}
		pDeviceContext->OMSetRenderTargets(0, NULL, NULL);
		SecureRelease(pRenderTargetView);
		pDeviceContext->ClearState();
		DXGI_SWAP_CHAIN_DESC swapchain_DESC;
		ZeroMemory(&swapchain_DESC, sizeof(swapchain_DESC));
		pSwapchain->GetDesc(&swapchain_DESC);
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
		ZBufferdesc.SampleDesc.Count = swapchain_DESC.SampleDesc.Count;
		ZBufferdesc.SampleDesc.Quality = swapchain_DESC.SampleDesc.Quality;
		ZBufferdesc.Usage = D3D11_USAGE_DEFAULT;
		ZBufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ZBufferdesc.CPUAccessFlags = 0;
		ZBufferdesc.MiscFlags = 0;
		pDevice->CreateTexture2D(&ZBufferdesc, 0, &pZBuffer);
		pDevice->CreateDepthStencilView(pZBuffer, NULL, &pDepthStencilView);
		camera.UpdateProjection(60.0f, (float)_width / 2.0f, (float)_height, NEAR_PLANE, FAR_PLANE);
		another_camera.UpdateProjection(60.0f, (float)_width / 2.0f, (float)_height, NEAR_PLANE, FAR_PLANE);
		viewport.Width = (float)_width / 2.0f;
		viewport.Height = (float)_height;
		another_viewport.TopLeftX = (float)_width / 2.0f;
		another_viewport.Width = (float)_width / 2.0f;
		another_viewport.Height = (float)_height;


		SecureRelease(pQuadRTV);
		SecureRelease(pQuad_texture);
		SecureRelease(pQuadSRV);
		D3D11_TEXTURE2D_DESC renderToTextureDesc;
		ZeroMemory(&renderToTextureDesc, sizeof(renderToTextureDesc));
		renderToTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		renderToTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		renderToTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		renderToTextureDesc.Height = _height;
		renderToTextureDesc.Width = _width;
		renderToTextureDesc.SampleDesc.Count = 1;
		renderToTextureDesc.ArraySize = 1;
		renderToTextureDesc.MipLevels = 1;
		pDevice->CreateTexture2D(&renderToTextureDesc, NULL, &pQuad_texture);
		pDevice->CreateRenderTargetView(pQuad_texture, 0, &pQuadRTV);
		quadDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		quadDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		quadDesc.Texture2D.MipLevels = 1;
		quadDesc.Texture2D.MostDetailedMip = 0;
		pDevice->CreateShaderResourceView(pQuad_texture, &quadDesc, &pQuadSRV);
	}
}