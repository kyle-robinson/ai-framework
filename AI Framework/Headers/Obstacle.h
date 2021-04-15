#pragma once
#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "DrawableGameObject.h"

class Obstacle : public DrawableGameObject
{
public:
	Obstacle( double x, double y, double r ) :
		DrawableGameObject( { x, y }, r )
	{}
	Obstacle( Vector2D pos, double r ) :
		DrawableGameObject( pos, r )
	{}
	
	HRESULT	InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, const std::wstring& texturePath )
	{
		m_scale = { 20, 20 };
		SetTextureName( texturePath );
		return DrawableGameObject::initMesh( pd3dDevice );
	}
	void Update( const float deltaTime )
	{
		DrawableGameObject::update( deltaTime );
	}
};

#endif