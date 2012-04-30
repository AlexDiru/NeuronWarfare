// irrGauntlet
#include "MazeGenerator.h"
#include "MazeRenderer.h"
#include "Entity.h"
#include "EntityManager.h"
#include "MyEventReceiver.h"
#include "GameCamera.h"
#include <irrlicht.h>
#include <string>
#include "string_enhancements.h"
#include "irr_vector_enhancements.h"
#include "hud.h"
#include <sstream>
#include "FireAtTarget.h"
#include "GetFreeCells.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#define SCREENX 1366
#define SCREENY 768
#define FULLSCREEN false	

int main()
{
	
	srand(time(NULL));
	int numPlayers = 4;

	//Setup irrlicht
	MyEventReceiver Receiver;
	IrrlichtDevice *device = createDevice(EDT_OPENGL,dimension2d<u32>(SCREENX,SCREENY),32,FULLSCREEN,false,false,&Receiver);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	//Load key config
	Receiver.KeyConfig.loadFromFile("KeyConfig.txt");

	//Generate maze
	MazeGenerator MG;
	MG.NewSize(495,497,495,497);
	MG.Generate("DrunkenWalk2",500,numPlayers);//500,numPlayers);
	MG.Minimise();
	MG.MinimalSolids(false);

	MG.ConvertLoneToCover();

	//Setup textures
	ITexture *FloorTexture = driver->getTexture("Textures/Floor.png");
	ITexture *WallTexture = driver->getTexture("Textures/Wall.png");
	ITexture *CoverTexture = driver->getTexture("Textures/Cover.png");

	//Import maze
	MazeRenderer MR;
	MR.CreateFromMazeGenerator( MG, smgr );
	MR.SetTextures( FloorTexture, WallTexture, CoverTexture );

	//Setup camera
	GameCamera gameCam(70);
	gameCam.camera = smgr->addCameraSceneNode();
	gameCam.camera->setPosition( vector3df( (MR.XSize/2)*MR.TileSize, 40, (MR.YSize/2)*MR.TileSize ) );
	gameCam.camera->setRotation( vector3df(0,90,90));

	//Create players (4) for testing
	//Have to use an array as you can't copy boost::threads which pushing into vectors does
	Entity *Player = new Entity[numPlayers];

	EntityManager PlayerManager(&Player[0],numPlayers);

	//Textures
	ITexture* PlayerTexture[4];
	PlayerTexture[0] = driver->getTexture("Textures/Player1.png");
	PlayerTexture[1] = driver->getTexture("Textures/Player2.png");
	PlayerTexture[2] = driver->getTexture("Textures/Player3.png");
	PlayerTexture[3] = driver->getTexture("Textures/Player4.png");

	//Names
	vector<std::string> PlayerName;
	PlayerName.push_back("Motoko");
	PlayerName.push_back("Saito");
	PlayerName.push_back("Batou");
	PlayerName.push_back("Togusa");

	//use this method to get starting cells for an enemy group
	vector<v2d> vv = GetFreeCells(MG.Convert(),MG.StartX,MG.StartY,4);
	
	//HUD test
	hud HUD(guienv,dimension2d<s32>(SCREENX,SCREENY),"Font\\myfont.xml");

	for (int i = 0; i < numPlayers; i++ )
	{
		Player[i].Construct(smgr,MG.Convert(), PlayerTexture[i]);
		Player[i].Stats.Name = PlayerName[i];
		Player[i].Stats.MaxHealth = 100;
		Player[i].Stats.RestoreAll();
		Player[i].X = MG.XStarts[i];
		Player[i].Y = MG.YStarts[i];
		Player[i].Stats.MovementSteps = 5+i;
		Player[i].ReachableTile = driver->getTexture("Textures/TurnReachable.png");
		Player[i].UnreachableTile = driver->getTexture("Textures/TurnUnreachable.png");
		Player[i].Stats.RealFiringDistance = 100;
		Player[i].CellShootable = driver->getTexture("Textures/CellShootable.png");
		Player[i].CellShootableCover = driver->getTexture("Textures/CellShootableCover.png");
		Player[i].Position();
		path filename = "Avatars/";
		filename.append(PlayerName[i].c_str());
		filename.append(".png");
		Player[i].Avatar = driver->getTexture( filename );

		//test stats
		Player[i].Stats.Accuracy = 100;
		Player[i].Stats.Attack = 100;
		Player[i].Stats.AttackVariance = 0;
		Player[i].Stats.Defense = 0;
		Player[i].Stats.DefenseVariance = 0;
	}


	// Create the 3D cursor
	ISceneNode *Cursor = smgr->addSphereSceneNode(4);

	//Setup AI
	int numEnemies = 1;
	Entity *Enemy = new Entity[numEnemies];
	EntityManager EnemyManager(&Enemy[0],numEnemies);

	//Textures (none for now)

	for (int i = 0; i < numEnemies; i++)
	{
		Enemy[i].Construct(smgr,MG.Convert(), driver->getTexture("Textures/Enemy.png"));
		Enemy[i].Stats.Name = PlayerName[i];
		Enemy[i].Stats.MaxHealth = 100;
		Enemy[i].Stats.RestoreAll();
		Enemy[i].Stats.MovementSteps = 5+i;
		Enemy[i].Stats.RealFiringDistance = 100;
		Enemy[i].Stats.Attack = 50;
		Enemy[i].Stats.Accuracy = 100;

		//Stats

		//steup random position
		Enemy[i].AI_SetRandomDestination();
		Enemy[i].X = Enemy[i].AI_DestinationX;
		Enemy[i].Y = Enemy[i].AI_DestinationY; //new path will be created
		Enemy[i].Position();

		Enemy[i].AI_State = AI::Patrol;
		Enemy[i].isAI = true;
	}

	bool rkf = false;

	//Game loop
	while (device->run())
	{
		//Create a list of all entities
		vector<Entity*> EntityList( PlayerManager.GetEntityList() );
		vector<Entity*> EnemyList( EnemyManager.GetEntityList() );
		EntityList.insert( EntityList.end(), EnemyList.begin(), EnemyList.end() );

		/* * * * * * * * * * * * * * */
		/* Player switching module   */
		/* * * * * * * * * * * * * * */
		for (int i = 0; i < PlayerManager.Size; i++)
			PlayerManager.GetEntity(i)->CheckIfDead();

		PlayerManager.ManageCharacterSwitching(Receiver);

		/* * * * * * * * * * * */
		/* Pathfinding module  */
		/* * * * * * * * * * * */
		PlayerManager.GetCurrentEntity()->UpdatePathDisplayTimer();
		if (Receiver.MouseState.LeftButtonDown && PlayerManager.GetCurrentEntity()->NewPathDisplayTimer < 1)
			PlayerManager.GetCurrentEntity()->CreatePathToDestination(EntityList,smgr,Receiver,Cursor->getPosition(),dimension2d<s32>(MR.XSize,MR.YSize),MG.Convert());

		/* * * * * * * * * * * * * */
		/* Update position module  */
		/* * * * * * * * * * * * * */
		PlayerManager.GetCurrentEntity()->Position();

		/* * * * * * * * * * * */
		/* Follow path module  */
		/* * * * * * * * * * * */
		if (Receiver.IsKeyDown(Receiver.KeyConfig.FOLLOW_PATH))
			PlayerManager.GetCurrentEntity()->FollowPath(EntityList);

		/* * * * * * * * * */
		/* End turn module */
		/* * * * * * * * * */
		if (Receiver.IsKeyDown(Receiver.KeyConfig.END_TURN) && !rkf)
		{
			rkf = true;
			PlayerManager.EndTurn();
			EnemyManager.AI_StartTurn(EntityList, PlayerManager, smgr, Receiver, irr::core::dimension2d<s32>(MR.XSize,MR.YSize), MG.Convert(), MR.MetaTriangleSelector, MR.MetaCoverTriangleSelector);
			PlayerManager.StartTurn();
		}
		else if (!Receiver.IsKeyDown(Receiver.KeyConfig.END_TURN))
			rkf = false;

		/* * * * * * * * */
		/* Cursor module */
		/* * * * * * * * */
		Cursor->setPosition( vftovi(MouseTo3D( Receiver, MR.CollisionManager, MR.TriSelector))  );

		/* * * * * * * * */
		/* Camera module */
		/* * * * * * * * */
		gameCam.Update(Receiver,PlayerManager.GetCurrentEntity()->Node);

		/* * * * * * * * * * * * */
		/* Stats monitor module  */
		/* * * * * * * * * * * * */
		for (int i=0;i<PlayerManager.Size;i++)
			PlayerManager.GetEntity(i)->Stats.MonitorAll();

		
		/* * * * * * * * * * */
		/* Draw scene module */
		/* * * * * * * * * * */
		driver->beginScene(true,true,SColor(255,100,101,140));
		

		smgr->drawAll();
		guienv->drawAll();
		HUD.Draw(guienv,*PlayerManager.GetCurrentEntity());
		HUD.DrawAIDebug(guienv,*EnemyManager.GetEntity(0),PlayerManager);
		driver->endScene();

		
		/* * * * * * * */
		/* Menu module */
		/* * * * * * * */
		if (Receiver.IsKeyDown(Receiver.KeyConfig.MENU))
			return 0;

		/* * * * * * * * * * * * */
		/* Firing display module */
		/* * * * * * * * * * * * */
		if (!Receiver.IsKeyDown(KEY_KEY_S)) 
			PlayerManager.GetCurrentEntity()->ShootDisplayKeyFlag = false;
		else 
		{
			PlayerManager.GetCurrentEntity()->ShootDisplayKeyFlag = true;
			PlayerManager.GetCurrentEntity()->DisplayAllCellsThatCanBeShotUsingThreads(smgr,MR.CollisionManager,MR.MetaTriangleSelector,MR.MetaCoverTriangleSelector);
		}

		/* * * * * * * * * */
		/* Firing module * */
		/* * * * * * * * * */
		if (Receiver.MouseState.RightButtonDown && !PlayerManager.GetCurrentEntity()->RightMouseFlag)
		{
			PlayerManager.GetCurrentEntity()->RightMouseFlag = true;

			//second playerManager needs to replaced with enemyManager when in place, the function will also need to be edited
			FireAtTarget(Receiver, PlayerManager, EnemyManager, MR, smgr, Cursor);
		}
		else if (!Receiver.MouseState.RightButtonDown)
			PlayerManager.GetCurrentEntity()->RightMouseFlag = false;
	}

	return 0;
}