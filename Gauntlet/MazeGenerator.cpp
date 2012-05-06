 #include "mazegenerator.h"

vector<string> MazeGenerator::Convert() const
{
	
		//create the collision map consisting of # C ' ' and .
	
	std::vector<std::string> groundCollisionMap;
	std::vector<std::string> mapCollisionMap;

	std::vector<int> SolTiles;
	std::vector<int> CovTiles;

	SolTiles.push_back(1);
	CovTiles.push_back(2);

	groundCollisionMap = getCollisionMap(groundLayer,SolTiles,CovTiles);
	mapCollisionMap = getCollisionMap(mapLayer,SolTiles,CovTiles);

	appendCollisionMap(groundCollisionMap, mapCollisionMap);

	return groundCollisionMap;
}

void MazeGenerator::MapFromFile( std::string groundDir, std::string mapDir, std::string entityDir )
{
	MapLoadedFromFile = true;

	std::string *directories = new std::string[3];
	directories[0] = groundDir;
	directories[1] = mapDir;
	directories[2] = entityDir;

	std::ifstream *mapFiles = new std::ifstream[3];

	for (int i = 0; i < 3; i++)
	{
		mapFiles[i].open( directories[i] );

		if (!mapFiles[i])
			//Error
			int brk = 23;

		while (!mapFiles[i].eof())
		{
			std::vector<int> row;
			std::string line;
			std::getline(mapFiles[i], line);
				
			int lineIndex = 0;
			std::string tileNum = "";

			//Parse line
			while (lineIndex < line.length())
			{
				if (lineIndex == line.length()-1)
				{
					tileNum += line[lineIndex];
					row.push_back( atoi(tileNum.c_str()) );
					tileNum = "";
				}
				else if (isdigit(line[lineIndex]))
				{
					tileNum += line[lineIndex];
				}
				else if (line[lineIndex] == ',')
				{
					row.push_back( atoi(tileNum.c_str()) );
					tileNum = "";
				}

				lineIndex++;
			}

			XSize = row.size();

			switch (i)
			{
				case 0: groundLayer.push_back(row); break;
				case 1: mapLayer.push_back(row); break;
				case 2: entityLayer.push_back(row); break;
				default: break;
			}
		}

		YSize = groundLayer.size();

		mapFiles[i].close();
	}

	delete[] mapFiles;
	delete[] directories;
}