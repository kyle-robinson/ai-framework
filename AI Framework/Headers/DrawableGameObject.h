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
#include "Vector2D.h"

#define NUM_VERTICES 36

class DrawableGameObject
{
public:
	DrawableGameObject( Vector2D position, double radius );
	virtual ~DrawableGameObject();

	virtual HRESULT initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice );
	virtual void update( const float deltaTime );
	void draw( Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext );

	ID3D11ShaderResourceView** getTextureResourceView() { return &m_pTextureResourceView; 	}
	XMFLOAT4X4* getTransform() { return &m_world; }
	ID3D11SamplerState** getTextureSamplerState() { return &m_pSamplerLinear; }

	void SetPosition( Vector2D position ) noexcept { m_position = position; }
	Vector2D GetPosition() const noexcept { return m_position; }

	void Tag() { m_bTag = true; }
	void UnTag() { m_bTag = false; }
	bool IsTagged()const { return m_bTag; }

	double GetBoundingRadius() const noexcept { return m_dBoundingRadius; }
	void SetBoundingRadius( double boundingRadius ) noexcept { m_dBoundingRadius = boundingRadius; }
protected:
	void SetTextureName( std::wstring texName ) noexcept { m_textureName = texName; }

	// helper functions
	XMFLOAT3 addFloat3( XMFLOAT3& f1, XMFLOAT3& f2 );
	XMFLOAT3 subtractFloat3( XMFLOAT3& f1, XMFLOAT3& f2 );
	XMFLOAT3 normaliseFloat3( XMFLOAT3& f1 );
	float magnitudeFloat3( XMFLOAT3& f1 );
	XMFLOAT3 multiplyFloat3( XMFLOAT3& f1, const float scalar );
	XMFLOAT3 divideFloat3( XMFLOAT3& f1, const float scalar );

	Vector2D m_scale;
	Vector2D m_position;
	float m_radianRotation;
	double m_dBoundingRadius;
private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11ShaderResourceView* m_pTextureResourceView;
	ID3D11SamplerState* m_pSamplerLinear;
	
	bool m_bTag;
	XMFLOAT4X4 m_world;
	std::wstring m_textureName;	
};

#endif