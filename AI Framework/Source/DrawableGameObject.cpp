#include "DrawableGameObject.h"

DrawableGameObject::DrawableGameObject( Vector2D position, double radius ) :
	m_position( position ),
	m_dBoundingRadius( radius ),
	m_scale( { 10.0f, 10.0f } )
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTextureResourceView = nullptr;
	m_pSamplerLinear = nullptr;

	// Initialize the world matrix
	XMStoreFloat4x4( &m_world, DirectX::XMMatrixIdentity() );

	//m_position = { 0, 0 };
	//m_direction = { 1, 0 };
	//m_scale = { 10, 10 };
	SetTextureName( L"Resources\\Textures\\stone.dds" );

	//m_radianRotation = 0;
}

/*void DrawableGameObject::SetDirection( XMFLOAT3 direction )
{
	XMVECTOR v = XMLoadFloat3( &direction );
	v = XMVector3Normalize( v );
	XMStoreFloat3( &XMFLOAT3( m_direction.x, m_direction.y, 0.0f ), v );
}*/

DrawableGameObject::~DrawableGameObject()
{
	if( m_pVertexBuffer ) 
		m_pVertexBuffer->Release();
	
	if( m_pIndexBuffer )
		m_pIndexBuffer->Release();	

	if ( m_pTextureResourceView )
		m_pTextureResourceView->Release();

	if ( m_pSamplerLinear )
		m_pSamplerLinear->Release();
}

HRESULT DrawableGameObject::initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice )
{
	// Create vertex buffer
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

	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( SimpleVertex ) * ARRAYSIZE( vertices );
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	HRESULT hr = pd3dDevice->CreateBuffer( &bd, &InitData, &m_pVertexBuffer );
	if ( FAILED( hr ) ) return hr;

	// Create index buffer
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

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( WORD ) * NUM_VERTICES;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = pd3dDevice->CreateBuffer( &bd, &InitData, &m_pIndexBuffer );
	if ( FAILED( hr ) ) return hr;
	
	// load and setup textures
	hr = CreateDDSTextureFromFile( pd3dDevice.Get(), m_textureName.c_str(), nullptr, &m_pTextureResourceView );
	if ( FAILED( hr ) ) return hr;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState( &sampDesc, &m_pSamplerLinear );

	return hr;
}

void DrawableGameObject::update( float deltaTime )
{
	//XMMATRIX mRotate = DirectX::XMMatrixRotationZ( m_radianRotation );
	XMMATRIX mTranslate = DirectX::XMMatrixTranslation( m_position.x, m_position.y, 1.0f );
	XMMATRIX mScale = DirectX::XMMatrixScaling( m_scale.x, m_scale.y, 10.0f );
	//XMStoreFloat4x4( &m_world, mScale * mRotate * mTranslate );
	XMStoreFloat4x4( &m_world, mScale * mTranslate );
}

XMFLOAT3 DrawableGameObject::addFloat3( XMFLOAT3& f1, XMFLOAT3& f2 )
{
	XMFLOAT3 out;
	out.x = f1.x + f2.x;
	out.y = f1.y + f2.y;
	out.z = f1.z + f2.z;

	return out;
}

XMFLOAT3 DrawableGameObject::subtractFloat3( XMFLOAT3& f1, XMFLOAT3& f2 )
{
	XMFLOAT3 out;
	out.x = f1.x - f2.x;
	out.y = f1.y - f2.y;
	out.z = f1.z - f2.z;

	return out;
}

XMFLOAT3 DrawableGameObject::multiplyFloat3( XMFLOAT3& f1, const float scalar )
{
	XMFLOAT3 out;
	out.x = f1.x * scalar;
	out.y = f1.y * scalar;
	out.z = f1.z * scalar;

	return out;
}

XMFLOAT3 DrawableGameObject::divideFloat3( XMFLOAT3& f1, const float scalar )
{
	XMFLOAT3 out;
	out.x = f1.x / scalar;
	out.y = f1.y / scalar;
	out.z = f1.z / scalar;

	return out;
}

float DrawableGameObject::magnitudeFloat3( XMFLOAT3& f1 )
{
	return sqrt( ( f1.x * f1.x ) + ( f1.y * f1.y ) + ( f1.z * f1.z ) );
}

XMFLOAT3 DrawableGameObject::normaliseFloat3(XMFLOAT3& f1)
{
	float length = sqrt( ( f1.x * f1.x ) + ( f1.y * f1.y ) + ( f1.z * f1.z ) );

	f1.x /= length;
	f1.y /= length;
	f1.z /= length;

	return f1;
}

void DrawableGameObject::draw( Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext )
{
	UINT offset = 0;
	UINT stride = sizeof( SimpleVertex );
	pContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	pContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pContext->DrawIndexed( NUM_VERTICES, 0, 0 );
}