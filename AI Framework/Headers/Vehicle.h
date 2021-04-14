#pragma once
#ifndef VEHICLE_H
#define VEHICLE_H

//#include "AIManager.h"
#include "MovingEntity.h"
//#include "SteeringBehaviour.h"

class AIManager;
class SteeringBehaviour;

#define MAX_SPEED 300

class Vehicle : public MovingEntity
{
public:
	Vehicle(
		AIManager* aiManager,
		Vector2D position,
		double rotation,
		Vector2D velocity,
		double mass,
		double maxForce,
		double maxSpeed,
		double maxTurnRate
	);
	virtual HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, const std::wstring& texturePath );
	virtual void Update( const float deltaTime );
	AIManager* const World() const noexcept { return m_pWorld; }
	SteeringBehaviour* const Steering() const noexcept { return m_pSteering; }
private:
	AIManager* m_pWorld;
	SteeringBehaviour* m_pSteering;
};

#endif