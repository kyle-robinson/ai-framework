#pragma once
#include "DrawableGameObject.h"
class Waypoint :
    public DrawableGameObject
{
public:
    virtual HRESULT initMesh(ID3D11Device* pd3dDevice, const int index);

    bool isOnTrack() { return m_isOnTrack; }
    void setVisible(const bool visible) { m_visible = visible; }
    bool isVisible() {return m_visible;}

private:
    bool m_isOnTrack;
    bool m_isCheckpoint;
    bool m_visible;
};

