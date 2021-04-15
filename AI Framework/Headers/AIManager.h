#pragma once
#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <vector>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "Vehicle.h"

class PickupItem;
class Waypoint;

typedef std::vector<DrawableGameObject*> vecDrawables;
typedef std::vector<Waypoint*> vecWaypoints;
typedef std::vector<PickupItem*> vecPickups;

class AIManager
{
public:
	HRESULT Initialise( HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, UINT width, UINT height );
	void Update( const float fDeltaTime );
	void HandleKeyPresses( WPARAM param );
	
	Vector2D GetCrosshair() const noexcept { return m_crosshair; }
	void SetCrosshair( const double x, const double y ) noexcept { m_crosshair = { x, y }; }

	HWND GetHWND() const noexcept { return hWnd; }
	int GetScreenX() const noexcept { return width; }
	int GetScreenY() const noexcept { return height; }

	Waypoint* GetWaypoint( const int x, const int y );
	vecWaypoints GetNeighbours( const int x, const int y );
	vecWaypoints GetWaypoints() const noexcept { return m_waypoints; }

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

	Vehicle* m_pCarRed;
	Vehicle* m_pCarBlue;
	vecPickups m_pickups;
	vecWaypoints m_waypoints;
	std::vector<Vector2D> path;
};

#endif