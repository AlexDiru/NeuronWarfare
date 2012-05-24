#pragma once

#include <irrlicht.h>
#include "MyEventReceiver.h"
#include "MazeRenderer.h"
#include "AIFireAtTarget.h"
#include "AIState.h"

class Entity;

class EntityManager
{
public:
	//stores a pointer to the first entity of the array and also the size
	EntityManager(int sz) 
		: Size(sz), CurrentPlayer(0) 
	{
		ePtr = new Entity[sz];
	};

	~EntityManager() //clear up memory
	{
		delete[] ePtr;
	}

	
	Entity* GetEntityThatResidesOnCell( const irr::core::position2di Cell );

	//switches characters (AI won't need this)
	void ManageCharacterSwitching( const MyEventReceiver& Receiver);

	//cycles through every linked entity ending their turns
	void EndTurn();

	//cycles through every linked entity starting their turns
	void StartTurn();

	//returns a pointer to the player's character in use (AI won't need this)
	Entity* GetCurrentEntity() { return (ePtr + CurrentPlayer); }

	//gets a specified entity
	Entity* GetEntity(int Index) { return (ePtr + Index); }

	//gets a list of all the entities contained
	std::vector<Entity*> GetEntityList();

	//clears up all data for a character switch
	void ForceCharacterSwitch( int oldChar, int newChar );

	int Size;
	int CurrentPlayer;


	//Does all the entity's (that belong to this manager) turns
	void AI_StartTurn(const std::vector<Entity*>& EntityList, EntityManager& PlayerManager, irr::scene::ISceneManager *smgr, MyEventReceiver& Receiver, 
		irr::core::dimension2di mapSize, const std::vector<std::string>& Map, irr::scene::IMetaTriangleSelector *walltrisel, irr::scene::IMetaTriangleSelector *coverTriSel);

private:
	Entity *ePtr;
};