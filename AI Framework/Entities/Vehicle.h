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
		float rotation,
		Vector2D velocity,
		float mass,
		float maxForce,
		float maxSpeed,
		float maxTurnRate
	);
	HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device, const std::wstring& texturePath );
	void Update( const float dt ) override;
	float GetDeltaTime() const noexcept { return m_fTimeElapsed; }

	AIManager* const World() const noexcept { return m_pWorld; }
	SteeringBehaviour* const Steering() const noexcept { return m_pSteering; }
private:
	float m_fTimeElapsed;
	AIManager* m_pWorld;
	SteeringBehaviour* m_pSteering;
};

#endif