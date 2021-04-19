#pragma once
#ifndef PICKUPITEM_H
#define PICKUPITEM_H

#include "DrawableGameObject.h"

std::vector<int> g_vPickupPositions{ 159,54,176,264,47,162,362 };

class PickupItem : public DrawableGameObject
{
public:
	PickupItem( float x, float y, float r ) :
		DrawableGameObject( { x, y }, r )
	{}
	PickupItem( Vector2D pos, float r ) :
		DrawableGameObject( pos, r )
	{}

    HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const int index )
	{
		if ( std::find( g_vPickupPositions.begin(), g_vPickupPositions.end(), index ) != g_vPickupPositions.end() )
		{
			m_vScale = { 20, 20 };
			SetTextureName( L"Resources\\Textures\\pickup.dds" );
			return DrawableGameObject::InitMesh( pDevice );
		}
		return E_FAIL;
	}
};

#endif