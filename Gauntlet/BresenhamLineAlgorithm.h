#pragma once

#include <iostream>
#include <string>
#include <vector>

struct line
{
	std::vector<std::pair<int,int>> Pos;
};

class BresenhamLineAlgo
{
public:
	//Pointer save memory and time to copy vector
	std::vector<std::string> Map;

	BresenhamLineAlgo(const std::vector<std::string>& map);

	line GetLine( int x0, int y0, int x1, int y1 );

	void CheckAllRange( std::vector<std::vector<int>>& canShoot, int tileFiringDistance, int playerTileX, int playerTileY);
};