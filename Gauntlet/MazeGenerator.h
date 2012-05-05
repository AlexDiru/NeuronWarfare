//===================================
//include guard
#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#define _EXCLUDE_RANDOM_GENERATION 1 

//===================================
//included dependencies
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <string>
#include <fstream>

#include "CollisionMapFuncs.h"

using namespace std;


//a vertex of the maze
class Tile
{
public:
	Tile *parent;
	Tile() 
	{
		marked=false;
		isNull = false;
		isPath = false;
		toBeRemoved = false;
		outsideOfMap = false;
		isBuilding = false;
		cover = false;
	}
	Tile(int X, int Y) 
	{ 
		Tile temp;
		temp.x = X;
		temp.y = Y;
		(*this) = temp;
	}

	int x;
	int y;
	bool solid;
	bool cover;
	bool marked;
	bool isNull;
	bool isPath;
	bool toBeRemoved;
	bool outsideOfMap;
	bool isBuilding;
};

//a class to generate a random maze
class MazeGenerator
{

	void DepthFirstSearch(Tile& vertex); //recursive DFS to generate the maze
	void DrunkenWalk(); //drunken walk algorithm to generate the maze
	void DrunkenWalk2(int density);
	void DrunkenWalk2(int density, int numberStarts);

	void CutHoleMark(int,int,int,int);
	bool MarkedTilesInTheArea(int, int, int, int);
	void ChooseRandomExit();
	void ChooseRandomStart();

	vector<Tile> GetNeighbours(Tile n);
	vector<Tile> GetDiagonalNeighbours(Tile n);


public:

	bool MapLoadedFromFile;
	std::vector<std::vector<int>> groundLayer;
	std::vector<std::vector<int>> mapLayer;
	std::vector<std::vector<int>> entityLayer;

	//Loads the map from files made up of ground layer, map layer and entity layer
	void MapFromFile( std::string groundDir, std::string mapDir, std::string entityDir );

	vector<int> XStarts;
	vector<int> YStarts;
	int StartX;
	int StartY;
	int XSize;
	int YSize;
	int EndX;
	int EndY;

	

	
	#ifndef _EXCLUDE_RANDOM_GENERATION

	std::vector< std::vector<Tile> > tiles;
	MazeGenerator() { MapLoadedFromFile = false; }
	MazeGenerator(std::string method,int,int); //Generates a maze given size

	void NewSize(int ix, int ax, int iy, int ay);
	void Generate(std::string method);
	void Generate(std::string method, int density);
	void Generate(std::string method, int density, int numberStarts);
	void Minimise();
	void MinimalSolids(bool includeDiagonals);
	void CalculateOutsideMap();
	void AddBuildings(int probability);


	void AddCircularPaths(int RemovalAttempts);
	void ConvertLoneToCover();
	void CutHole(int, int, int, int);
	void CutHoleInMiddle(int, int); //Cuts a hole in the maze - space for menu
	void CreateRandomHoles(int maxWidth, int maxHeight);
	void CreateRandomHoles(int minWidth, int minHeight, int maxWidth, int maxHeight);
	void CreateRandomHoles(int minWidth, int minHeight, int maxWidth, int maxHeight, int minHoles, int maxHoles);
	void CreateRandomHoles(int minWidth, int minHeight, int maxWidth, int maxHeight, bool NoCollisions, int DensityPercent);
	void Print();
	void Print(int x, int y, int Dist);
	void Print(int x, int y, int DistX, int DistY);

	#endif

	void ParseForPlayerStartPositions(std::vector<int> PlayerStartTilesValue)
	{
		//entityLayer
		for (int i = 0; i < entityLayer.size(); i++)
			for (int j = 0; j < entityLayer[i].size(); j++)
				for (int s = 0; s < PlayerStartTilesValue.size(); s++)
					if (entityLayer[i][j] == PlayerStartTilesValue[s])
					{
						XStarts.push_back(j);
						YStarts.push_back(i);
					}
	}

	std::vector<std::string> Convert() const;
};

#endif