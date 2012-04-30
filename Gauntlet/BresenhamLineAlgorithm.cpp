#include "BresenhamLineAlgorithm.h"

void BresenhamLineAlgo::CheckAllRange( std::vector<std::vector<int>>& canShoot, int tileFiringDistance, int playerTileX, int playerTileY)
{
	for (int y = playerTileY-tileFiringDistance; y <= playerTileY+tileFiringDistance; y++) //Borders
		for (int x = playerTileX-tileFiringDistance; x <= playerTileX+tileFiringDistance; x ++) //Borders
			//if (x == -tileFiringDistance || x == tileFiringDistance || y == -tileFiringDistance || y == tileFiringDistance) //Has to be a border
			{
				//no boundary errors
				//if (!(x+tileFiringDistance > -1 && y+tileFiringDistance > -1 && x+tileFiringDistance < this->Map[0].size() && y+tileFiringDistance < this->Map.size()))
				if (!(x > -1 && y > -1 && y < Map[0].size() && x < Map[0].size()))
					continue;
				//draw the line
				line l = this->GetLine(playerTileX,playerTileY,x,y);

				for (int c = 0; c < l.Pos.size(); c ++) //Line coords - have to start at the coord that is the player's pos
				{
					int ly = l.Pos[c].second;
					int lx = l.Pos[c].first;

					//no boundary errors
					if (!(lx > -1 && ly > -1 && lx < this->Map[0].size() && ly < this->Map.size()))
					{
						canShoot[ly+tileFiringDistance-playerTileY][lx+tileFiringDistance-playerTileX] = 0;
						continue;
					}

					
					if ( Map[ly][lx] == '#' || Map[ly][lx] == ' ') //Wall or out of map
					{
						canShoot[ly+tileFiringDistance-playerTileY][lx+tileFiringDistance-playerTileX] = 0; //false - can't shoot
						//set all future coords to can't shoot
						while (++c < l.Pos.size())
						{
							ly = l.Pos[c].second;
							lx = l.Pos[c].first;
							canShoot[ly+tileFiringDistance-playerTileY][lx+tileFiringDistance-playerTileX] = 0;
						}
					}
					else if (Map[ly][lx] == 'C' ) //Cover
						//set next coord to cover
						canShoot[ly+tileFiringDistance-playerTileY][lx+tileFiringDistance-playerTileX] = 2; //Cover
				}
			}

}

line BresenhamLineAlgo::GetLine( int x0, int y0, int x1, int y1 )
{
	line Line;

	bool steep = abs(y1-y0) > abs(x1-x0);

	if (steep)
	{
		std::swap(x0,y0);
		std::swap(x1,y1);
	}

	if (x0 > x1)
	{
		std::swap(x0,x1);
		std::swap(y0,y1);
	}

	int dx = x1 - x0;
	int dy = abs(y1 - y0);

	int error = dx/2;

	int ystep;
	int y = y0;

	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	for (int x = x0; x < x1; x++)
	{
		if (steep)
			Line.Pos.push_back( std::pair<int,int>(y,x) );
		else
			Line.Pos.push_back( std::pair<int,int>(x,y) );
			
		error -= dy;

		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}

	return Line;
}

BresenhamLineAlgo::BresenhamLineAlgo(const std::vector<std::string>& map)
{
	for (int i = 0; i < map.size(); i++)
	{
		Map.push_back(map[i]);
	}
}