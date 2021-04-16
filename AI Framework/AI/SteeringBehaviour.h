#pragma once
#ifndef STEERINGBEHAVIOUR_H
#define STEERINGBEHAVIOUR_H

#include <vector>
#include <memory>
#include <string>
#include "Vehicle.h"

class Vehicle;

class SteeringBehaviour
{
private:
	enum BehaviourType
	{
		NONE		= 0x00000,
		ARRIVE		= 0x00002,
		SEEK		= 0x00004,
		FLEE		= 0x00006,
		PURSUIT		= 0x00008,
		OBSTACLE	= 0x00010,
		WANDER		= 0x00020
	};
public:
	enum Deceleration
	{
		SLOW	= 3,
		NORMAL	= 2,
		FAST	= 1
	};
private:
	Vector2D m_vSteeringForce;
	Vehicle* m_pTargetAgent = nullptr;
	Vehicle* m_pVehicle = nullptr;

	// behaviour parameters
	Vector2D m_vWanderTarget;
	float m_dDBoxLength = 40.0f;
	float m_dWanderRadius = 10.0f;
	float m_dWanderJitter = 200.0f;
	float m_dWanderDistance = 10.0f;

	// weightings
	float m_dWeightObstacleAvoidance = 10.0;
	float m_dWeightPursuit = 1.0;
	float m_dWeightWander = 10.0;
	float m_dWeightArrive = 1.0;
	float m_dWeightFlee = 1.0;
	float m_dWeightSeek = 1.0;

	// behaviours
	bool AccumulateForce( Vector2D& netForce, Vector2D newForce );
	Vector2D ObstacleAvoidance( const std::vector<DrawableGameObject*>& obstacles );
	Vector2D Arrive( Vector2D target, Deceleration deceleration );
	Vector2D Pursuit( const Vehicle* agent );
	Vector2D Flee( Vector2D target );
	Vector2D Seek( Vector2D target );
	Vector2D Wander();

	uint32_t m_uFlags = 0u;
	Deceleration m_eDeceleration = NORMAL;
	bool On( BehaviourType type ) { return ( m_uFlags & type ) == type; }
public:
	SteeringBehaviour( Vehicle* vehicle );

	// force components
	Vector2D Calculate();
	double ForwardComponent();
	double SideComponent();
	Vector2D Force() const noexcept { return m_vSteeringForce; }
	void SetDeceleration( Deceleration deceleration ) { m_eDeceleration = deceleration; }

	// obstacle avoidance
	void ObstacleAvoidanceOn() { m_uFlags |= OBSTACLE; }
	void ObstacleAvoidanceOff() { if ( On( OBSTACLE ) ) m_uFlags ^= OBSTACLE; }
	bool IsObstacleAvoidanceOn() { return On( OBSTACLE ); }

	// arrive
	void ArriveOn() { m_uFlags |= ARRIVE; }
	void ArriveOff() { if ( On( ARRIVE ) ) m_uFlags ^= ARRIVE; }
	bool IsArriveOn() { return On( ARRIVE ); }

	// flee
	void FleeOn() { m_uFlags |= FLEE; }
	void FleeOff() { if ( On( FLEE ) ) m_uFlags ^= FLEE; }
	bool IsFleeOn() { return On( FLEE ); }

	// seek
	void SeekOn() { m_uFlags |= SEEK; }
	void SeekOff() { if ( On( SEEK ) ) m_uFlags ^= SEEK; }
	bool IsSeekOn() { return On( SEEK ); }

	// pursuit
	void PursuitOn( Vehicle* vehicle ) { m_uFlags |= PURSUIT; m_pTargetAgent = vehicle; }
	void PursuitOff() { if ( On( PURSUIT ) ) m_uFlags ^= PURSUIT; }
	bool IsPursuitOn() { return On( PURSUIT ); }

	// wander
	void WanderOn() { m_uFlags |= WANDER; }
	void WanderOff() { if ( On( WANDER ) ) m_uFlags ^= WANDER; }
	bool IsWanderOn() { return On( WANDER ); }

	double GetWanderJitter() const { return m_dWanderJitter; }
	double GetWanderDistance() const { return m_dWanderDistance; }
	double GetWanderRadius() const { return m_dWanderRadius; }
};

#endif