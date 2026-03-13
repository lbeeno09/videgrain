#pragma once

#include <DirectXMath.h>

struct VideoVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

struct VideoConstantBuffer
{
	float opacity;
	float brightness;
	float contrast;
	float saturation;

	float multiplierX;
	float multiplierY;
	float padding[2];
};