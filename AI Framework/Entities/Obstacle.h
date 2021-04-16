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
	
	HRESULT	InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device, const std::wstring& texturePath )
	{
		m_vScale = { 30, 30 };
		SetTextureName( texturePath );
		return DrawableGameObject::InitMesh( device );
	}
	void Update( const float dt )
	{
		DrawableGameObject::Update( dt );
	}
};

#endif