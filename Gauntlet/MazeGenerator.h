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

//a class to generate a random maze
class MazeGenerator
{

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