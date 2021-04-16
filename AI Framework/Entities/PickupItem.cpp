#include "PickupItem.h"

HRESULT	PickupItem::initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice )
{
	SetTextureName( L"Resources\\Textures\\yellow.dds" );

	m_vScale = { 20, 20 };
	SetTextureName( L"Resources\\Textures\\pickup.dds" );

	HRESULT hr = DrawableGameObject::InitMesh( pd3dDevice );
	SetPosition( { 100, 200 });

	return hr;
}