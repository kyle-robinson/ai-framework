#pragma once
#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <vector>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "Obstacle.h"
#include "Vehicle.h"

class PickupItem;
class Waypoint;

typedef std::vector<DrawableGameObject*> vecDrawables;
typedef std::vector<Obstacle*> vecObstacles;
typedef std::vector<Waypoint*> vecWaypoints;
typedef std::vector<PickupItem*> vecPickups;

inline bool PointInCircle( Vector2D Pos, float radius, Vector2D p )
{
	float DistFromCenterSquared = ( p - Pos ).LengthSq();
	if ( DistFromCenterSquared < ( radius * radius ) )
		return true;
	return false;
}

template <class T, class conT>
void TagNeighbors( const T& entity, conT& ContainerOfEntities, float radius )
{
	//iterate through all entities checking for range
	for ( typename conT::iterator curEntity = ContainerOfEntities.begin();
		curEntity != ContainerOfEntities.end();
		++curEntity )
	{
		//first clear any current tag
		( *curEntity )->UnTag();

		Vector2D to = ( *curEntity )->GetPosition() - entity->GetPosition();

		//the bounding radius of the other is taken into account by adding it 
		//to the range
		float range = radius + ( *curEntity )->GetBoundingRadius();

		//if entity within range, tag for further consideration. (working in
		//distance-squared space to avoid sqrts)
		if ( ( ( *curEntity ) != entity ) && ( to.LengthSq() < range * range ) )
		{
			( *curEntity )->Tag();
		}

	}//next entity
}

class AIManager
{
public:
	HRESULT Initialise( HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, UINT width, UINT height );
	void Update( const float fDeltaTime );
	void HandleKeyPresses( WPARAM param );
	void CreateObstacles();
	
	Vector2D GetCrosshair() const noexcept { return m_crosshair; }
	void SetCrosshair( const float x, const float y ) noexcept
	{
		Vector2D ProposedPosition( x, y );
		for ( vecDrawables::iterator curOb = m_obstacles.begin(); curOb != m_obstacles.end(); ++curOb )
		{
			if ( PointInCircle( ( *curOb )->GetPosition(), ( *curOb )->GetBoundingRadius(), ProposedPosition ) )
			{
				return;
			}
		}
		m_crosshair = { x, y };
	}

	HWND GetHWND() const noexcept { return hWnd; }
	int GetScreenX() const noexcept { return width; }
	int GetScreenY() const noexcept { return height; }

	Waypoint* GetWaypoint( const int x, const int y );
	vecWaypoints GetNeighbours( const int x, const int y );
	vecWaypoints GetWaypoints() const noexcept { return m_waypoints; }
	vecDrawables GetObstacles() noexcept { return m_obstacles; }

	void  TagObstaclesWithinViewRange( DrawableGameObject* pVehicle, float range )
	{
		TagNeighbors( pVehicle, m_obstacles, range );
	}

	void SpawnControlWindow();
protected:
	bool checkForCollisions( Vehicle* car );
private:
	void TogglePause() noexcept { m_paused = !m_paused; }
	bool IsPaused() const noexcept { return m_paused; }

	HWND hWnd;
	UINT width;
	UINT height;

	Vector2D m_crosshair;
	bool m_paused = false;
	bool m_bEnableRedCar = false;
	bool m_bShowObstacles = false;

	Vehicle* m_pCarRed;
	Vehicle* m_pCarBlue;
	vecPickups m_pickups;
	vecWaypoints m_waypoints;
	vecDrawables m_obstacles;
	std::vector<Vector2D> path;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
};

#endif