#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include "attackResult.h"
#include "Entity.h"

void AIFireAtTarget(Entity *AI, Entity *Target, irr::scene::ISceneManager *smgr, irr::scene::IMetaTriangleSelector *coverTriSel);