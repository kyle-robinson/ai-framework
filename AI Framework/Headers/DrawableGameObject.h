#pragma once
#ifndef DRAWABLEGAMEOBJECT_H
#define DRAWABLEGAMEOBJECT_H

#include <iostream>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>

#include "DDSTextureLoader.h"
#include "structures.h"
#include "resource.h"

#define NUM_VERTICES 36

class DrawableGameObject
{
public:
	DrawableGameObject();
	virtual ~DrawableGameObject();

	virtual HRESULT initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice );
	virtual void update( const float t );
	void draw( Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext );

	ID3D11ShaderResourceView** getTextureResourceView() { return &m_pTextureResourceView; 	}
	XMFLOAT4X4* getTransform() { return &m_world; }
	ID3D11SamplerState** getTextureSamplerState() { return &m_pSamplerLinear; }

	void setPosition( XMFLOAT3 position );
	XMFLOAT3* getPosition() noexcept { return &m_position; }
	XMFLOAT3* getDirection() noexcept { return &m_direction; }
protected:
	void setDirection( XMFLOAT3 direction );
	void setTextureName( std::wstring texName ) noexcept { m_textureName = texName; }

	// helper functions
	XMFLOAT3 addFloat3( XMFLOAT3& f1, XMFLOAT3& f2 );
	XMFLOAT3 subtractFloat3( XMFLOAT3& f1, XMFLOAT3& f2 );
	XMFLOAT3 normaliseFloat3( XMFLOAT3& f1 );
	float magnitudeFloat3( XMFLOAT3& f1 );
	XMFLOAT3 multiplyFloat3( XMFLOAT3& f1, const float scalar );
	XMFLOAT3 divideFloat3( XMFLOAT3& f1, const float scalar );

	XMFLOAT3 m_scale;
	float m_radianRotation;
private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11ShaderResourceView* m_pTextureResourceView;
	ID3D11SamplerState* m_pSamplerLinear;
	
	XMFLOAT4X4 m_world;
	XMFLOAT3 m_position;
	XMFLOAT3 m_direction;
	std::wstring m_textureName;	
};

#endif