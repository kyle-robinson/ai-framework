#include "DrawableGameObject.h"
#include "DDSTextureLoader.h"
#include "Structures.h"

DrawableGameObject::DrawableGameObject( Vector2D position, float radius ) :
	m_vPosition( position ),
	m_fBoundingRadius( radius ),
	m_vScale( { 10.0f, 10.0f } )
{
	XMStoreFloat4x4( &m_mWorld, DirectX::XMMatrixIdentity() );
}

HRESULT DrawableGameObject::InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device )
{
	SimpleVertex vertices[] =
	{
		{ { -1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, 1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f, 1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },

		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f,  1.0f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f, -1.0f,  1.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },

		{ { -1.0f, -1.0f,  1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f,  1.0f,  1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },

		{ { 1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { 1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { 1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },

		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ {  1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },

		{ { -1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
		{ {  1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
		{ {  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	};

	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	// vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleVertex ) * ARRAYSIZE( vertices );
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	HRESULT hr = device->CreateBuffer( &bd, &InitData, &m_pVertexBuffer );
	if ( FAILED( hr ) ) return hr;

	// index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( WORD ) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = device->CreateBuffer( &bd, &InitData, &m_pIndexBuffer );
	if ( FAILED( hr ) ) return hr;

	// load and setup textures
	hr = CreateDDSTextureFromFile( device.Get(), m_sTextureName.c_str(), nullptr, &m_pTextureResourceView );
	if ( FAILED( hr ) ) return hr;

	CD3D11_SAMPLER_DESC samplerDesc( CD3D11_DEFAULT{} );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = device->CreateSamplerState( &samplerDesc, &m_pSamplerLinear );

	return hr;
}

void DrawableGameObject::Update( const float dt )
{
	XMMATRIX mRotate = DirectX::XMMatrixRotationZ( m_fRadianRotation );
	XMMATRIX mTranslate = DirectX::XMMatrixTranslation( m_vPosition.x, m_vPosition.y, 1.0f );
	XMMATRIX mScale = DirectX::XMMatrixScaling( m_vScale.x, m_vScale.y, 10.0f );
	XMStoreFloat4x4( &m_mWorld, mScale * mRotate * mTranslate );
}

void DrawableGameObject::Draw( Microsoft::WRL::ComPtr<ID3D11DeviceContext> context )
{
	UINT offset = 0;
	UINT stride = sizeof( SimpleVertex );
	context->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset );
	context->IASetIndexBuffer( m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0 );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	context->DrawIndexed( NUM_VERTICES, 0, 0 );
}

XMFLOAT3 DrawableGameObject::AddFloat3( const XMFLOAT3& f1, const XMFLOAT3& f2 ) noexcept
{
	XMFLOAT3 out;
	out.x = f1.x + f2.x;
	out.y = f1.y + f2.y;
	out.z = f1.z + f2.z;

	return out;
}

XMFLOAT3 DrawableGameObject::SubtractFloat3( const XMFLOAT3 & f1, const XMFLOAT3 & f2 ) noexcept
{
	XMFLOAT3 out;
	out.x = f1.x - f2.x;
	out.y = f1.y - f2.y;
	out.z = f1.z - f2.z;

	return out;
}

XMFLOAT3 DrawableGameObject::MultiplyFloat3( const XMFLOAT3 & f1, const float scalar ) noexcept
{
	XMFLOAT3 out;
	out.x = f1.x * scalar;
	out.y = f1.y * scalar;
	out.z = f1.z * scalar;

	return out;
}

XMFLOAT3 DrawableGameObject::DivideFloat3( const XMFLOAT3 & f1, const float scalar ) noexcept
{
	XMFLOAT3 out;
	out.x = f1.x / scalar;
	out.y = f1.y / scalar;
	out.z = f1.z / scalar;

	return out;
}

float DrawableGameObject::MagnitudeFloat3( const XMFLOAT3 & f1 ) noexcept
{
	return sqrt( ( f1.x * f1.x ) + ( f1.y * f1.y ) + ( f1.z * f1.z ) );
}

XMFLOAT3 DrawableGameObject::NormaliseFloat3( XMFLOAT3 & f1 ) noexcept
{
	float length = sqrt( ( f1.x * f1.x ) + ( f1.y * f1.y ) + ( f1.z * f1.z ) );

	f1.x /= length;
	f1.y /= length;
	f1.z /= length;

	return f1;
}