#include <irrlicht.h>
#include "MyEventReceiver.h"

// Class the manage the camera in game (read user input etc)

class GameCamera
{
public:
	irr::scene::ICameraSceneNode *camera;

	GameCamera( int defHeight ) : Height(defHeight) {}

	void Update(MyEventReceiver& Receiver, irr::scene::ISceneNode* Node);

private:

	int TotalHeight; //actual height of camera (accounts for mouse scroll)
	int MovementScalar; //faster movement when the camera is zoomed out
	int Height; //default height of camera

	void MoveWithArrowKeys(MyEventReceiver& R); //allows the userto control the camera with arrow keys 
	void MoveWithWASD(MyEventReceiver& R); //allows the userto control the camera with WASD

	void ManageHeight( int MouseScroll ); //calculates the height and scalar given a mousescroll
};