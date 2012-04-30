#include "EntityManager.h"

Entity* EntityManager::GetEntityThatResidesOnCell( const irr::core::position2di Cell )
{
	//cycle through all the entities
	for (int i = 0; i < this->Size; i++)
		if ( this->GetEntity(i)->X == Cell.Y )
			if ( this->GetEntity(i)->Y == Cell.X ) 
				return this->GetEntity(i);
		
	return NULL; //nothing found
}

void EntityManager::ManageCharacterSwitching( const MyEventReceiver& Receiver)
{
	//get the original current player, required to clear up the ocp's paths if there
	//is a character change
	int OCP = CurrentPlayer;

	if      (Receiver.IsKeyDown(irr::KEY_KEY_1) && Size > 0 && !ePtr->IsDead     ) CurrentPlayer = 0;
	else if (Receiver.IsKeyDown(irr::KEY_KEY_2) && Size > 1 && !(ePtr+1)->IsDead ) CurrentPlayer = 1;
	else if (Receiver.IsKeyDown(irr::KEY_KEY_3) && Size > 2 && !(ePtr+2)->IsDead ) CurrentPlayer = 2;
	else if (Receiver.IsKeyDown(irr::KEY_KEY_4) && Size > 3 && !(ePtr+3)->IsDead ) CurrentPlayer = 3;

	if (CurrentPlayer != OCP) //character switched so clear the previous characters stuff up
	{
		(ePtr + OCP)->ClearUpShootCubes(); //clear up cubes

		//Hide the old player's pathcubes
		(ePtr + OCP)->HidePath();

		//Show the new player's pathcubes
		(ePtr + CurrentPlayer)->ShowPath();
	}
}


void EntityManager::EndTurn()
{
	for (int i = 0; i < Size; i++)
		(ePtr+i)->EndTurn();
}

std::vector<Entity*> EntityManager::GetEntityList()
{
	std::vector<Entity*> rv;
	for (int i=0; i < this->Size; i++)
		rv.push_back( (ePtr + i) );
	return rv;
}

void EntityManager::StartTurn()
{
	for (int i = 0; i < Size; i++)
	{
		(ePtr + i)->StartTurn();

		//show the current player's path
		(ePtr + CurrentPlayer)->ShowPath();
	}
}