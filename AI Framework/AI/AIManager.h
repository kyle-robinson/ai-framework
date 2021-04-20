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
#include "Crosshair.h"
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
	~AIManager();
	HRESULT Initialise( HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> pDevice, UINT width, UINT height );
	void Update( const float dt );
	void CreateObstacles();

	// objects
	vecWaypoints GetWaypoints() const noexcept { return m_waypoints; }
	Crosshair* GetCrosshair() const noexcept { return m_pCrosshair; }
	vecDrawables GetObstacles() noexcept { return m_obstacles; }
	Vehicle* GetRedCar() const noexcept { return m_pCarRed; }

	// utility
	HWND GetHWND() const noexcept { return hWnd; }
	int GetScreenX() const noexcept { return width; }
	int GetScreenY() const noexcept { return height; }
	void TagObstaclesWithinViewRange( DrawableGameObject* pVehicle, float range )
		{ TagNeighbors( pVehicle, m_obstacles, range ); }
protected:
	bool CheckForCollisions( Vehicle* car );
private:
	void CreateWaypoints();
	void CreatePickups();

	void SpawnBehaviourWindow();
	void SpawnObstacleWindow();
	void SpawnControlWindow();

	HWND hWnd;
	UINT width;
	UINT height;

	bool m_bPaused = false;
	int m_iObstacleCount = 7;
	bool m_bItemPickedUp = false;
	bool m_bEnableRedCar = false;
	bool m_bShowObstacles = false;

	Path* m_pPath;
	Vehicle* m_pCarRed;
	Vehicle* m_pCarBlue;
	vecPickups m_pickups;
	Crosshair* m_pCrosshair;
	vecWaypoints m_waypoints;
	vecDrawables m_obstacles;
	std::vector<Vector2D> path;
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
};

#endif