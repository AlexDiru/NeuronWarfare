#include "MazeRenderer.h"

Tile3d::Tile3d ( irr::scene::ISceneManager *smgr, int size )
{
	this->smgr = smgr;
	Size = size;
	IsFloor = false;
	IsSolid = false;
	IsCover = false;
	this->triSel;
}

void Tile3d::CreateSolid()
{
	node = smgr->addCubeSceneNode(Size);
	IsSolid = true;
}

void Tile3d::CreateCover()
{
	node = smgr->addCubeSceneNode(Size / 2);
	IsCover = true;
}

void Tile3d::Position(int x, int z)
{
	node->setPosition(irr::core::vector3df(Size*x,Size/2,Size*z));
}

//======

void MazeRenderer::CreateFromMazeGenerator(const MazeGenerator& mg, irr::scene::ISceneManager *smgr)
{
	XSize = mg.YSize;
	YSize = mg.XSize;
	SetStart(mg.StartY,mg.StartX);
	SetEnd(mg.EndY,mg.EndX);

	CreateFromVectorString(smgr,mg.Convert());

	CollisionManager = smgr->getSceneCollisionManager();
}

void MazeRenderer::AddEndLandmark(irr::video::ITexture *txt, irr::scene::ISceneManager *smgr)
{
	EndLandmark = smgr->addCubeSceneNode();
	EndLandmark->setScale(irr::core::vector3df(1,30,1));
	EndLandmark->setPosition(irr::core::vector3df(EndX*TileSize,15,EndY*TileSize));
	EndLandmark->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	EndLandmark->setMaterialTexture(0,txt);
	EndLandmark->getMaterial(0).getTextureMatrix(0).setTextureScale(1,30);
}

void MazeRenderer::CreateFromVectorString(irr::scene::ISceneManager *smgr, const std::vector<std::string>& Maze)
{
	this->MetaTriangleSelector = smgr->createMetaTriangleSelector();
	this->MetaCoverTriangleSelector = smgr->createMetaTriangleSelector();

	for (int i=0;i<Maze.size();i++)
	{
		std::vector<Tile3d> tempVector;
			for (int j=0;j<Maze[i].size();j++)
			{
				if (Maze[i][j] == '#')
				{
					Tile3d temp(smgr,TileSize);
					temp.CreateSolid();
					temp.Position(i,j);

					temp.triSel = smgr->createTriangleSelectorFromBoundingBox(temp.node);

					tempVector.push_back(temp);

					this->MetaTriangleSelector->addTriangleSelector(temp.triSel);
				}
				else if (Maze[i][j] == 'C') //cover
				{
					Tile3d temp(smgr, TileSize);
					temp.CreateCover();
					temp.Position(i,j);

					temp.triSel = smgr->createTriangleSelectorFromBoundingBox(temp.node);

					this->MetaCoverTriangleSelector->addTriangleSelector(temp.triSel);

					tempVector.push_back(temp);
				}
			}
			tiles.push_back(tempVector);
	}

	//create floor
	Floor = smgr->addCubeSceneNode(1);
	Floor->setScale(irr::core::vector3df(XSize*TileSize,1,YSize*TileSize));
	Floor->setPosition(irr::core::vector3df(floor(double(XSize*TileSize/2))-TileSize/2,0,floor(double(YSize*TileSize/2))-TileSize/2));

	//tri selector for floor
	TriSelector = smgr->createTriangleSelectorFromBoundingBox(Floor);
	this->MetaTriangleSelector->addTriangleSelector(TriSelector);
}

void MazeRenderer::SetTextures(irr::video::ITexture *FloorTexture, irr::video::ITexture *WallTexture, irr::video::ITexture *CoverTexture)
{
	for (int i=0;i<tiles.size();i++)
	{
		for (int j=0;j<tiles[i].size();j++)
		{
			tiles[i][j].node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		
			if (tiles[i][j].IsSolid)
				tiles[i][j].node->setMaterialTexture(0,WallTexture);
			else if (tiles[i][j].IsCover)
				tiles[i][j].node->setMaterialTexture(0,CoverTexture);
		}
	}

	//scroll floor text
	Floor->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	Floor->setMaterialTexture(0,FloorTexture);
	Floor->getMaterial(0).getTextureMatrix(0).setTextureScale(YSize,XSize);
}