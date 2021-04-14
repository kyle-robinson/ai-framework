#pragma once
#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "DrawableGameObject.h"
#include "C2DMatrix.h"

class MovingEntity : public DrawableGameObject
{
public:
	MovingEntity(
		Vector2D position,
		double radius,
		Vector2D velocity,
		double maxSpeed,
		Vector2D heading,
		double mass,
		double turnRate,
		double maxForce
	)
		:
		DrawableGameObject( position, radius ),
		m_vHeading( heading ),
		m_vVelocity( velocity ),
		m_dMass( mass ),
		m_vSide( m_vHeading.Perp() ),
		m_dMaxSpeed( maxSpeed ),
		m_dMaxTurnRate( turnRate ),
		m_dMaxForce( maxForce )
	{ }
	//virtual ~MovingEntity();

	// speed
	double GetMaxSpeed() const noexcept { return m_dMaxSpeed; }
	void SetMaxSpeed( double newSpeed ) noexcept { m_dMaxSpeed = newSpeed; }

	// accessors
	Vector2D GetVelocity() const noexcept { return m_vVelocity; }
	void SetVelocity( const Vector2D& newVel ) noexcept { m_vVelocity = newVel; }

	double GetMass() const noexcept { return m_dMass; }

	Vector2D GetSide() const noexcept { return m_vSide; }

	double GetMaxForce() const noexcept { return m_dMaxForce; }
	void SetMaxForce( double maxForce ) noexcept { m_dMaxForce = maxForce; }

	bool IsSpeedMaxedOut() const noexcept { return m_dMaxSpeed * m_dMaxSpeed >= m_vVelocity.LengthSq(); }
	double GetSpeed() const noexcept { return m_vVelocity.Length(); }
	double GetSpeedSq() const noexcept { return m_vVelocity.LengthSq(); }

	Vector2D GetHeading() const noexcept { return m_vHeading; }
	void SetHeading( Vector2D newHeading ) noexcept
	{
		m_vHeading = newHeading;
	}
	bool RotateHeadingToFaceDirection( Vector2D target )
	{
		Vector2D toTarget = Vec2DNormalize( target - GetPosition() );

		//first determine the angle between the heading vector and the target
		double angle = acos( m_vHeading.Dot( toTarget ) );

		//return true if the player is facing the target
		if ( angle < 0.00001 ) return true;

		//clamp the amount to turn to the max turn rate
		if ( angle > m_dMaxTurnRate ) angle = m_dMaxTurnRate;

		//The next few lines use a rotation matrix to rotate the player's heading
		//vector accordingly
		C2DMatrix RotationMatrix;

		//notice how the direction of rotation has to be determined when creating
		//the rotation matrix
		RotationMatrix.Rotate( angle * m_vHeading.Sign( toTarget ) );
		RotationMatrix.TransformVector2Ds( m_vHeading );
		RotationMatrix.TransformVector2Ds( m_vVelocity );

		//finally recreate m_vSide
		m_vSide = m_vHeading.Perp();

		return false;
	}

	double GetMaxTurnRate() const noexcept { m_dMaxTurnRate; }
	void SetMaxTurnRate( double maxTurnRate ) { m_dMaxTurnRate = maxTurnRate; }
protected:
	double m_dMass;
	double m_dMaxSpeed;
	double m_dMaxForce;
	double m_dMaxTurnRate;
	Vector2D m_vSide;
	Vector2D m_vHeading;
	Vector2D m_vVelocity;
	Vector2D m_currentPosition;
};

#endif