#pragma once

#include <irrlicht.h>
#include "MyEventReceiver.h"
#include "math_enhancements.h" //round()

//converts a given position, to be used for the position of the 2d->3d cursor
irr::core::vector3df vftovi( const irr::core::vector3df& v );

//converts the cursors position to 3d coords
irr::core::vector3df MouseTo3D(const MyEventReceiver& Receiver, irr::scene::ISceneCollisionManager *colMan, irr::scene::ITriangleSelector *triSel);