#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <DirectXMath.h>
using namespace DirectX;

class DrawableGameObject;
class PickupItem;
class Waypoint;

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct SimpleVertex
{
	XMFLOAT3 fPosition;
	XMFLOAT3 fNormal;
	XMFLOAT2 fTexCoord;
};

#endif