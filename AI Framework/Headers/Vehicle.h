#pragma once
#ifndef VEHICLE_H
#define VEHICLE_H

#include "MovingEntity.h"

class AIManager;
class SteeringBehaviour;

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
	double GetDeltaTime() const noexcept { return m_dTimeElapsed; }
private:
	double m_dTimeElapsed;
	AIManager* m_pWorld;
	SteeringBehaviour* m_pSteering;
};

#endif