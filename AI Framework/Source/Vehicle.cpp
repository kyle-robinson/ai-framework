#include "Vehicle.h"

HRESULT	Vehicle::initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, const std::wstring& texturePath )
{
	m_scale = { 30, 20, 1 };
	setTextureName( texturePath );
	HRESULT hr = DrawableGameObject::initMesh( pd3dDevice );

	//m_maxSpeed = 200;
	setMaxSpeed( MAX_SPEED );
	setVehiclePosition( { 0, 0 } );
	
	m_lastPosition = { 0, 0 };
	m_currentSpeed = m_maxSpeed;

	return hr;
}

void Vehicle::update( const float deltaTime )
{
	// consider replacing with force based acceleration / velocity calculations
	Vector2D vecTo = m_positionTo - m_currentPosition;
	float velocity = deltaTime * m_currentSpeed;

	float length = static_cast<float>( vecTo.Length() );
	// if the distance to the end point is less than the car would move, then only move that distance. 
	if ( length > 0 )
	{
		vecTo.Normalize();
		if( length > velocity )
			vecTo *= velocity;
		else
			vecTo *= length;

		m_currentPosition += vecTo;
	}

	// rotate the object based on its last & current position
	Vector2D diff = m_currentPosition - m_lastPosition;
	m_lastPosition = m_currentPosition;
	if ( diff.Length() > 0.1 )
	{ // if zero then don't update rotation
		diff.Normalize();
		m_radianRotation = atan2f( diff.y, diff.x ); // this is used by DrawableGameObject to set the rotation
	}

	// set the current poistion for the drawablegameobject
	setPosition( { static_cast<float>( m_currentPosition.x ), static_cast<float>( m_currentPosition.y ), 0 } );

	DrawableGameObject::update( deltaTime );
}

// a ratio: a value between 0 and 1 (1 being max speed)
void Vehicle::setCurrentSpeed( const float speed )
{
	m_currentSpeed = m_maxSpeed * speed;
	m_currentSpeed = max( 0, m_currentSpeed );
	m_currentSpeed = min( 1, m_currentSpeed );
}

// a position to move to
void Vehicle::setPositionTo( Vector2D position )
{
	m_startPosition = m_currentPosition;
	m_positionTo = position;
}

// the current position
void Vehicle::setVehiclePosition( Vector2D position )
{
	m_currentPosition = position;
	m_positionTo = position;
	m_startPosition = position;
	setPosition( { static_cast<float>( position.x ), static_cast<float>( position.y ), 0 } );
}