#pragma once
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"

#define NUM_INDICES 6
using namespace DirectX;

class Background
{
public:
	Background();
	HRESULT	InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device );
	void Draw( Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context );

	XMFLOAT4X4* GetTransform() noexcept { return &m_mWorld; }
	ID3D11SamplerState** GetTextureSamplerState() noexcept { return m_pSamplerLinear.GetAddressOf(); }
	ID3D11ShaderResourceView** GetTextureResourceView() noexcept { return m_pTextureResourceView.GetAddressOf(); }
private:
	XMFLOAT4X4 m_mWorld;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerLinear;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureResourceView;
};

#endif