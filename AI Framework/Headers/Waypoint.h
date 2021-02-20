#pragma once
#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "DrawableGameObject.h"

class Waypoint : public DrawableGameObject
{
public:
    virtual HRESULT initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, const int index );

    bool isOnTrack() const noexcept { return m_isOnTrack; }
    void setVisible( const bool visible ) noexcept { m_visible = visible; }
    bool isVisible() const noexcept { return m_visible; }

private:
    bool m_isOnTrack;
    bool m_isCheckpoint;
    bool m_visible;
};

#endif