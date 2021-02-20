#include "PickupItem.h"

HRESULT	PickupItem::initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice )
{
	setTextureName( L"Resources\\Textures\\yellow.dds" );

	m_scale = { 20, 20, 1 };
	setTextureName( L"Resources\\Textures\\pickup.dds" );

	HRESULT hr = DrawableGameObject::initMesh( pd3dDevice );
	setPosition( { 100, 200, 0 });

	return hr;
}