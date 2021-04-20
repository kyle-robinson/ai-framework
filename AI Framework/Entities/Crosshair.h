#pragma once
#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "DrawableGameObject.h"

class Crosshair : public DrawableGameObject
{
public:
	Crosshair( float x, float y, float r = 0.0f ) :
		DrawableGameObject( { x, y }, r )
	{}
	Crosshair( Vector2D pos, float r = 0.0f ) :
		DrawableGameObject( pos, r )
	{}

	HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pDevice )
	{
		m_vScale = { 10, 10 };
		SetTextureName( L"Resources\\Textures\\crosshair_corner.dds" );
		return DrawableGameObject::InitMesh( pDevice );
	}
};

#endif