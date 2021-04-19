#pragma once
#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "DrawableGameObject.h"
#include <vector>

class Waypoint : public DrawableGameObject
{
public:
    Waypoint() : DrawableGameObject( { 0.0f, 0.0f }, 0.0f ) {}
    HRESULT InitMesh( Microsoft::WRL::ComPtr<ID3D11Device> device, const int index );

    bool IsVisible() const noexcept { return m_bVisible; }
    bool IsOnTrack() const noexcept { return m_bIsOnTrack; }
    void SetVisible( const bool visible ) noexcept { m_bVisible = visible; }

private:
    bool m_bVisible;
    bool m_bIsOnTrack;
    bool m_bIsCheckpoint;
};

#endif