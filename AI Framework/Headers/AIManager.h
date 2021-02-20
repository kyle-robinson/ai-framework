#pragma once
#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <vector>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>

struct Vector2D;

class Vehicle;
class DrawableGameObject;
class PickupItem;
class Waypoint;

typedef std::vector<DrawableGameObject*> vecDrawables;
typedef std::vector<Waypoint*> vecWaypoints;
typedef std::vector<PickupItem*> vecPickups;

enum Deceleration { slow = 3, normal = 2, fast = 1 };

class AIManager
{
public:
	Waypoint* GetWaypoint( const int x, const int y );
	vecWaypoints GetNeighbours( const int x, const int y );
	vecWaypoints GetWaypoints() const noexcept { return m_waypoints; }

	HRESULT initialise( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, UINT width, UINT height );
	void update( const float fDeltaTime );
	void LeftMouseUp( const int x, int y );
	void RightMouseUp( const int x, int y );
	void keyPress( WPARAM param );
protected:
	void checkWallWrapping( Vehicle* car );
	bool checkForCollisions( Vehicle* car );
	void Wander( Vehicle* car );
	Vector2D Flee( Vector2D TargetPos );
	Vector2D Arrive( Vector2D TargetPos, Deceleration deceleration );
private:
	UINT width;
	UINT height;

	vecWaypoints m_waypoints;
	vecPickups m_pickups;
	Vehicle* m_pCar = nullptr;
	Vehicle* m_pCar2 = nullptr;
	std::vector<Vector2D> path;
};

#endif