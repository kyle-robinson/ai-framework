#pragma once
#ifndef STEERINGBEHAVIOUR_H
#define STEERINGBEHAVIOUR_H

#include <vector>
#include <memory>
#include <string>
#include "Path.h"
#include "Vehicle.h"

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
		FOLLOW_PATH	= 0x00080
	};
private:
	Path* m_pPath;
	Vehicle* m_pVehicle;
	Vehicle* m_pTargetAgent;
	Vector2D m_vSteeringForce;

	// behaviour parameters
	Vector2D m_vWanderTarget;
	bool m_bFleeCar = true;
	bool m_bEnablePanic = false;
	float m_fDBoxLength = 40.0f;
	float m_fWanderRadius = 10.0f;
	float m_fWanderJitter = 200.0f;
	float m_fWanderDistance = 10.0f;
	float m_fWaypointSeekDistSq = powf( 20, 2 );

	// behaviours
	bool AccumulateForce( Vector2D& netForce, Vector2D newForce );
	Vector2D ObstacleAvoidance( const std::vector<DrawableGameObject*>& obstacles );
	Vector2D Arrive( Vector2D target );
	Vector2D Pursuit( const Vehicle* agent );
	Vector2D Flee( Vector2D target );
	Vector2D Seek( Vector2D target );
	Vector2D FollowPath();
	Vector2D Wander();

	uint32_t m_uFlags = 0u;
	bool On( BehaviourType type ) { return ( m_uFlags & type ) == type; }
public:
	SteeringBehaviour( Vehicle* vehicle );
	~SteeringBehaviour();

	// force components
	Vector2D CalculateSteeringBehaviours();
	float ForwardComponent() const noexcept { return m_pVehicle->GetHeading().Dot( m_vSteeringForce ); }
	float SideComponent() const noexcept { return m_pVehicle->GetSide().Dot( m_vSteeringForce ); }

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
	void EnablePanic( bool panic ) { m_bEnablePanic = panic; }
	void FleeCar( bool flee ) { m_bFleeCar = flee; }

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

	float GetWanderJitter() const noexcept { return m_fWanderJitter; }
	float GetWanderDistance() const noexcept { return m_fWanderDistance; }
	float GetWanderRadius() const noexcept { return m_fWanderRadius; }

	// path following
	void FollowPathOn() { m_uFlags |= FOLLOW_PATH; }
	void FollowPathOff() { if ( On( FOLLOW_PATH ) ) m_uFlags ^= FOLLOW_PATH; }
	bool IsFollowPathOn() { return On( FOLLOW_PATH ); }
	void SetPath( std::vector<Vector2D> new_path ) { m_pPath->Set( new_path ); }
};

#endif