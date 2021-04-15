#include "Vehicle.h"
#include "AIManager.h"
#include "SteeringBehaviour.h"

Vehicle::Vehicle(
		AIManager* aiManager,
		Vector2D position,
		double rotation,
		Vector2D velocity,
		double mass,
		double maxForce,
		double maxSpeed,
		double maxTurnRate
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
	m_pWorld( aiManager )
{
	m_pSteering = new SteeringBehaviour( this );
}

HRESULT	Vehicle::InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, const std::wstring& texturePath )
{
	m_scale = { 30, 20 };
	SetTextureName( texturePath );
	return DrawableGameObject::initMesh( pd3dDevice );
}

void Vehicle::Update( const float deltaTime )
{
	m_dTimeElapsed = deltaTime;

	Vector2D OldPos = GetPosition();

	// calculate the net force for each steering behaviour
	Vector2D SteeringForce;
	SteeringForce = m_pSteering->Calculate();

	// a = f / m
	Vector2D acceleration = SteeringForce / m_dMass;
	m_vVelocity += acceleration * deltaTime;

	// ensure vehicle does not exceed maximum velocity
	m_vVelocity.Truncate( m_dMaxSpeed );

	// update the position
	m_position += m_vVelocity * deltaTime;

	// update the heading if the vehicle has a non zero velocity
	if ( m_vVelocity.LengthSq() > 0.00000001 )
	{
		m_vHeading = Vec2DNormalize( m_vVelocity );
		m_vSide = m_vHeading.Perp();
	}

	// handle vehicle screen wrapping
	RECT rect;
	GetClientRect( m_pWorld->GetHWND(), &rect );
	int cxClient = rect.right;
	int cyClient = rect.bottom;
	WrapAround( m_position, cxClient / 2, cyClient / 2 );

	// update the vehicle's rotation
	Vector2D diff;
	diff.x = m_position.x - OldPos.x;
	diff.y = m_position.y - OldPos.y;
	if ( diff.Length() > 0.0f )
	{
		diff.Normalize();
		m_radianRotation = atan2f( diff.y, diff.x );
	}

	// update the vehicle's position/rotation/scale matrices
	DrawableGameObject::update( deltaTime );
}