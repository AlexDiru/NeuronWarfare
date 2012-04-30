#include "irr_node_enhancements.h"

void PointNodeAtPosition( irr::scene::ISceneNode* node, irr::core::vector3df target )
{
	//only y axis
	//sina = (x2-x1)/(z2-z1)
	
	float x = target.X - node->getPosition().X;
	float z = target.Z - node->getPosition().Z;

	float a = sinh(x/z);

	node->setRotation( irr::core::vector3df( node->getRotation().X, node->getRotation().Y + a, node->getRotation().Z) );
}

bool NodeEquality( const irr::scene::ISceneNode* a, const irr::scene::ISceneNode* b)
{
	return (a->getPosition() == b->getPosition()) && (a->getScale() == b->getScale()) && (a->getRotation() == b->getRotation()) && (a->getMaterialCount() == b->getMaterialCount());
}