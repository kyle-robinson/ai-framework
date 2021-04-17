#pragma once
#ifndef WINMAIN_H
#define WINMAIN_H

#include "Structures.h"
#include "DrawableGameObject.h"
#include "Resources/resource.h"
#include <windowsx.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define WAYPOINT_RESOLUTION 20

void AddItemToDrawList( DrawableGameObject* object );

#endif