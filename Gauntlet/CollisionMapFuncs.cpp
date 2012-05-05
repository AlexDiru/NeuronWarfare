#include "CollisionMapFuncs.h"

std::vector<std::string> getCollisionMap( const std::vector<std::vector<int>>& tiles, const std::vector<int>& solidTiles, const std::vector<int>& coverTiles )
{
	std::vector<std::string> rv;

	for (int y = 0; y < tiles.size(); y++)
	{
		std::string row; //to push back into vec
		for (int x = 0; x < tiles[y].size(); x++)
		{
			bool solidFound = false;
			bool coverFound = false;
			for (int s = 0; s < solidTiles.size(); s++)
			{
				if (tiles[y][x] == solidTiles[s])
				{
					//solid detected
					solidFound = true;
					row.push_back('#');
					break;
				}
			}
			for (int s = 0; s < coverTiles.size(); s++)
			{
				if (tiles[y][x] == coverTiles[s])
				{
					coverFound = true;
					row.push_back('C');
					break;
				}
			}
			if (!solidFound && !coverFound)
				row.push_back('.');
		}
		rv.push_back(row);
	}

	return rv;
}

void appendCollisionMap(std::vector<std::string>& target, std::vector<std::string>& tm)
{
	if (tm.size() != target.size())
	{
		std::cout << "Incompatible size" << std::endl;
		return;
	}

	for (int y = 0; y < tm.size(); y++)
		for (int x = 0; x < tm[y].size(); x++)
			if (tm[y][x] == '#') //solid
			{
				if (target[y][x] != '#')
					target[y][x] = '#';
			}
			else if (tm[y][x] == 'C') //cover
				if (target[y][x] != 'C')
					target[y][x] = 'C';

}