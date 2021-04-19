#pragma once
#ifndef DRAWABLEGAMEOBJECT_H
#define DRAWABLEGAMEOBJECT_H

#include <iostream>
#include <d3d11_1.h>
#include <wrl/client.h>
#include "Math/Vector2D.h"
#include "Structures.h"

#define NUM_VERTICES 36

class DrawableGameObject
{
public:
	DrawableGameObject( Vector2D position, float radius );
	virtual void Update( const float dt );
	void Draw( Microsoft::WRL::ComPtr<ID3D11DeviceContext> context );

	XMFLOAT4X4* GetTransform() { return &m_mWorld; }
	ID3D11SamplerState** GetTextureSamplerState() { return m_pSamplerLinear.GetAddressOf(); }
	ID3D11ShaderResourceView** GetTextureResourceView() { return m_pTextureResourceView.GetAddressOf(); }

	void SetPosition( Vector2D position ) noexcept { m_vPosition = position; }
	Vector2D GetPosition() const noexcept { return m_vPosition; }

	void SetScale( Vector2D scale ) noexcept { m_vScale = scale; }
	Vector2D GetScale() const noexcept { return m_vScale; }

	float GetBoundingRadius() const noexcept { return m_fBoundingRadius; }
	void SetBoundingRadius( float boundingRadius ) noexcept { m_fBoundingRadius = boundingRadius; }

	void Tag() { m_bTag = true; }
	void UnTag() { m_bTag = false; }
	bool IsTagged() const { return m_bTag; }
protected:
	HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device );
	void SetTextureName( std::wstring texName ) noexcept { m_sTextureName = texName; }

	// helper functions
	XMFLOAT3 NormaliseFloat3( XMFLOAT3& f1 ) noexcept;
	float MagnitudeFloat3( const XMFLOAT3& f1 ) noexcept;
	XMFLOAT3 AddFloat3( const XMFLOAT3& f1, const XMFLOAT3& f2 ) noexcept;
	XMFLOAT3 SubtractFloat3( const XMFLOAT3& f1, const XMFLOAT3& f2 ) noexcept;
	XMFLOAT3 MultiplyFloat3( const XMFLOAT3& f1, const float scalar ) noexcept;
	XMFLOAT3 DivideFloat3( const XMFLOAT3& f1, const float scalar ) noexcept;

	Vector2D m_vScale;
	Vector2D m_vPosition;
	float m_fRadianRotation;
	float m_fBoundingRadius;
private:
	XMFLOAT4X4 m_mWorld;
	bool m_bTag = false;
	std::wstring m_sTextureName;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerLinear;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureResourceView;
};

#endif