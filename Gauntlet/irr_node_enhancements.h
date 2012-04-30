#ifndef IRR_NODE_ENHANCEMENTS_H
#define IRR_NODE_ENHANCEMENTS_H

#include <irrlicht.h>

void PointNodeAtPosition( irr::scene::ISceneNode* node, irr::core::vector3df target );

bool NodeEquality( const irr::scene::ISceneNode* a, const irr::scene::ISceneNode* b);

#endif