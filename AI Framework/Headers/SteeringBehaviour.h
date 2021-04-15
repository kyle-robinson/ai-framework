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
	enum Deceleration
	{
		SLOW	= 3,
		NORMAL	= 2,
		FAST	= 1
	} m_deceleration;
private:
	Vector2D m_vSteeringForce;
	Vehicle* m_pTargetAgent;
	Vehicle* m_pVehicle;

	// behaviour parameters
	Vector2D m_vWanderTarget;
	double m_dWanderJitter;
	double m_dWanderRadius;
	double m_dWanderDistance;

	double m_dWeightPursuit;
	double m_dWeightWander;
	double m_dWeightArrive;
	double m_dWeightFlee;
	double m_dWeightSeek;

	// behaviours
	bool AccumulateForce( Vector2D& RunningTot, Vector2D ForceToAdd );
	Vector2D Arrive( Vector2D TargetPos, Deceleration deceleration );
	Vector2D Pursuit( const Vehicle* agent );
	Vector2D Flee( Vector2D TargetPos );
	Vector2D Seek( Vector2D TargetPos );
	Vector2D Wander();

	int m_iFlags;
	bool On( BehaviourType type ) { return ( m_iFlags & type ) == type; }
public:
	SteeringBehaviour( Vehicle* vehicle );

	// forces
	Vector2D Calculate();
	double ForwardComponent();
	double SideComponent();
	Vector2D Force() const noexcept { return m_vSteeringForce; }

	// arrive
	void ArriveOn() { m_iFlags |= ARRIVE; }
	void ArriveOff() { if ( On( ARRIVE ) ) m_iFlags ^= ARRIVE; }
	bool IsArriveOn() { return On( ARRIVE ); }

	// flee
	void FleeOn() { m_iFlags |= FLEE; }
	void FleeOff() { if ( On( FLEE ) ) m_iFlags ^= FLEE; }
	bool IsFleeOn() { return On( FLEE ); }

	// seek
	void SeekOn() { m_iFlags |= SEEK; }
	void SeekOff() { if ( On( SEEK ) ) m_iFlags ^= SEEK; }
	bool IsSeekOn() { return On( SEEK ); }

	// pursuit
	void PursuitOn( Vehicle* vehicle ) { m_iFlags |= PURSUIT; m_pTargetAgent = vehicle; }
	void PursuitOff() { if ( On( PURSUIT ) ) m_iFlags ^= PURSUIT; }
	bool IsPursuitOn() { return On( PURSUIT ); }

	// wander
	void WanderOn() { m_iFlags |= WANDER; }
	void WanderOff() { if ( On( WANDER ) ) m_iFlags ^= WANDER; }
	bool IsWanderOn() { return On( WANDER ); }

	double GetWanderJitter() const { return m_dWanderJitter; }
	double GetWanderDistance() const { return m_dWanderDistance; }
	double GetWanderRadius() const { return m_dWanderRadius; }
};

#endif