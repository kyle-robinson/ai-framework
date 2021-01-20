#pragma once
#include "DrawableGameObject.h"

class PickupItem :
    public DrawableGameObject
{
public:
    virtual HRESULT initMesh(ID3D11Device* pd3dDevice);

protected:

};

