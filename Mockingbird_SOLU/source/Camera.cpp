#include "Camera.h"



Camera::Camera() : up(0.0f, 1.0f, 0.0f), side(1.0f, 0.0f, 0.0f), forward(0.0f, 0.0f, 1.0f), position(0.0f, 1.0f, -10.0f)
{
	XMFLOAT3 focus { 0.0f,0.0f,1.0f };
	view = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&focus), XMLoadFloat3(&up));
	proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV_DEGREE), BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT, NEAR_PLANE, FAR_PLANE);
}


Camera::~Camera()
{
}

void Camera::UpdateView()
{
	view = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&up));
}

void Camera::UpdateProjection(float _fovDegree,float _width,float _height, float _nPlane, float _fPlane)
{
	proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(_fovDegree), _width / _height, _nPlane, _fPlane);
}

void Camera::Walk(float _speed)
{
	XMStoreFloat3(&position, XMVectorMultiplyAdd(XMVectorReplicate(_speed), XMLoadFloat3(&forward), XMLoadFloat3(&position)));
	UpdateView();
}

void Camera::Stafe(float _speed)
{
	XMStoreFloat3(&position, XMVectorMultiplyAdd(XMVectorReplicate(_speed), XMLoadFloat3(&side), XMLoadFloat3(&position)));
	UpdateView();
}

void Camera::Climb(float _speed)
{
	XMFLOAT3 y{ 0.0f,1.0f,0.0f };
	XMStoreFloat3(&position, XMVectorMultiplyAdd(XMVectorReplicate(_speed), XMLoadFloat3(&y), XMLoadFloat3(&position)));
	UpdateView();
}

void Camera::Pitch(float _angleDegree)
{
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&side), XMConvertToRadians(_angleDegree));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rot));
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), rot));
	UpdateView();
}

void Camera::RotateY(float _angleDegree)
{
	XMMATRIX rot = XMMatrixRotationY(XMConvertToRadians(_angleDegree));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rot));
	XMStoreFloat3(&side, XMVector3TransformNormal(XMLoadFloat3(&side), rot));
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), rot));
	UpdateView();
}
