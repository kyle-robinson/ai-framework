#pragma once
#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <vector>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
//#include "Vector2D.h"
#include "Vehicle.h"

class PickupItem;
class Waypoint;

typedef std::vector<DrawableGameObject*> vecDrawables;
typedef std::vector<Waypoint*> vecWaypoints;
typedef std::vector<PickupItem*> vecPickups;

class AIManager
{
public:
	Waypoint* GetWaypoint( const int x, const int y );
	vecWaypoints GetNeighbours( const int x, const int y );
	vecWaypoints GetWaypoints() const noexcept { return m_waypoints; }

	HRESULT Initialise( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, UINT width, UINT height );
	void Update( const float fDeltaTime );
	void HandleKeyPresses( WPARAM param );
	
	Vector2D GetCrosshair() const noexcept { return m_crosshair; }
	void SetCrosshair( const double x, const double y ) noexcept { m_crosshair = { x, y }; }
protected:
	//void checkWallWrapping( Vehicle* car );
	bool checkForCollisions( Vehicle* car );
	//void Wander( Vehicle* car );
	//Vector2D Flee( Vector2D TargetPos );
	//Vector2D Arrive( Vector2D TargetPos, Deceleration deceleration );
private:
	void TogglePause() noexcept { m_paused = !m_paused; }
	bool Paused() const noexcept { return m_paused; }

	UINT width;
	UINT height;

	bool m_paused = false;
	vecWaypoints m_waypoints;
	vecPickups m_pickups;
	Vector2D m_crosshair;
	Vehicle* m_pCar;
	//Vehicle* m_pCar2 = nullptr;
	std::vector<Vector2D> path;
};

#endif