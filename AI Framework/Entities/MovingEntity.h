#pragma once
#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "DrawableGameObject.h"
#include "Math/C2DMatrix.h"

class MovingEntity : public DrawableGameObject
{
public:
	MovingEntity(
		Vector2D position,
		float radius,
		Vector2D velocity,
		float maxSpeed,
		Vector2D heading,
		float mass,
		float turnRate,
		float maxForce
	)
		:
		DrawableGameObject( position, radius ),
		m_vHeading( heading ),
		m_vVelocity( velocity ),
		m_fMass( mass ),
		m_vSide( m_vHeading.Perp() ),
		m_fMaxSpeed( maxSpeed ),
		m_fMaxTurnRate( turnRate ),
		m_fMaxForce( maxForce )
	{ }

	// speed
	float GetMaxSpeed() const noexcept { return m_fMaxSpeed; }
	void SetMaxSpeed( float newSpeed ) noexcept { m_fMaxSpeed = newSpeed; }

	bool IsSpeedMaxedOut() const noexcept { return m_fMaxSpeed * m_fMaxSpeed >= m_vVelocity.LengthSq(); }
	float GetSpeed() const noexcept { return m_vVelocity.Length(); }
	float GetSpeedSq() const noexcept { return m_vVelocity.LengthSq(); }

	float GetMaxTurnRate() const noexcept { m_fMaxTurnRate; }
	void SetMaxTurnRate( float maxTurnRate ) { m_fMaxTurnRate = maxTurnRate; }

	// forces
	Vector2D GetVelocity() const noexcept { return m_vVelocity; }
	void SetVelocity( const Vector2D& newVel ) noexcept { m_vVelocity = newVel; }

	float GetMaxForce() const noexcept { return m_fMaxForce; }
	void SetMaxForce( float maxForce ) noexcept { m_fMaxForce = maxForce; }

	float GetMass() const noexcept { return m_fMass; }

	// accessors
	Vector2D GetSide() const noexcept { return m_vSide; }
	Vector2D GetHeading() const noexcept { return m_vHeading; }
	void SetHeading( Vector2D newHeading ) noexcept
	{
		m_vHeading = newHeading;
	}
	bool RotateHeadingToFaceDirection( Vector2D target )
	{
		Vector2D toTarget = Vec2DNormalize( target - GetPosition() );
		float angle = acos( m_vHeading.Dot( toTarget ) );

		// return if player is already facing the target
		if ( angle < 0.00001 ) return true;
		if ( angle > m_fMaxTurnRate ) angle = m_fMaxTurnRate;

		// rotate the player's heading vector
		C2DMatrix RotationMatrix;
		RotationMatrix.Rotate( angle * m_vHeading.Sign( toTarget ) );
		RotationMatrix.TransformVector2Ds( m_vHeading );
		RotationMatrix.TransformVector2Ds( m_vVelocity );

		m_vSide = m_vHeading.Perp();

		return false;
	}
protected:
	float m_fMass;
	float m_fMaxSpeed;
	float m_fMaxForce;
	float m_fMaxTurnRate;
	Vector2D m_vSide;
	Vector2D m_vHeading;
	Vector2D m_vVelocity;
	Vector2D m_vCurrentPosition;
};

#endif