#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <vector>
using namespace DirectX;

class DrawableGameObject;
class Waypoint;
class PickupItem;

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

#endif