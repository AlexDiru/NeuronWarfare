//===================================
//include guard
#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

//===================================
//included dependencies
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <cmath>
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
	vector<int> XStarts;
	vector<int> YStarts;
	int StartX;
	int StartY;
	int XSize;
	int YSize;
	int EndX;
	int EndY;

	vector< vector<Tile> > tiles;
	MazeGenerator() {}
	MazeGenerator(string method,int,int); //Generates a maze given size

	void NewSize(int ix, int ax, int iy, int ay);
	void Generate(string method);
	void Generate(string method, int density);
	void Generate(string method, int density, int numberStarts);
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
	vector<string> Convert() const;
};

#endif