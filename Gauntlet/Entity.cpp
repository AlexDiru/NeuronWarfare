#include "Entity.h"

Entity::Entity(irr::scene::ISceneManager *smgr, std::vector<std::string> map, irr::video::ITexture* Tx)
{
	Construct(smgr,map,Tx);
}

void Entity::Construct(irr::scene::ISceneManager *smgr, std::vector<std::string> map, irr::video::ITexture* Tx)
{
	//default position
	X = 0; Y = 0;
	
	//setup object
	this->Node = smgr->addSphereSceneNode(3.f);
	this->GlobalPosition.set( irr::core::vector3df( 0.f, 0.f, 0.f) );
	Node->setPosition( GlobalPosition );
	Node->setMaterialTexture(0,Tx);
	Node->setMaterialFlag(irr::video::EMF_LIGHTING, false);

	//setup pathfinder
	PathFinder.UnwalkableTiles = "#C";
	PathFinder.Map = map;
	PathFinder.YSize = map.size();
	if (map.size() > 0) 
		PathFinder.XSize = map[0].size();

	Stats.StepsMovedThisTurn = 0;

	NewPathDisplayTimer = 0;

	IsDead = false;

	isAI = false;
}

void Entity::Position()
{
	//position the obejct but lock it in relation to the map grid
	this->Node->setPosition( irr::core::vector3df( Y*10, 1.5, X*10) );
}

void Entity::CreatePath(irr::scene::ISceneManager* smgr, const std::vector<std::string>& map, int StartX, int StartY, int EndX, int EndY, int XSize, int YSize)
{
	ClearUpShootCubes(); //so the path isn't hidden by the shootcubes
	
	//find path
	PathFinder.m_intializedStartGoal = false;
	PathFinder.m_foundGoal = false;
	while (!PathFinder.m_foundGoal)
		PathFinder.FindPath(Vector3(StartX, 0, StartY), Vector3(EndX, 0, EndY));

	//remove previous path cubes
	ClearUpPath();

	//create new path cubes
	for (int i = 0; i < PathFinder.m_pathToGoal.size()-1; i++)
	{
		//display path
		irr::scene::ISceneNode* cube = smgr->addCubeSceneNode(8);
		cube->setPosition( irr::core::vector3df( PathFinder.m_pathToGoal[i]->z*10, -3.25, PathFinder.m_pathToGoal[i]->x*10) );
		cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		cube->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);

		// Set the (un)reachable texture of the cube
		if (PathFinder.m_pathToGoal.size()  - i < (this->Stats.MovementSteps+2 - this->Stats.StepsMovedThisTurn)) //walkable
			cube->setMaterialTexture(0,this->ReachableTile);
		else //unreachable
			cube->setMaterialTexture(0,this->UnreachableTile);

		PathCubes.push_back(cube);
	}
}

void Entity::FollowPath(const std::vector<Entity*>& EntityList)
{
	//when following a path, the node will chomp the next path node (erase it) and then move to it's position
	if (this->PathCubes.size() > 0 && this->Stats.StepsMovedThisTurn < this->Stats.MovementSteps)
	{
		int destinationX = (int)(this->PathCubes[PathCubes.size()-1]->getPosition().Z)/10;
		int destinationY = (int)(this->PathCubes[PathCubes.size()-1]->getPosition().X)/10;

		//make sure it's impossible of LANDING on an entity
		//if next cube has an entity
		//keep checking next nodes until free
		int nodeAhead = 0, prevNodeAhead;
		do
		{
			//update destination
			destinationX = (int)(this->PathCubes[PathCubes.size()-1-nodeAhead]->getPosition().Z)/10;
			destinationY = (int)(this->PathCubes[PathCubes.size()-1-nodeAhead]->getPosition().X)/10;

			prevNodeAhead = nodeAhead;
			for (int i = 0; i < EntityList.size(); i++)
				if ( EntityList[i]->X == destinationX )
					if (EntityList[i]->Y == destinationY )
						//entity on node
					{
						if (nodeAhead < PathFinder.m_pathToGoal.size()-1)
							nodeAhead++; //look another node ahead
						else break;
					}
					if (nodeAhead >= this->Stats.MovementSteps - this->Stats.StepsMovedThisTurn)
						return;
		} while (prevNodeAhead != nodeAhead && nodeAhead < PathFinder.m_pathToGoal.size()-1);


		//'chomp' next node
		this->X = destinationX;
		this->Y = destinationY;

		//chomp the required amount of nodes
		for (int i = 0; i <= prevNodeAhead && !PathCubes.empty(); i++)
		{
			PathCubes[PathCubes.size()-1]->remove();
			PathCubes.erase(PathCubes.begin() + PathCubes.size()-1);
		}

		this->Stats.StepsMovedThisTurn += nodeAhead + 1;

		ClearUpShootCubes();
	}
}

void Entity::EndTurn()
{ 
	Stats.StepsMovedThisTurn = 0; 

	HidePath();

	ClearUpShootCubes();
}

void Entity::StartTurn()
{
	UpdatePathCubeColour();
}

bool Entity::CanShoot( const irr::core::vector3df& target, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel )
{
	irr::core::line3d<irr::f32> Ray (this->Node->getPosition(),
										irr::core::vector3df(target.X, this->Node->getPosition().Y, target.Z));

	//length check
	if (Ray.getLengthSQ() > Stats.RealFiringDistance*Stats.RealFiringDistance)
		return false;

	irr::core::vector3df point;
	irr::core::triangle3df tri;
	const irr::scene::ISceneNode *node = NULL;

	return !(wallCollMan->getCollisionPoint(Ray,triSel,point,tri,node)); //intersection
}

void Entity::DisplayAllCellsThatCanBeShot (irr::scene::ISceneManager *smgr, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel )
{
	// slow method

	//clear cur shootcubes
	while (!ShootCubes.empty())
	{
		ShootCubes[0]->remove();
		ShootCubes.erase(ShootCubes.begin());
	}

	//spiral around player starting at the outer edges
	int level = ceil((double)Stats.RealFiringDistance/10);
	int xo = level; //x-offset
	int yo = level; //y-offset

	do
	{
		float x =  (Y - xo)*10;
		float y = 1.5;
		float z =  (X - yo)*10;
		irr::core::vector3df curTar = irr::core::vector3df( x, y, z );

		irr::core::vector3df debug = Node->getPosition();

		if (CanShoot( curTar, wallCollMan, triSel ) )
		{
			//add to shootcubes
			//display path
			irr::scene::ISceneNode* cube = smgr->addCubeSceneNode(8);
			cube->setPosition( curTar );
			cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);
			cube->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);

			cube->setMaterialTexture(0,this->ReachableTile);

			ShootCubes.push_back(cube);
		}

		xo--;
		if (xo == -level)
		{
			xo = level;
			yo--;
		}

	} while (yo >= -level); //while not beginning corner reached
}

void Entity::PerformShootRaycasts( irr::scene::ISceneManager *smgr, irr::scene::IMetaTriangleSelector *triSel, irr::core::vector2d<int> P, const irr::core::rect<int>& check, bool* signal, irr::scene::IMetaTriangleSelector *coverTriSel )
{
	int xo = check.LowerRightCorner.X; //x-offset
	int yo = check.LowerRightCorner.Y; //y-offset

	do
	{
		//get the positions that will be tested to see if they can be shot
		float x =  (P.Y - xo)*10;
		float y = 1.5;
		float z =  (P.X - yo)*10;
		irr::core::vector3df curTar = irr::core::vector3df( x, y, z );

		//if not on player's position
		if (!(!xo && !yo))
			//if the cell can be shot
			if (CanShoot( curTar, smgr->getSceneCollisionManager(), triSel ) )
			{
				//add to shootcubes
				irr::scene::ISceneNode* cube = smgr->addCubeSceneNode(8);
				cube->setPosition( irr::core::vector3df(curTar.X, -3.25, curTar.Z) );
				cube->setMaterialFlag(irr::video::EMF_LIGHTING, false);
				cube->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);

				//check if hidden by cover
				bool coverHit = false;

				//pointer to make code more readable
				std::vector<std::string> *m = &this->PathFinder.Map;

				irr::core::position2di Target( (int)(curTar.Z/10), (int)(curTar.X/10) );

				//have to look at each individually as multiple covers can be surrounding a cell........
				bool covered = false;
				for (int y = -1; y < 2 && !covered; y++)
					for (int x = -1; x < 2 && !covered; x++)
						if (!(y == 0 && x == 0)) //not middle
							if ((*m)[Target.Y+y][Target.X+x] == 'C')
								if (CoverHit(curTar,smgr->getSceneCollisionManager(),coverTriSel,irr::core::vector3df(Target.Y*10,5,Target.X*10),x,y))
								{
									cube->setMaterialTexture(0,CellShootableCover);
									covered = true;
								}
				if (!covered)
					cube->setMaterialTexture(0,CellShootable);

				ShootCubes.push_back(cube);
			}

		//traverse left
		xo--;
		if (xo == check.UpperLeftCorner.X)
		{
			//traverse up and move xoffset back to the right
			xo = check.LowerRightCorner.X;
			yo--;
		}

	} while (yo >= (int)check.UpperLeftCorner.Y); //while not above topleft corner reached

	*signal = true;
}

void Entity::DisplayAllCellsThatCanBeShotUsingThreads(irr::scene::ISceneManager *smgr, irr::scene::ISceneCollisionManager* wallCollMan, irr::scene::IMetaTriangleSelector *triSel, irr::scene::IMetaTriangleSelector *coverTriSel )
{
	//clear our shootcubes
	ClearUpShootCubes();

	//clear up pathcubes as they get in the way and make the shoot squares look annoying
	ClearUpPath();

	int level = ceil((double)Stats.RealFiringDistance/10);

	//Setup 4 threads to do the job
	std::vector<int> x1,x2,y1,y2;

	/*****************************
	*              *             *
	*   thread 1   *   thread 3  *
	*              *             *
	*              *             *
	******************************
	*              *             *
	*   thread 2   *   thread 4  *
	*              *             *
	*              *             *
	*****************************/

	//thread 1 - topleft
	x1.push_back(-level);
	y1.push_back(-level);
	x2.push_back(0);
	y2.push_back(-1);

	//thread 2 - btmleft
	x1.push_back(-level);
	y1.push_back(1-1);
	x2.push_back(0);
	y2.push_back(level);

	//thread 3 - topright
	x1.push_back(0);
	y1.push_back(-level);
	x2.push_back(level);
	y2.push_back(-1);

	//thread 4 - btmright
	x1.push_back(0);//
	y1.push_back(0);
	x2.push_back(level);
	y2.push_back(level);

	//signal from when the threads are complete
	bool comp1 = false, comp2 = false, comp3 = false, comp4 = false;
	
	//create and join threads
	//if t1.create, t2. create, t1.join, t2.join then EXCEPTION
	ST1 = boost::thread( &Entity::PerformShootRaycasts, this, smgr, triSel, irr::core::vector2d<int>(X,Y), irr::core::rect<irr::s32>(x1[0],y1[0],x2[0],y2[0]),&comp1,coverTriSel);
	ST1.join();
	ST2 = boost::thread( &Entity::PerformShootRaycasts, this, smgr, triSel, irr::core::vector2d<int>(X,Y), irr::core::rect<irr::s32>(x1[1],y1[1],x2[1],y2[1]),&comp2,coverTriSel);
	ST2.join();
	ST3 = boost::thread( &Entity::PerformShootRaycasts, this, smgr, triSel, irr::core::vector2d<int>(X,Y), irr::core::rect<irr::s32>(x1[2],y1[2],x2[2],y2[2]),&comp3,coverTriSel);
	ST3.join();
	ST4 = boost::thread( &Entity::PerformShootRaycasts, this, smgr, triSel, irr::core::vector2d<int>(X,Y), irr::core::rect<irr::s32>(x1[3],y1[3],x2[3],y2[3]),&comp4,coverTriSel);
	ST4.join();
		
	//wait for threads to complete
	while (!comp1 || !comp2 || !comp3 || !comp4); 

	//done with threads
	ST1.yield();
	ST2.yield();
	ST3.yield();
	ST4.yield();

	ShootCubesBeingDisplayed = true;
}

void Entity::UpdateMovement(MyEventReceiver& Receiver)
{
	if (!KeyFlag) //stop movement spamming (wait for user to depress key before registering response)
	{
		//Move forward
		if (Receiver.IsKeyDown(irr::KEY_KEY_W))
		{
			KeyFlag = true;
			Y++;
		}
		
		//Move backward
		if (Receiver.IsKeyDown(irr::KEY_KEY_S))
		{
			Y--;
			KeyFlag = true;
		}

		//Move left
		if (Receiver.IsKeyDown(irr::KEY_KEY_D))
		{
			KeyFlag = true;
			X--;
		}
		
		//Move right
		if (Receiver.IsKeyDown(irr::KEY_KEY_A))
		{
			KeyFlag = true;
			X++;
		}
	}

	if (KeyFlag && Receiver.IsKeyDown(irr::KEY_KEY_W) +
				   Receiver.IsKeyDown(irr::KEY_KEY_S) +
				   Receiver.IsKeyDown(irr::KEY_KEY_A) + 
				   Receiver.IsKeyDown(irr::KEY_KEY_D)   < 1 )
			KeyFlag = false;
}

int Entity::CreatePathToDestination(const vector<Entity*>& ListOfAllEntities, irr::scene::ISceneManager* smgr, MyEventReceiver& Receiver, irr::core::vector3df Destination, irr::core::dimension2d<int> mapSize, const std::vector<std::string>& Map)
{
	int FUCK_YOU_COMPILER_STOP_OPTIMISING_THIS_FUNCTION_OUT = 0;

	
	//get the start and goal positions 
	int px = X;
	int py = Y;
	int rx = Destination.Z/10;
	int ry = Destination.X/10;

	if (rx > -1 && rx < mapSize.Height && ry > -1 && ry < mapSize.Width) //boundary check
	{
		//make sure destination isn't solid or outside of map, else the whole map will be checked (slow for large maps)
		for (int i = 0; i < PathFinder.UnwalkableTiles.length(); i++)
			if (Map[ry][rx] == PathFinder.UnwalkableTiles[i])
				return 0;
		if (Map[ry][rx] == ' ')
			return 0;

		//Make sure no other entities on the destination square
		for (int i = 0; i < ListOfAllEntities.size(); i++)
			if (rx == ListOfAllEntities[i]->X)
				if (ry == ListOfAllEntities[i]->Y)
					if (!isAI)//and if not an AI
						return 0;

		//Create path
		CreatePath(smgr,Map, px, py, rx, ry, mapSize.Width, mapSize.Height);
		NewPathDisplayTimer = 1;
			
	}
	FUCK_YOU_COMPILER_STOP_OPTIMISING_THIS_FUNCTION_OUT = 1;


	return 1;
}

bool Entity::CoverHit(const irr::core::vector3df& target, irr::scene::ISceneCollisionManager* collMan, irr::scene::IMetaTriangleSelector *triSel, const irr::core::vector3df& coverPos, int coverOX, int coverOY)
{
	//Parameters
	// Target - position of the cell being checked
	// CollMan - any collision manager
	// triSel - the triangle selector containing all the cover nodes
	// coverPos - the position of the cover being checked
	// coverOX - X offset of the cover from the cell, 1, 0 or -1
	// coverOY - Y offset of the cover from the cell, 1, 0 or -1

	//cover needs to be specified otherwise the bug occurs (try to remember alex...)
	/*
	x = shootable (cover)
	o = shootable
	P = player
	C = cover


		xxxooxCooP
		C
	
	the left row of cells shouldn't be hidden by cover but they are */

	///rant
	///this algorithm was horrible to code
	///fuck linear algebra
	///i guess it's proved uni's helped...

	//get the start of the raycast
	irr::core::vector3df start(Node->getPosition().X, 5 ,Node->getPosition().Z);

	//counter to stop infinite loops
	int inf_loop_count = 0;

	//keep looping as there can be cover behind each other
	while (inf_loop_count < this->Stats.RealFiringDistance/10) 
	{
		inf_loop_count++;

		//the raycast line
		irr::core::line3df ray( start, 
								irr::core::vector3df(target.X, 5 , target.Z) );
		
		//ray cast returns (redundant)
		irr::core::vector3df point;
		irr::core::triangle3df tri;
		const irr::scene::ISceneNode *node = NULL;

		//the raycast intersection, nothing intersected return
		if (!collMan->getCollisionPoint(ray,triSel,point,tri,node)) 
			return false;

		//convert the vectors to 2d coords
		irr::core::vector3df pt1 = vftovi(coverPos);
		irr::core::vector3df pt2 = vftovi(point);
		pt2 = irr::core::vector3df( pt2.X-coverOY*10, pt1.Y, pt2.Z-coverOX*10);

		if ( pt1 == pt2)
			return true;
		else if ( target.getDistanceFromSQ(irr::core::vector3df(Node->getPosition().X, 5 ,Node->getPosition().Z)) < point.getDistanceFromSQ(irr::core::vector3df(Node->getPosition().X, 5 ,Node->getPosition().Z)))
			return false; //gone too far no need to go further
		else //keep raycasting as there may be cover behind
		{
			start = point + 0.00001*ray.getVector(); //have to add a little bit more onto the vector to stop the same point being intersected
			//i can imagine this statement breaking down when higher firing distances are reached, though i doubt ten times the size will even be a problem - and really, a firing distance of 1000 will be so SO slow
		}
	}
}

void Entity::AI_AutomateDestination(const vector<Entity*>& ListOfAllEntities, irr::scene::ISceneManager* smgr, MyEventReceiver& Receiver, 
								 int targetX, int targetY, irr::core::dimension2d<int> 
								 mapSize, const std::vector<std::string>& Map)
{
	//if reached destination set new destination
	if (!((!(X == AI_DestinationX && Y == AI_DestinationY) && PathFinder.m_pathToGoal.size() > 0)))
	{
		RetryPathCreation:
		AI_SetRandomDestination();
		if (this->CreatePathToDestination(ListOfAllEntities, smgr, Receiver, irr::core::vector3df( AI_DestinationY*10, 0, AI_DestinationX*10 ), mapSize, Map) == 0)
		{
			goto RetryPathCreation;
		}
		if (PathFinder.m_pathToGoal.size() == 0)
			goto RetryPathCreation;

		//set all pathcubes to invisible
		for (int i = 0; i < PathCubes.size(); i++)
		{
			PathCubes[i]->setVisible(false);
		}

		PathFinder.m_pathToGoal.erase( PathFinder.m_pathToGoal.end()-1 );
	}

	//if haven't reached destination
	
	//follow path
	//will be blocked by entities in the way

	//use the index to stop an infinite loop that would be caused by the above 'bug' but lets call it a feature
	//nah in future have to change the path destination if the entity is blocked
	AI_FollowPathWholeTurn(ListOfAllEntities);
}

void Entity::AI_SetRandomDestination()
{
	while (1)
	{
		int xSize = PathFinder.Map[0].size();
		int ySize = PathFinder.Map.size();
		int X = rand()%xSize;
		int Y = rand()%ySize;

		if (PathFinder.Map[Y][X] == '.' && !(this->Y == Y && this->X == X))
		{
			AI_DestinationX = X;

			AI_DestinationY = Y;
			return;
		}
	}
}

void Entity::AI_FollowPath(std::vector<Entity*> EntityList)
{
	//when following a path, the node will chomp the next path node (erase it) and then move to it's position
	if (this->PathFinder.m_pathToGoal.size() > 0 && this->Stats.StepsMovedThisTurn < this->Stats.MovementSteps)
	{
		int destinationX = this->PathFinder.m_pathToGoal[PathFinder.m_pathToGoal.size()-1]->x;
		int destinationY = this->PathFinder.m_pathToGoal[PathFinder.m_pathToGoal.size()-1]->z;

		//make sure it's impossible of LANDING on an entity
		//if next cube has an entity
		//keep checking next nodes until free
		int nodeAhead = 0, prevNodeAhead;
		do
		{
			//update destination
			destinationX = this->PathFinder.m_pathToGoal[PathFinder.m_pathToGoal.size()-1-nodeAhead]->x;
			destinationY = this->PathFinder.m_pathToGoal[PathFinder.m_pathToGoal.size()-1-nodeAhead]->z;


			prevNodeAhead = nodeAhead;
			for (int i = 0; i < EntityList.size(); i++)
				if ( EntityList[i]->X == destinationX )
					if (EntityList[i]->Y == destinationY )
						//entity on node
					{
						if (nodeAhead < PathFinder.m_pathToGoal.size()-1)
							nodeAhead++; //look another node ahead
						else break;
					}
					if (nodeAhead >= this->Stats.MovementSteps - this->Stats.StepsMovedThisTurn)
						return;
		} while (prevNodeAhead != nodeAhead && nodeAhead < PathFinder.m_pathToGoal.size()-1);

		//'chomp' next node
		this->X = destinationX;
		this->Y = destinationY;

		//chomp the required amount of nodes
		if (PathFinder.m_pathToGoal.size() > 0)
			for (int i = 0; i <= prevNodeAhead; i++)
			{
				PathFinder.m_pathToGoal.erase(PathFinder.m_pathToGoal.begin()  + PathFinder.m_pathToGoal.size()-1);
			}

		this->Stats.StepsMovedThisTurn += prevNodeAhead + 1;
	}
}

void Entity::AI_MakePath(int targetX, int targetY, irr::scene::ISceneManager *smgr, MyEventReceiver& Receiver, irr::core::dimension2d<irr::s32> mapSize, const std::vector<std::string>& Map, const std::vector<Entity*>& ListOfAllEntities)
{
	CreatePathToDestination(ListOfAllEntities, smgr, Receiver, irr::core::vector3df( targetY*10, 0, targetX*10 ), mapSize, Map);
		
	//set all pathcubes to invisible
	for (unsigned int i = 0; i < PathCubes.size(); i++)
		PathCubes[i]->setVisible(false);

	if (PathFinder.m_pathToGoal.size() > 0)
		PathFinder.m_pathToGoal.erase( PathFinder.m_pathToGoal.end()-1 );

	if (PathFinder.m_pathToGoal.size() > 0)
		PathFinder.m_pathToGoal.erase( PathFinder.m_pathToGoal.begin() );
}

void Entity::AI_FollowPathWholeTurn(const std::vector<Entity*>& ListOfAllEntities)
{
	int steps = 0;
	while (steps++ < this->Stats.MovementSteps)
		AI_FollowPath(ListOfAllEntities);
}

void Entity::UpdatePathCubeColour()
{
	for (int i = PathCubes.size() - 1; i >= 0 ; i--)
	{
		if (PathCubes.size()  - i < (this->Stats.MovementSteps+1 - this->Stats.StepsMovedThisTurn)) //walkable
			PathCubes[i]->setMaterialTexture(0,this->ReachableTile);
		else //unreachable
			PathCubes[i]->setMaterialTexture(0,this->UnreachableTile);
	}
}

void Entity::ClearUpPath() 
{ 
	while (!PathCubes.empty()) 
	{ 
		PathCubes[0]->remove(); 
		PathCubes.erase(PathCubes.begin()); 
	} 
}

void Entity::HidePath() 
{ 
	for (int i = 0; i < PathCubes.size(); i++) 
	{ 
		PathCubes[i]->setVisible(false); 
	} 
}

void Entity::ShowPath() 
{ 
	for (int i = 0; i < PathCubes.size(); i++) 
	{ 
		PathCubes[i]->setVisible(true); 
	} 
}

void Entity::ClearUpShootCubes() 
{ 
	ShootCubesBeingDisplayed = false; 
	while (!ShootCubes.empty()) 
	{ 
		ShootCubes[0]->remove(); 
		ShootCubes.erase(ShootCubes.begin()); 
	} 
}

void Entity::UpdatePathDisplayTimer() 
{ 
	if (NewPathDisplayTimer > 0) 
		NewPathDisplayTimer--; 
}