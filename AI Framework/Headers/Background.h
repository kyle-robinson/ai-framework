#pragma once
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <iostream>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>

#include "DDSTextureLoader.h"
#include "structures.h"
#include "resource.h"

#define NUM_INDICES 6

/* This could be a drawable game object */

using namespace DirectX;

class Background
{
public:
	Background();
	virtual ~Background();

	virtual HRESULT	initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice );
	void draw( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext );

	ID3D11Buffer* getVertexBuffer() const noexcept { return m_pVertexBuffer; }
	ID3D11Buffer* getIndexBuffer() const noexcept { return m_pIndexBuffer; }
	ID3D11ShaderResourceView** getTextureResourceView() noexcept { return &m_pTextureResourceView; }
	XMFLOAT4X4* getTransform() noexcept { return &m_World; }
	ID3D11SamplerState** getTextureSamplerState() noexcept { return &m_pSamplerLinear; }
private:
	XMFLOAT4X4 m_World;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11ShaderResourceView* m_pTextureResourceView;
	ID3D11SamplerState* m_pSamplerLinear;
};

#endif