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
		ForceCharacterSwitch(OCP, CurrentPlayer);
	}
}

void EntityManager::ForceCharacterSwitch( int oldChar, int newChar )
{
	(ePtr + oldChar)->ClearUpShootCubes(); //clear up cubes

	//Hide the old player's pathcubes
	(ePtr + oldChar)->HidePath();

	//Show the new player's pathcubes
	(ePtr + newChar)->ShowPath();

	CurrentPlayer = newChar;
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

		//if the current player is dead, find a non-dead entity
		if ((ePtr + CurrentPlayer)->IsDead)
		{
			int oc = CurrentPlayer; //old character
			CurrentPlayer = -1;
			for (int i = 0; i < Size; i++)
			{
				if (!(ePtr + i)->IsDead)
				{
					ForceCharacterSwitch( oc, i );
					break;
				}
			}

			//alive player not found, therefore player loses
			if (CurrentPlayer == -1)
			{
				//lose game
				int lose = 23423;
			}
		}

		//show the current player's path
		(ePtr + CurrentPlayer)->ShowPath();
	}
}

void EntityManager::AI_StartTurn(const std::vector<Entity*>& EntityList, EntityManager& PlayerManager, irr::scene::ISceneManager *smgr, MyEventReceiver& Receiver, 
		irr::core::dimension2di mapSize, const std::vector<std::string>& Map, irr::scene::IMetaTriangleSelector *walltrisel, irr::scene::IMetaTriangleSelector *coverTriSel)
{
	for (int i = 0; i < Size; i++)
	{
		(ePtr + i)->CheckIfDead();
		if (!(ePtr + i)->IsDead)
		{
			/* * * * * * * * */
			/* Patrol State  */
			/* * * * * * * * */
			if ((ePtr + i)->AI_State == AI::Patrol)
			{
				//check if any enemies are nearby
				std::vector<int> ListOfShootableEntities; //index of shootable entities

				for (int p = 0; p < PlayerManager.Size; p++)
				{
					if (!PlayerManager.GetEntity(p)->IsDead) //target cannot be dead
					{
						int maxDist = (ePtr+i)->Stats.RealFiringDistance/5; //twice firing dist
						float xDist = (PlayerManager.GetEntity(p)->X - (ePtr+i)->X)*(PlayerManager.GetEntity(p)->X - (ePtr+i)->X);
						float yDist = (PlayerManager.GetEntity(p)->Y - (ePtr+i)->Y)*(PlayerManager.GetEntity(p)->Y - (ePtr+i)->Y);

						if ( xDist + yDist < maxDist*maxDist) 
							//we need to also check path distance
							//close enemy
							ListOfShootableEntities.push_back(p);
					}
				}

				std::pair<int, int> PathSize(-1,1000000); //First = target index, Second = path length
			
				for (int p = 0; p < ListOfShootableEntities.size(); p++)
				{
					// Make AI attack the player with the smallest path size
					(ePtr + i)->AI_MakePath(PlayerManager.GetEntity(ListOfShootableEntities[p])->X, 
											PlayerManager.GetEntity(ListOfShootableEntities[p])->Y, smgr, Receiver, mapSize, Map, EntityList);

					int pathSize = (ePtr + i)->PathFinder.m_pathToGoal.size();

					if (pathSize < PathSize.second) //smaller than smallest path size
					{
						PathSize.second = pathSize;
						PathSize.first = p;
					}
				}

				if (PathSize.first != -1) //path found
				{
					//do health check
					if ( (ePtr + i)->Stats.CurrentHealth < 0.4*(ePtr +i)->Stats.MaxHealth)
						(ePtr + i)->AI_State = AI::HealthSearch;
					else
					{
						(ePtr + i)->AI_State = AI::Attack;
						//pick a random enemy to attack
						(ePtr + i)->AI_TargetIndex = ListOfShootableEntities[ PathSize.first ];
					}
				}
				else
				{
					(ePtr + i)->AI_AutomateDestination(EntityList,smgr,Receiver, (ePtr + i)->AI_DestinationX, (ePtr + i)->AI_DestinationY,
														mapSize, Map);
					(ePtr + i)->Position();
				}
			}
			
			/* * * * * * * * */
			/* Attack State  */
			/* * * * * * * * */
			if ((ePtr + i)->AI_State == AI::Attack)
			{
				//if can shoot enemy, shoot
				if ((ePtr + i)->CanShoot( PlayerManager.GetEntity( (ePtr+i)->AI_TargetIndex )->Node->getPosition(), smgr->getSceneCollisionManager(), walltrisel) )
				{
					//shoot player
					AIFireAtTarget(ePtr + i, PlayerManager.GetEntity( (ePtr+i)->AI_TargetIndex ), smgr, coverTriSel);

					//if target is dead, go back to move state
					if (PlayerManager.GetEntity( (ePtr+i)->AI_TargetIndex )->IsDead)
						(ePtr + i)->AI_State = AI::Patrol;
				}
				//else move closer
				else
				{
					//set target to enemy pos
					(ePtr + i)->AI_DestinationX = PlayerManager.GetEntity((ePtr+i)->AI_TargetIndex)->X;
					(ePtr + i)->AI_DestinationY = PlayerManager.GetEntity((ePtr+i)->AI_TargetIndex)->Y;
					
					(ePtr + i)->AI_MakePath((ePtr + i)->AI_DestinationX,(ePtr + i)->AI_DestinationY,smgr,Receiver,mapSize,Map,EntityList);
					(ePtr + i)->AI_FollowPathWholeTurn(EntityList);
					(ePtr + i)->Position();
				}
			}

				
			/* * * * * * * * */
			/* Health State  */
			/* * * * * * * * */
			if ((ePtr + i)->AI_State == AI::HealthSearch)
			{
			}

			(ePtr + i)->Stats.StepsMovedThisTurn = 0;
		}
			
		(ePtr + i)->CheckIfDead(); // final check
	}
}