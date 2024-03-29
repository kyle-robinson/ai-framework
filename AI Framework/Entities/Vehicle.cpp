#include "Vehicle.h"
#include "AIManager.h"
#include "SteeringBehaviour.h"

Vehicle::Vehicle(
		AIManager* aiManager,
		Vector2D position,
		float rotation,
		Vector2D velocity,
		float mass,
		float maxForce,
		float maxSpeed,
		float maxTurnRate
	)
	:
	MovingEntity(
		position,
		3.0f,
		velocity,
		maxSpeed,
		Vector2D( sin( rotation ), -cos( rotation ) ),
		mass,
		maxTurnRate,
		maxForce
	),
	m_pManager( aiManager )
{
	m_pSteering = new SteeringBehaviour( this );
}

HRESULT	Vehicle::InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::wstring& texturePath )
{
	m_vScale = { 30, 20 };
	SetTextureName( texturePath );
	return DrawableGameObject::InitMesh( pDevice );
}

void Vehicle::Update( const float dt )
{
	m_fTimeElapsed = dt;
	Vector2D currentPos = GetPosition();

	// calculate net force
	Vector2D steeringForce = m_pSteering->CalculateSteeringBehaviours();

	// a = f / m
	Vector2D acceleration = steeringForce / m_fMass;
	m_vVelocity += acceleration * dt;

	// update the position
	m_vVelocity.Truncate( m_fMaxSpeed );
	m_vPosition += m_vVelocity * dt;

	// update the heading if the vehicle has a non zero velocity
	if ( m_vVelocity.LengthSq() > 0.00000001f )
	{
		m_vHeading = Vec2DNormalize( m_vVelocity );
		m_vSide = m_vHeading.Perp();
	}

	// handle vehicle screen wrapping
	RECT rect;
	GetClientRect( m_pManager->GetHWND(), &rect );
	WrapAround( m_vPosition, rect.right / 2, rect.bottom / 2 );

	// update the vehicle's rotation
	Vector2D diff = m_vPosition - currentPos;
	if ( diff.Length() > 0.0f )
	{
		diff.Normalize();
		m_fRadianRotation = atan2f( diff.y, diff.x );
	}

	// update vehicle's matrices
	DrawableGameObject::Update( dt );
}