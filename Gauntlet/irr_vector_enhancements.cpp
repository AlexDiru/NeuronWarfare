#include "irr_vector_enhancements.h"

irr::core::vector3df vftovi( const irr::core::vector3df& v )
{
	return irr::core::vector3df ( round(v.X/10)*10, (int)v.Y, round(v.Z/10)*10 );
}

irr::core::vector3df MouseTo3D(const MyEventReceiver& Receiver, irr::scene::ISceneCollisionManager *colMan, irr::scene::ITriangleSelector *triSel)
{
	irr::core::position2d<irr::s32> Cur ( Receiver.MouseState.Position );

	const irr::core::line3d<irr::f32> Ray = colMan->getRayFromScreenCoordinates(Cur);

	irr::core::vector3df point;

	irr::core::triangle3df triangle;

	const irr::scene::ISceneNode *node = NULL;

	if (colMan->getCollisionPoint( Ray, triSel, point, triangle, node))
		return point;

	return irr::core::vector3df(0,0,0);
}