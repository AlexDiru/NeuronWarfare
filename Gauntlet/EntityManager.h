#pragma once

#include <irrlicht.h>
#include "MyEventReceiver.h"
#include "MazeRenderer.h"
#include "AIFireAtTarget.h"

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
	
	int Size;
	int CurrentPlayer;


	//Does all the entity's (that belong to this manager) turns
	void AI_StartTurn(const std::vector<Entity*>& EntityList, EntityManager& PlayerManager, irr::scene::ISceneManager *smgr, MyEventReceiver& Receiver, 
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

private:
	Entity *ePtr;
};