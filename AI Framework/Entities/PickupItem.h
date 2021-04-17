#pragma once
#ifndef PICKUPITEM_H
#define PICKUPITEM_H

#include "DrawableGameObject.h"

class PickupItem : public DrawableGameObject
{
public:
	PickupItem( float x, float y, float r ) :
		DrawableGameObject( { x, y }, r )
	{}
	PickupItem( Vector2D pos, float r ) :
		DrawableGameObject( pos, r )
	{}

    HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pDevice )
	{
		m_vScale = { 20, 20 };
		SetTextureName( L"Resources\\Textures\\pickup.dds" );
		return DrawableGameObject::InitMesh( pDevice );
	}
};

#endif