#pragma once

#include <irrlicht.h>
#include "Entity.h"

void AIFireAtTarget(Entity *AI, Entity *Target, irr::scene::ISceneManager *smgr, irr::scene::IMetaTriangleSelector *coverTriSel);