#pragma once
#ifndef STEERINGBEHAVIOUR_H
#define STEERINGBEHAVIOUR_H

#include <vector>
#include <memory>
#include <string>
#include "Path.h"
#include "Vehicle.h"

class Vehicle;

const float waypointSeekDist = 20;

class SteeringBehaviour
{
private:
	enum BehaviourType
	{
		NONE		= 0x00000,
		ARRIVE		= 0x00002,
		SEEK		= 0x00004,
		FLEE		= 0x00008,
		PURSUIT		= 0x00010,
		OBSTACLE	= 0x00020,
		WANDER		= 0x00040,
		FOLLOW_PATH = 0x00080
	};
public:
	enum Deceleration
	{
		SLOW	= 3,
		NORMAL	= 2,
		FAST	= 1
	};
private:
	Path* m_pPath;
	Vehicle* m_pVehicle;
	Vehicle* m_pTargetAgent;
	Vector2D m_vSteeringForce;

	// behaviour parameters
	Vector2D m_vWanderTarget;
	float m_fDBoxLength = 40.0f;
	float m_fWanderRadius = 10.0f;
	float m_fWanderJitter = 200.0f;
	float m_fWanderDistance = 10.0f;
	double m_fWaypointSeekDistSq = powf( waypointSeekDist, 2 );

	// weightings
	float m_fWeightObstacleAvoidance = 10.0f;
	float m_fWeightFollowPath = 10.0f;
	float m_fWeightPursuit = 1.0f;
	float m_fWeightWander = 10.0f;
	float m_fWeightArrive = 1.0f;
	float m_fWeightFlee = 10.0f;
	float m_fWeightSeek = 1.0f;

	// behaviours
	bool AccumulateForce( Vector2D& netForce, Vector2D newForce );
	Vector2D ObstacleAvoidance( const std::vector<DrawableGameObject*>& obstacles );
	Vector2D Arrive( Vector2D target, Deceleration deceleration );
	Vector2D Pursuit( const Vehicle* agent );
	Vector2D Flee( Vector2D target );
	Vector2D Seek( Vector2D target );
	Vector2D FollowPath();
	Vector2D Wander();

	uint32_t m_uFlags = 0u;
	Deceleration m_eDeceleration = NORMAL;
	bool On( BehaviourType type ) { return ( m_uFlags & type ) == type; }
public:
	SteeringBehaviour( Vehicle* vehicle );
	~SteeringBehaviour();

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

	double GetWanderJitter() const { return m_fWanderJitter; }
	double GetWanderDistance() const { return m_fWanderDistance; }
	double GetWanderRadius() const { return m_fWanderRadius; }

	// path following
	void FollowPathOn() { m_uFlags |= FOLLOW_PATH; }
	void FollowPathOff() { if ( On( FOLLOW_PATH ) ) m_uFlags ^= FOLLOW_PATH; }
	bool IsFollowPathOn() { return On( FOLLOW_PATH ); }

	void SetPath( std::list<Vector2D> new_path ) { m_pPath->Set( new_path ); }
	void CreateRandomPath( int num_waypoints, int mx, int my, int cx, int cy )const
	{
		m_pPath->CreateRandomPath( num_waypoints, mx, my, cx, cy );
	}
};

#endif