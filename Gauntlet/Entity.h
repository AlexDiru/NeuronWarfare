#ifndef ENTITY_H
#define ENTITY_H

#include <irrlicht.h>
#include <cmath>
#include "vector_enhancements.h"
#include "math_enhancements.h"
#include "irr_node_enhancements.h"
#include "irr_vector_enhancements.h"
#include "PathFinding.h"
#include "MyEventReceiver.h"
#include "EntityStats.h"
#include "MazeRenderer.h"
#include "BresenhamLineAlgorithm.h"

#include <boost\thread.hpp>

//Enum for the state that an AI Entity is in

namespace AI
{
	enum State
	{
		Patrol,
		Attack,
		Run,
		HealthSearch,
		SeekRouteRoundCover
	};
}

//This class

class Entity
{
public:
	Entity() {}
	Entity(irr::scene::ISceneManager *smgr, std::vector<std::string> map, irr::video::ITexture* Tx);
	void Construct(irr::scene::ISceneManager *smgr, std::vector<std::string> map, irr::video::ITexture* Tx);
	irr::scene::ISceneNode *Node; // Entity's object
	irr::video::ITexture *Texture; // Entity's texture

	/* * * * * * * */
	/* Aesthetics  */
	/* * * * * * * */
	irr::video::ITexture *Avatar;

	/* * * * * * * */
	/* Game Logic  */
	/* * * * * * * */
	void StartTurn();
	void EndTurn();
	bool IsDead;
	EntityStats Stats;
	void CheckIfDead()
	{
		if (Stats.CurrentHealth <= 0)
		{
			IsDead = true;
			Node->setVisible(false);
		}
		else if (IsDead) //revived
		{
			IsDead = false;
			Node->setVisible(true);
		}
	}

	/* * * * * * */
	/* Movement  */
	/* * * * * * */
	void Position(); //Updates the player's position using the X and Y variables, locks the player on the map grid
	void FollowPath( const std::vector<Entity*>& EntityList);
	irr::core::vector3d<float> GlobalPosition; // Position (world-wise)
	int X, Y; // Position (map-grid)
	void UpdateMovement(MyEventReceiver& Receiver); // Handles the player's movement as well as keypressing
	bool RightMouseFlag;

	/* * * * * * * */
	/* Pathfinding */
	/* * * * * * * */
	int CreatePathToDestination(const vector<Entity*>& ListOfAllEntities, irr::scene::ISceneManager* smgr, MyEventReceiver& Receiver, 
								 irr::core::vector3df Destination, irr::core::dimension2d<int> 
								 mapSize, const std::vector<std::string>& Map); // Function used to manage path creation - takes care of the mouse pressing and keyflagging
	void UpdatePathDisplayTimer();
	irr::video::ITexture *ReachableTile; // Texture used to represent whether a tile is reachable on the current turn
	irr::video::ITexture *UnreachableTile; // Texture used to represent whether a tile is unreachable on the current turn
	void ClearUpPath();
	void HidePath();
	void ShowPath();
	PathFinding PathFinder; // Class required for A* algorithm
	int NewPathDisplayTimer; // So paths can't be spammed (key flag)
	void UpdatePathCubeColour(); // Makes sure the blue/red colours of the path are correct

	/* * * * * */
	/* Firing  */
	/* * * * * */
	#pragma region Firing
	irr::video::ITexture *CellShootable; //Texture used to represent whether a tile can be shot
	irr::video::ITexture *CellShootableCover; //Texture used to represent whether a shootable cell is hidden by cover

	//Whether or not the cell can be shot
	bool CanShoot( const irr::core::vector3df& target, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel );
	
	void BADisplayAllCellsThatCanBeShot( const std::vector<std::string>& Map, irr::scene::ISceneManager *smgr )
	{
		//Get 2d coords of target
		//std::pair<int, int> Coord( target.Z/10, target.X/10 );

		BresenhamLineAlgo BLA(Map);
		std::vector<std::vector<int>> canShoot;

		int tfs = this->Stats.RealFiringDistance/10; //tile firing distance
		
		//set shoot array to default (all true)
		for (int i = 0; i < tfs*2+1; i++)
		{
			std::vector<int> row(tfs*2+1,1);
			canShoot.push_back(row);
		}

		BLA.CheckAllRange(canShoot,tfs,this->X,this->Y);

		//Display shootcubes
		for (int y = this->Y-tfs; y <= this->Y+tfs; y++)
			for (int x = this->X-tfs; x <= this->X+tfs; x++)
			{
				//boundary check
				if (y+tfs < 0 || x+tfs < 0 || x+tfs >= Map[0].size() || x+tfs >= Map.size())
					continue;
				if (canShoot[y+tfs-Y][x+tfs-X] != 0)
				{
					irr::scene::ISceneNode* cube = smgr->addCubeSceneNode(8);
					cube->setPosition( irr::core::vector3df(y*10, -3.25, x*10) );
					cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);
					cube->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);


					if (canShoot[y+tfs-Y][x+tfs-X] == 1) //Cover
						cube->setMaterialTexture(0,this->CellShootableCover);
					else
						cube->setMaterialTexture(0,this->CellShootable);

					ShootCubes.push_back(cube);
				}
			}
			
		ShootCubesBeingDisplayed = true;
	}

	//returns the collision point of the shot, returns NULL if no collision
	irr::core::vector3df* Entity::CanShoot( const irr::core::vector3df& target, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel, bool OVERLOAD_BLANK_VARIABLE );
	
	//displays all the cells the entity can shoot (without using threads)
	void DisplayAllCellsThatCanBeShot(irr::scene::ISceneManager *smgr, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel ); 

	//uses boost::threads to display the areas the player can shoot in a decent amount of time
	void DisplayAllCellsThatCanBeShotUsingThreads(irr::scene::ISceneManager *smgr, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel, irr::scene::IMetaTriangleSelector *coverTriSel ); 

	//deletes all shoot cubes and clears the vector
	void ClearUpShootCubes();

	bool ShootCubesBeingDisplayed;
	bool ShootDisplayKeyFlag;

	bool CoverHit(const irr::core::vector3df& target, irr::scene::ISceneCollisionManager* collMan, irr::scene::IMetaTriangleSelector *triSel, const irr::core::vector3df& coverPos, int coverOX, int coverOY);
	#pragma endregion

	/* * * */
	/* AI  */
	/* * * */

	int AI_DestinationX;
	int AI_DestinationY;
	AI::State AI_State;
	bool isAI;
	int AI_TargetIndex;
	void AI_FollowPathWholeTurn(const std::vector<Entity*>& ListOfAllEntities);
	void AI_MakePath(int targetX, int targetY, irr::scene::ISceneManager *smgr, MyEventReceiver& Receiver, irr::core::dimension2d<irr::s32> mapSize, const std::vector<std::string>& Map, const std::vector<Entity*>& ListOfAllEntities);
	

	void AI_AutomateDestination(const vector<Entity*>& ListOfAllEntities, irr::scene::ISceneManager* smgr, MyEventReceiver& Receiver, 
								 int targetX, int targetY, irr::core::dimension2d<int> 
								 mapSize, const std::vector<std::string>& Map);
	void AI_SetRandomDestination();

protected:
	/* * * * * * */
	/* Movement  */
	/* * * * * * */
	bool KeyFlag; //stop movement spamming
	
	/* * * * * * * */
	/* Pathfinding */
	/* * * * * * * */
	void CreatePath(irr::scene::ISceneManager* smgr, const std::vector<std::string>& map, 
					int StartX, int StartY, int EndX, int EndY, int XSize, int YSize); // Creates a path from a position to a target using A*
	std::vector<irr::scene::ISceneNode*> PathCubes; // Contains the cubes that are used to display the path

	/* * * * * */
	/* Firing  */
	/* * * * * */
	boost::thread ST1, ST2, ST3, ST4, ST5, ST6, ST7, ST8;
	//MAKE SURE check.x1 < check.x2 and check.y1 < check.y2 OR EVERYTHING GOES TITS UP - break
	//allows for multithreading otherwise algorithm is too slow
	void PerformShootRaycasts( irr::scene::ISceneManager *smgr, irr::scene::IMetaTriangleSelector *triSel, 
							   irr::core::vector2d<int> P, const irr::core::rect<int>& check, bool* signal, 
							   irr::scene::IMetaTriangleSelector *coverTriSel ); 
	std::vector<irr::scene::ISceneNode*> ShootCubes;

	/* * * */
	/* AI  */
	/* * * */
	void AI_FollowPath(std::vector<Entity*> EntityList);
	

};

#endif