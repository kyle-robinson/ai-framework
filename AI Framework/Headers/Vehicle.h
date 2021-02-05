#pragma once
#include "DrawableGameObject.h"
#include "Vector2D.h"

#define MAX_SPEED 300

class Vehicle : public DrawableGameObject
{
public:
	virtual HRESULT initMesh(ID3D11Device* pd3dDevice, const std::wstring& texturePath);
	virtual void update(const float deltaTime);

	void setMaxSpeed(const float maxSpeed) { m_maxSpeed = maxSpeed; }
	void setCurrentSpeed(const float speed); // a ratio: a value between 0 and 1 (1 being max speed)
	void setPositionTo(Vector2D positionTo); // a position to move to
	void setVehiclePosition(Vector2D position); // the current position - this resets positionTo

	float getMaxSpeed() const { return m_maxSpeed; };
	float getCurrentSpeed() const { return m_currentSpeed; };

protected:
	float m_maxSpeed;
	float m_currentSpeed;
	
	Vector2D m_currentPosition;
	Vector2D m_startPosition;
	Vector2D m_positionTo;
	Vector2D m_lastPosition;

};

