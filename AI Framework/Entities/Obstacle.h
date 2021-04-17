#pragma once
#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "DrawableGameObject.h"

class Obstacle : public DrawableGameObject
{
public:
	Obstacle( float x, float y, float r ) :
		DrawableGameObject( { x, y }, r )
	{}
	Obstacle( Vector2D pos, float r ) :
		DrawableGameObject( pos, r )
	{}
	
	HRESULT	InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pDevice )
	{
		m_vScale = { 30, 30 };
		SetTextureName( L"Resources\\Textures\\tyre.dds" );
		return DrawableGameObject::InitMesh( pDevice );
	}
};

#endif