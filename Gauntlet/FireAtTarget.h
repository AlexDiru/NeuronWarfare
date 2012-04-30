#pragma once

//this function needs to be procedural else entity and entitymanager would both depend on each other

#include <irrlicht.h>
#include "MazeRenderer.h"
#include "EntityManager.h"
#include "irr_vector_enhancements.h"

//enemy manager is redundant until there are enemies
void FireAtTarget(const MyEventReceiver& Receiver, EntityManager& PlayerManager, EntityManager& EnemyManager, 
					MazeRenderer& MR, irr::scene::ISceneManager *smgr, irr::scene::ISceneNode *Cursor);	