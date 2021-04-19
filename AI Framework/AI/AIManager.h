#pragma once
#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <vector>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "EntityTemplates.h"
#include "Obstacle.h"
#include "Vehicle.h"
#include "Path.h"

class PickupItem;
class Waypoint;
class Path;

typedef std::vector<DrawableGameObject*> vecDrawables;
typedef std::vector<Obstacle*> vecObstacles;
typedef std::vector<Waypoint*> vecWaypoints;
typedef std::vector<PickupItem*> vecPickups;

class AIManager
{
public:
	HRESULT Initialise( HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height );
	void Update( const float dt );
	void HandleKeyPresses( WPARAM param );
	void CreateObstacles();
	
	// crosshair
	Vector2D GetCrosshair() const noexcept { return m_crosshair; }
	void SetCrosshair( const float x, const float y ) noexcept
	{
		Vector2D posToMoveTo( x, y );
		for ( vecDrawables::iterator curOb = m_obstacles.begin(); curOb != m_obstacles.end(); ++curOb )
		{
			// prevent moving to a space occupied by an obstacle
			if ( PointInCircle( ( *curOb )->GetPosition(), ( *curOb )->GetBoundingRadius(), posToMoveTo ) )
				return;
		}
		m_crosshair = { x, y };
	}

	// objects
	vecWaypoints GetWaypoints() const noexcept { return m_waypoints; }
	vecDrawables GetObstacles() noexcept { return m_obstacles; }

	// utility
	void SpawnControlWindow();
	HWND GetHWND() const noexcept { return hWnd; }
	int GetScreenX() const noexcept { return width; }
	int GetScreenY() const noexcept { return height; }
	void TagObstaclesWithinViewRange( DrawableGameObject* pVehicle, float range )
		{ TagNeighbors( pVehicle, m_obstacles, range ); }
protected:
	bool CheckForCollisions( Vehicle* car );
private:
	bool IsPaused() const noexcept { return m_bPaused; }
	void TogglePause() noexcept { m_bPaused = !m_bPaused; }

	HWND hWnd;
	UINT width;
	UINT height;

	Vector2D m_crosshair;
	bool m_bPaused = false;
	bool m_bItemPickedUp = false;
	bool m_bEnableRedCar = false;
	bool m_bShowObstacles = false;

	Path* m_pPath;
	Vehicle* m_pCarRed;
	Vehicle* m_pCarBlue;
	vecPickups m_pickups;
	vecWaypoints m_waypoints;
	vecDrawables m_obstacles;
	std::vector<Vector2D> path;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
};

#endif