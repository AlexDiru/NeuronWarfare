//===================================
//include guard
#ifndef MYEVENTRECEIVER_H
#define MYEVENTRECEIVER_H

//===================================
//included dependencies
#include <irrlicht.h>
#include <string>
#include <fstream>

//Contains variables managing each key press
struct keyConfig
{
	irr::EKEY_CODE DISPLAY_SHOOT_AREA;
	irr::EKEY_CODE FOLLOW_PATH;
	irr::EKEY_CODE END_TURN;
	irr::EKEY_CODE MENU;

	//camera controls
	irr::EKEY_CODE MOVE_CAMERA_UP;
	irr::EKEY_CODE MOVE_CAMERA_DOWN;
	irr::EKEY_CODE MOVE_CAMERA_LEFT;
	irr::EKEY_CODE MOVE_CAMERA_RIGHT;

	
	//constructor to set default values
	keyConfig() : 
		DISPLAY_SHOOT_AREA(irr::KEY_KEY_S),
		MOVE_CAMERA_UP(irr::KEY_UP),
		MOVE_CAMERA_DOWN(irr::KEY_DOWN),
		MOVE_CAMERA_LEFT(irr::KEY_LEFT),
		MOVE_CAMERA_RIGHT(irr::KEY_RIGHT),
		FOLLOW_PATH(irr::KEY_SPACE),
		END_TURN(irr::KEY_RETURN),
		MENU(irr::KEY_ESCAPE) {}

	void loadFromFile(std::string fileDir)
	{
		std::ifstream file;
		file.open(fileDir);

		std::string temp = "";
		getline(file,temp);
		DISPLAY_SHOOT_AREA = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		MOVE_CAMERA_UP = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		MOVE_CAMERA_DOWN = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		MOVE_CAMERA_LEFT = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		MOVE_CAMERA_RIGHT = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		FOLLOW_PATH = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		END_TURN = (irr::EKEY_CODE)atoi( temp.c_str() );
		getline(file,temp);
		MENU = (irr::EKEY_CODE)atoi( temp.c_str() );

		file.close();
	}
};

class MyEventReceiver : public irr::IEventReceiver
{
public:
	keyConfig KeyConfig;

	// We'll create a struct to record info on the mouse state
    struct SMouseState
    {
		int MouseScroll;
        irr::core::position2di OldPosition;
        irr::core::position2di Position;
        bool LeftButtonDown, RightButtonDown;
        SMouseState() : LeftButtonDown(false), RightButtonDown(false), MouseScroll(0) { }
    } MouseState;

    // This is the one method that we have to implement
    virtual bool OnEvent(const irr::SEvent& event)
    {
		// Update mouse move
		MouseState.OldPosition.X = MouseState.Position.X;
		MouseState.OldPosition.Y = MouseState.Position.Y;

	    // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
                KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		// Remember the mouse state
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            switch(event.MouseInput.Event)
            {
            case irr::EMIE_LMOUSE_PRESSED_DOWN:
                MouseState.LeftButtonDown = true;
                break;

            case irr::EMIE_LMOUSE_LEFT_UP:
                MouseState.LeftButtonDown = false;
                break;

			case irr::EMIE_RMOUSE_PRESSED_DOWN:
				MouseState.RightButtonDown = true;
				break;

			case irr::EMIE_RMOUSE_LEFT_UP:
				MouseState.RightButtonDown = false;
				break;

            case irr::EMIE_MOUSE_MOVED:
                MouseState.Position.X = event.MouseInput.X;
                MouseState.Position.Y = event.MouseInput.Y;
                break;

			case irr::EMIE_MOUSE_WHEEL:
				MouseState.MouseScroll += (int)event.MouseInput.Wheel*5;
				break;

            default:
                break;
            }
        }

        return false;
    }

	int GetMouseMoveX() const
	{
		return MouseState.Position.X-MouseState.OldPosition.X;
	}

	int GetMouseMoveY() const
	{
		return MouseState.Position.Y-MouseState.OldPosition.Y;
	}

    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const
    {
            return KeyIsDown[keyCode];
    }

    const SMouseState & GetMouseState(void) const
    {
        return MouseState;
    }
        
    MyEventReceiver()
    {
            for (irr::u32 i=0; i<irr::KEY_KEY_CODES_COUNT; ++i)
                    KeyIsDown[i] = false;
    }

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
};

#endif