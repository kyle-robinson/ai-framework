#pragma once
#ifndef PICKUPITEM_H
#define PICKUPITEM_H

#include "DrawableGameObject.h"

class PickupItem : public DrawableGameObject
{
public:
    virtual HRESULT initMesh( Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice );
};

#endif