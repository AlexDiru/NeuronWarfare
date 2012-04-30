#include "GameCamera.h"

void GameCamera::ManageHeight( int MouseScroll )
{
	TotalHeight = Height + MouseScroll;
	MovementScalar = (TotalHeight/35)*3;
	if (MovementScalar < 1)
		MovementScalar = 1;
}

void GameCamera::MoveWithArrowKeys(MyEventReceiver& R)
{
	camera->setPosition( irr::core::vector3df( camera->getPosition().X + MovementScalar*((R.IsKeyDown(irr::KEY_UP) - R.IsKeyDown(irr::KEY_DOWN))),
									TotalHeight,
									camera->getPosition().Z + MovementScalar*((R.IsKeyDown(irr::KEY_LEFT) - R.IsKeyDown(irr::KEY_RIGHT))) ) );
}

void GameCamera::MoveWithWASD(MyEventReceiver& R)
{
	camera->setPosition( irr::core::vector3df( camera->getPosition().X + MovementScalar*((R.IsKeyDown(irr::KEY_KEY_W) - R.IsKeyDown(irr::KEY_KEY_S))),
									TotalHeight,
									camera->getPosition().Z + MovementScalar*((R.IsKeyDown(irr::KEY_KEY_A) - R.IsKeyDown(irr::KEY_KEY_D))) ) );
}

void GameCamera::Update(MyEventReceiver& Receiver, irr::scene::ISceneNode* Node)
{
	MoveWithArrowKeys(Receiver);
	camera->setTarget( irr::core::vector3df( camera->getPosition().X, Node->getPosition().Y, camera->getPosition().Z) );
	ManageHeight(-Receiver.GetMouseState().MouseScroll);
}