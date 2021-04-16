#include "Background.h"
#include "Structures.h"

Background::Background()
{
	XMStoreFloat4x4( &m_mWorld, XMMatrixIdentity() );
}

HRESULT Background::InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device )
{
	SimpleVertex vertices[] =
	{
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
	};

	WORD indices[] =
	{
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( WORD ) * NUM_INDICES;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData = { 0 };
	iinitData.pSysMem = indices;
	HRESULT hr = device->CreateBuffer( &indexBufferDesc, &iinitData, &m_pIndexBuffer );

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( SimpleVertex ) * ARRAYSIZE( vertices );
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = vertices;
	hr = device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &m_pVertexBuffer );

	// load and setup textures
	hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\track.dds", nullptr, &m_pTextureResourceView );
	if ( FAILED( hr ) ) return hr;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState( &sampDesc, &m_pSamplerLinear );

	return hr;
}

void DepthTest( const bool isOn, Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext )
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	if ( isOn )
	{
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	}
	else
	{
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	}

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* m_DepthStencilState = { 0 };
	pd3dDevice->CreateDepthStencilState( &depthStencilDesc, &m_DepthStencilState );
	pContext->OMSetDepthStencilState( m_DepthStencilState, 0 );
}

void Background::Draw( Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context )
{
	// turn depth write off
	DepthTest( false, device, context );
	UINT stride = sizeof( SimpleVertex );
	UINT offset = 0;

	context->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset );
	context->IASetIndexBuffer( m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->DrawIndexed( NUM_INDICES, 0, 0 );

	// turn depth write back on
	DepthTest( true, device, context );
}