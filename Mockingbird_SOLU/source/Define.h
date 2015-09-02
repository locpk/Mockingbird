#pragma once
//General Header files include
#include <d3d11.h>
#include <DirectXMath.h>
#include "XTime.h"
#include "DDSTextureLoader.h"

using namespace std;
using namespace DirectX;

#define BACKBUFFER_WIDTH	1280.0f
#define BACKBUFFER_HEIGHT	720.0f
#define FOV_DEGREE 60.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define ANIMATION_SPEED (1.0 / 60.0)
#define SecureRelease(p) (p)->Release(); (p) = nullptr;