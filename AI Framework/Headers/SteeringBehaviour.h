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
	double m_dWeightArrive;
	Vehicle* m_pVehicle;
	Vector2D m_vSteeringForce;

	bool AccumulateForce( Vector2D& RunningTot, Vector2D ForceToAdd );
	Vector2D Arrive( Vector2D targetPos, Deceleration deceleration );

	int m_iFlags;
	bool On( BehaviourType type ) { return ( m_iFlags & type ) == type; }
public:
	SteeringBehaviour( Vehicle* vehicle );

	Vector2D Calculate();
	double ForwardComponent();
	double SideComponent();
	Vector2D Force() const noexcept { return m_vSteeringForce; }

	void ArriveOn() { m_iFlags |= ARRIVE; }
	void ArriveOff() { if ( On( ARRIVE ) ) m_iFlags ^= ARRIVE; }
	bool IsArriveOn() { return On( ARRIVE ); }
};

#endif