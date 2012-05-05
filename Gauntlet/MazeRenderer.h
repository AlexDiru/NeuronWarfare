#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include "MazeGenerator.h"

class Tile3d
{
public:
	irr::scene::ISceneManager *smgr;
	irr::scene::ISceneNode *node;
	int Size;
	int Height;
	bool IsFloor;
	bool IsSolid;
	bool IsCover;

	irr::scene::ITriangleSelector *triSel;

	Tile3d ( irr::scene::ISceneManager *smgr, int size );

	void CreateSolid();

	void CreateCover();

	void Position(int x, int z);
};

class MazeRenderer
{
public:

	std::vector<std::vector<Tile3d>> tiles;
	std::vector<irr::scene::ISceneNode*> Buildings;
	irr::scene::ISceneNode *Floor;
	irr::scene::ISceneNode *EndLandmark;

	//for cursor -> 2d
	irr::scene::ISceneCollisionManager *CollisionManager;
	irr::scene::ITriangleSelector *TriSelector;

	irr::scene::IMetaTriangleSelector *MetaTriangleSelector;
	irr::scene::IMetaTriangleSelector *MetaCoverTriangleSelector;

	int TileSize;
	int StartX, StartY;
	int EndX, EndY;
	int XSize, YSize;

	MazeRenderer() : TileSize(10) {}

	void CreateFromMazeGenerator(const MazeGenerator& mg, irr::scene::ISceneManager *smgr);

	void SetStart(int x, int y) { StartX = x; StartY = y; }

	void SetEnd(int x, int y) { EndX = x; EndY = y; }

	void AddEndLandmark(irr::video::ITexture *txt, irr::scene::ISceneManager *smgr);

	void CreateFromVectorString(irr::scene::ISceneManager *smgr, const std::vector<std::string>& Maze);

	void SetTextures(irr::video::ITexture *FloorTexture, irr::video::ITexture *WallTexture, irr::video::ITexture *CoverTexture);

	void CreateFromMazeGeneratorFile( const MazeGenerator &mg, irr::scene::ISceneManager *smgr)
	{
		//Ge
	}
};