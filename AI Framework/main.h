#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <d3d11_1.h>
#include <windowsx.h>
#include <d3dcompiler.h>

#include "Resources/resource.h"
#include "DrawableGameObject.h"
#include "DDSTextureLoader.h"
#include "structures.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define WAYPOINT_RESOLUTION 20

void AddItemToDrawList( DrawableGameObject* object );

#endif