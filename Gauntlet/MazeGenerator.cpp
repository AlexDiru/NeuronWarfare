 #include "mazegenerator.h"

#ifndef _EXCLUDE_RANDOM_GENERATION
void MazeGenerator::CutHoleInMiddle(int Width, int Height)
{
	for (int y=this->YSize/2-Height/2; y<=this->YSize/2+Height/2; y++)
		for (int x=this->XSize/2-Width/2; x<=this->XSize/2+Width/2; x++)
			this->tiles[y][x].solid = false;
}

void MazeGenerator::CutHole(int x1, int y1, int x2, int y2)
{
	for (int y=y1; y<=y2; y++)
		for (int x=x1; x<=x2; x++)
			this->tiles[y][x].solid = false;
}

void MazeGenerator::CutHoleMark(int x1, int y1, int x2, int y2)
{
	for (int y=y1; y<=y2; y++)
		for (int x=x1; x<=x2; x++)
		{
			this->tiles[y][x].solid = false;
			this->tiles[y][x].marked = true;
		}
}

bool MazeGenerator::MarkedTilesInTheArea(int x1, int y1, int x2, int y2)
{
	for (int i = y1; i <= y2; i++)
		for (int j = x1; j <= x2; j++)
			if (tiles[i][j].marked)
				return true;
	return false;
}

void MazeGenerator::CreateRandomHoles(int maxWidth, int maxHeight)
{
	int NumberOfHoles = rand()%3+1;

	for (int i = 0; i < NumberOfHoles; i++)
	{
		int x1 = rand()%(this->XSize - 1 - maxWidth)+1;
		int y1 = rand()%(this->YSize - 1 - maxHeight)+1;

		int x2 = rand()%maxWidth;
		int y2 = rand()%maxHeight;

		this->CutHole(x1,y1,x1+x2,y1+y2);
	}
	
}

void MazeGenerator::CreateRandomHoles(int minWidth, int minHeight, int maxWidth, int maxHeight)
{
	int NumberOfHoles = rand()%3+1;

	for (int i = 0; i < NumberOfHoles; i++)
	{
		int x1 = rand()%(this->XSize - 1 - maxWidth)+1;
		int y1 = rand()%(this->YSize - 1 - maxHeight)+1;

		int x2 = rand()%(maxWidth-minWidth)+minWidth;
		int y2 = rand()%(maxHeight-minHeight)+minHeight;

		this->CutHole(x1,y1,x1+x2,y1+y2);
	}
	
}

void MazeGenerator::CreateRandomHoles(int minWidth, int minHeight, int maxWidth, int maxHeight, int minHoles, int maxHoles)
{
	int NumberOfHoles = rand()%(maxHoles-minHoles)+minHoles;

	for (int i = 0; i < NumberOfHoles; i++)
	{
		int x1 = rand()%(this->XSize - 1 - maxWidth)+1;
		int y1 = rand()%(this->YSize - 1 - maxHeight)+1;

		int x2 = rand()%(maxWidth-minWidth)+minWidth;
		int y2 = rand()%(maxHeight-minHeight)+minHeight;

		this->CutHole(x1,y1,x1+x2,y1+y2);
	}
	
}

void MazeGenerator::AddCircularPaths(int RemovalAttempts)
{
	for (int i = 0;i<RemovalAttempts;i++)
		tiles[rand()%(this->YSize-2)+1][rand()%(this->XSize-2)+1].solid = false;
}

void MazeGenerator::DepthFirstSearch(Tile& vertex)
{
	//remove this tile
	vertex.solid = false;

	//error check - otherwise vector range may be oob
	if (vertex.x < 0 || vertex.x >= this->XSize || vertex.y < 0 || vertex.y >= this->YSize)
		return;
	
	//pick a random direction
	int d = rand()%4;
	//variable to make sure all directions have been checked
	int count = 0;

	switch (d)
	{
	case 0:
		case0:
		//e
		if (vertex.x - 2 > 0)
			if (tiles[vertex.y][vertex.x-2].solid)
			{
			
				tiles[vertex.y][vertex.x-1].solid = false;
				DepthFirstSearch(tiles[vertex.y][vertex.x-2]);
			}
		count++;
		if (count > 3) break;

	case 1:
		//w
		if (vertex.x + 2 < this->XSize-1)
			if (tiles[vertex.y][vertex.x+2].solid)
			{
				tiles[vertex.y][vertex.x+1].solid = false;
				DepthFirstSearch(tiles[vertex.y][vertex.x+2]);
			}
		count++;
		if (count > 5) break;

	case 2:
		//n
		if (vertex.y - 2 > 0)
			if (tiles[vertex.y-2][vertex.x].solid)
			{
				tiles[vertex.y-1][vertex.x].solid = false;
				DepthFirstSearch(tiles[vertex.y-2][vertex.x]);
			}
		count++;
		if (count > 3) break;

	case 3:
		//s
		if (vertex.y + 2 < this->YSize-1)
			if (tiles[vertex.y+2][vertex.x].solid)
			{
				tiles[vertex.y+1][vertex.x].solid = false;
				DepthFirstSearch(tiles[vertex.y+2][vertex.x]);
			}
		count++;
		if (count > 3) break;
		goto case0;
	}
}

void MazeGenerator::ConvertLoneToCover()
{
	//if a wall is open on sides convert it to cover ('C')
	for (int y = 1; y < YSize-1; y++)
		for (int x = 1; x < XSize-1; x++)
			if ( tiles[y][x].solid 
								   //north south east west
								   && (!tiles[y-1][x].solid && !tiles[y-1][x].cover) 
								   && (!tiles[y+1][x].solid && !tiles[y+1][x].cover)
								   && (!tiles[y][x-1].solid && !tiles[y][x-1].cover)
								   && (!tiles[y][x+1].solid && !tiles[y][x+1].cover)  
								   //diagonals
								   && (!tiles[y-1][x-1].solid && !tiles[y-1][x-1].cover) 
								   && (!tiles[y-1][x+1].solid && !tiles[y-1][x+1].cover)
								   && (!tiles[y+1][x-1].solid && !tiles[y+1][x-1].cover)
								   && (!tiles[y+1][x+1].solid && !tiles[y+1][x+1].cover) )
				tiles[y][x].cover = true;
}

void MazeGenerator::CreateRandomHoles(int minWidth, int minHeight, int maxWidth, int maxHeight, bool NoCollisions, int DensityPercent)
{
	if (NoCollisions)
	{
		for (int i=0;i<tiles.size();i++)
					for (int j=0;j<tiles[i].size();j++)
						tiles[i][j].marked = false;

		int x = 1;
		int y = 1;

		DensityPercent = 100 - DensityPercent;
		int Prob = (DensityPercent*((XSize+YSize)))/60;

		while (1)
		{
			int r = rand()%(Prob+1);
			
			if (r == 0 && !MarkedTilesInTheArea(x,y,x+maxWidth,y+maxHeight))
			{
				int w = rand()%(maxWidth-minWidth)+minWidth;
				int h = rand()%(maxHeight-minHeight)+minHeight;
				CutHoleMark(x,y,x + w, y + h);
				x += w;
				//
			}
			else x++;

			if (x >= XSize-1-maxWidth) { x = 1; y++; }
			if (y >= YSize-1-maxHeight) 
			{
				//remove all marks
				for (int i=0;i<tiles.size();i++)
					for (int j=0;j<tiles[i].size();j++)
						tiles[i][j].marked = false;
				return;
			}

		}
	}
	else
		CreateRandomHoles(minWidth,minHeight,maxWidth,maxHeight,(DensityPercent*((XSize+YSize)/2))/100,((DensityPercent*((XSize+YSize)/2))/100)+1);
}

int nearestOdd(int n, int minval, int maxval)
{
	if ((n/2)*2 != n)
		return n;
	else 
	{
		if (n-1 < minval)
			return n+1;
		else
			return n-1;
	}
}

void MazeGenerator::Generate(string method)
{
	Generate(method,1000);
}

void MazeGenerator::Generate(string method, int Density)
{
	this->tiles.clear();
	int ys = XSize;
	int xs = YSize;

	//Setup the maze tiles vector
	for (int i=0;i<ys;i++)
	{
		vector<Tile> tvec;
		for (int j=0;j<xs;j++)
		{
			Tile temptile;
			temptile.x = j;
			temptile.y = i;
			temptile.solid = true;
			tvec.push_back(temptile);
		}
		tiles.push_back(tvec);
	}

	//Random odd current cell
	this->StartX = nearestOdd(rand()%(xs-2)+1,1,ys-1);
	this->StartY = nearestOdd(rand()%(ys-2)+1,1,ys-1);
	Tile currentCell(StartX,StartY);
	this->XSize = xs;
	this->YSize = ys;

	if (method.compare("DFS") == 0)
	{
		DepthFirstSearch(currentCell);
	
		ChooseRandomStart();
		ChooseRandomExit();
	}
	else if (method.compare("DrunkenWalk") == 0)
	{
		DrunkenWalk();

		ChooseRandomStart();
		ChooseRandomExit();
	}
	else if (method.compare("DrunkenWalk2") == 0)
	{
		DrunkenWalk2(Density);
	}
}

void MazeGenerator::Generate(string method, int Density, int numberStarts)
{
	this->tiles.clear();
	int ys = XSize;
	int xs = YSize;

	//Setup the maze tiles vector
	for (int i=0;i<ys;i++)
	{
		vector<Tile> tvec;
		for (int j=0;j<xs;j++)
		{
			Tile temptile;
			temptile.x = j;
			temptile.y = i;
			temptile.solid = true;
			tvec.push_back(temptile);
		}
		tiles.push_back(tvec);
	}

	//Random odd current cell
	this->StartX = nearestOdd(rand()%(xs-2)+1,1,ys-1);
	this->StartY = nearestOdd(rand()%(ys-2)+1,1,ys-1);
	Tile currentCell(StartX,StartY);
	this->XSize = xs;
	this->YSize = ys;

	if (method.compare("DFS") == 0)
	{
		DepthFirstSearch(currentCell);
	
		ChooseRandomStart();
		ChooseRandomExit();
	}
	else if (method.compare("DrunkenWalk") == 0)
	{
		DrunkenWalk();

		ChooseRandomStart();
		ChooseRandomExit();
	}
	else if (method.compare("DrunkenWalk2") == 0)
	{
		DrunkenWalk2(Density,numberStarts);
	}
}

void MazeGenerator::ChooseRandomExit()
{
	int x = this->XSize-1;
	int y = this->YSize-1;

	while (1)
	{
		if (!tiles[y][x].solid)
		{
			this->EndX = x;
			this->EndY = y;
			return;
		}

		x--;
		if (x == -1)
		{
			x = XSize-1;
			y--;

			if (y < 0)
			{
				cout << "No free tiles";
				return;
			}
		}
	}
}

void MazeGenerator::ChooseRandomStart()
{
	int x = 0;
	int y = 0;

	while (1)
	{
		if (!tiles[y][x].solid)
		{
			this->StartX = x;
			this->StartY = y;
			return;
		}

		x++;
		if (x == XSize-1)
		{
			x = 0;
			y++;

			if (y > YSize-1)
			{
				cout << "No free tiles";
				return;
			}
		}
	}
}

MazeGenerator::MazeGenerator(string method, int xs, int ys)
{
	XSize = xs;
	YSize = ys;
	Generate(method);
	MapLoadedFromFile = false;
}

void MazeGenerator::Print()
{
	for (int y=0;y<tiles.size();y++)
	{
		for (int x=0;x<tiles[y].size();x++)
		{
			if (StartX == x && StartY == y)
				cout << "S";
			else if (EndX == x && EndY == y)
				cout << "E";
			else if (tiles[y][x].solid)
				cout << "#";
			else if (tiles[y][x].isPath)
				cout << "*";
			else if (tiles[y][x].marked)
				cout << ".";
			else cout << " ";
		}
		cout << endl;
	}
}

void MazeGenerator::Print(int x, int y, int Dist)
{
	int oi,oj;
	int iMax,jMax;

	if (y-Dist/2 > -1)
		oi = y-Dist/2;
	else
		oi = 0;

	if (x-Dist/2 > -1)
		oj = x-Dist/2;
	else
		oj = 0;

	if (y+Dist/2 < YSize)
		iMax = y+Dist/2;
	else
		iMax = YSize-1;

	if (x+Dist/2 < XSize)
		jMax = x+Dist/2;
	else
		jMax = XSize-1;

	for (int i=oi;i<=iMax;i++)
	{
		for (int j=oj;j<=jMax;j++)
			if (tiles[i][j].solid)
				cout << '#';
			else cout << ' ';
		cout << endl;
	}
}

void MazeGenerator::Print(int x, int y, int DistX, int DistY)
{
	int oi,oj;
	int iMax,jMax;

	if (y-DistY/2 > -1)
		oi = y-DistY/2;
	else
		oi = 0;

	if (x-DistX/2 > -1)
		oj = x-DistX/2;
	else
		oj = 0;

	if (y+DistY/2 < YSize)
		iMax = y+DistY/2;
	else
		iMax = YSize-1;

	if (x+DistX/2 < XSize)
		jMax = x+DistX/2;
	else
		jMax = XSize-1;

	for (int i=oi;i<=iMax;i++)
	{
		for (int j=oj;j<=jMax;j++)
			if (tiles[i][j].solid)
				cout << '#';
			else cout << ' ';
		cout << endl;
	}
}
#endif

vector<string> MazeGenerator::Convert() const
{
	
	#ifndef _EXCLUDE_RANDOM_GENERATION
		vector<string> retVal;
	
		for (int i=0; i<tiles.size();i++)
		{
			string temp = "";
			for (int j=0; j<tiles[i].size(); j++)
				if (tiles[i][j].cover)
					temp += 'C';
				else if (tiles[i][j].solid)
					temp += '#';
				else if (tiles[i][j].marked)
					temp += '.';
				else temp += ' ';
				retVal.push_back(temp);
		}

		return retVal;

	#else
	
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

	#endif
}

#ifndef _EXCLUDE_RANDOM_GENERATION
void MazeGenerator::DrunkenWalk()
{
	StartX = rand()%(XSize-2)+1;
	StartY = rand()%(YSize-2)+1;
	int numTries = (XSize*YSize)/2;

	Tile current(StartX,StartY);

	int i = 0;

	do
	{
		//cout << "( " << current.x << " , " << current.y << " )" << endl;
		tiles[current.y][current.x].solid = false;
		
		//rand dir
		int d = rand()%4;

		int count = 0;

		switch (d)
		{
		case 0:
			case0:
			//e
			if (current.x - 1 > 0)
				if (tiles[current.y][current.x-1].solid)
					current = tiles[current.y][current.x-1];
			count++;
			if (count > 3) break;

		case 1:
			//w
			if (current.x + 1 < this->XSize-1)
				if (tiles[current.y][current.x+1].solid)
					current = tiles[current.y][current.x+1];
			count++;
			if (count > 3) break;

		case 2:
			//n
			if (current.y - 1 > 0)
				if (tiles[current.y-1][current.x].solid)
					current = tiles[current.y-1][current.x];
			count++;
			if (count > 3) break;

		case 3:
			//s
			if (current.y + 1 < this->YSize-1)
				if (tiles[current.y+1][current.x].solid)
					current = tiles[current.y+1][current.x];
			count++;
			if (count > 3) break;
			goto case0;
		}

		if (count > 3)
		{
			current.x = rand()%(XSize-2)+1;
			current.y = rand()%(YSize-2)+1;
		}

	} while (++i < numTries);

	return;
}

void MazeGenerator::NewSize(int ix, int ax, int iy, int ay)
{
	XSize = rand()%(ay-iy)+iy;
	YSize = rand()%(ax-ix)+ix;
}

void MazeGenerator::Minimise()
{
	int x1=0,y1=0,x2=XSize-1,y2=YSize-1;
	bool appendLeftBorder=false, appendRightBorder=false, appendTopBorder=false, appendBottomBorder=false;

	//find x1
	for (int i=0; i < XSize; i++)
		for (int j=0; j < YSize; j++)
			if (!tiles[j][i].solid)
			{
				if (i-1 >= 0)
					x1 = i-1;
				else 
				{
					x1 = i;
					appendLeftBorder = true;
				}

				i = XSize;
				j = YSize;
			}
	StartX -= x1;
	for (int i = 0; i < XStarts.size(); i++) XStarts[i] -= x1;
	EndX -=x1;

	//find x2
	for (int i=XSize-1; i >= 0; i--)
		for (int j=YSize-1; j >= 0; j--)
			if (!tiles[j][i].solid)
			{
				if (i+1 < XSize)
					x2 = i+1;
				else 
				{
					x2 = i;
					appendRightBorder = true;
				}
				
				i = -1;
				j = -1;
			}

	//find y1
	for (int j=0; j < YSize; j++)
		for (int i=0; i < XSize; i++)
			if (!tiles[j][i].solid)
			{
				if (j-1 >= 0)
					y1 = j-1;
				else 
				{
					y1 = j;
					appendTopBorder = true;
				}

				i = XSize;
				j = YSize;
			}
	StartY -= y1;
	for (int i = 0; i < YStarts.size(); i++) YStarts[i] -= y1;
	EndY -= y1;

	//find y2
	for (int j=YSize-1; j >= 0; j--)
		for (int i=XSize-1; i >= 0; i--)
			if (!tiles[j][i].solid)
			{
				if (j+1 < YSize)
					y2 = j+1;
				else 
				{
					y2 = j;
					appendBottomBorder = true;
				}
				
				i = -1;
				j = -1;
			}

	//remove tiles (X)
	if (x1 > 0)
		for (int i=0;i<tiles.size();i++)
		{
			tiles[i].erase(tiles[i].begin(),tiles[i].begin() + x1);
		}

	if (tiles.size() > 0)
	{
		//calc new size
		int ox = XSize;
		XSize = tiles[0].size();
		x2 = x2-(ox-XSize);
	}

	if (x2 < XSize-1)
		for (int i=0;i<tiles.size();i++)
			tiles[i].erase(tiles[i].begin() + x2+1,tiles[i].end());


	//remove tiles (Y)
	tiles.erase(tiles.begin(),tiles.begin() + y1);

	//calc new size
	int oy = YSize;
	YSize = tiles.size();
	y2 = y2-(oy-YSize);

	if (y2 < YSize-1)
		tiles.erase(tiles.begin() + y2+1,tiles.end());

	YSize = tiles.size();
	int ox = XSize;
	if (YSize < 1)
		XSize = 0;
	else XSize = tiles[0].size();

}

void MazeGenerator::MinimalSolids(bool includeDiagonals=false)
{
	for (int y=0;y<tiles.size();y++)
		for (int x=0;x<tiles[y].size();x++)
			tiles[y][x].toBeRemoved = false;

	if (!includeDiagonals)
	{
		for (int y=0;y<this->tiles.size();y++)
		{
			for (int x=0;x<this->tiles[y].size();x++)
			{
				Tile temp(x,y);
				vector<Tile> t = GetNeighbours(temp);
				if (t.size() == 0)
					tiles[y][x].toBeRemoved = true;
			}
		}
	}
	else
	{
		for (int y=0;y<this->tiles.size();y++)
		{
			for (int x=0;x<this->tiles[y].size();x++)
			{
				Tile temp(x,y);
				vector<Tile> t = GetDiagonalNeighbours(temp);
				if (t.size() == 0)
					tiles[y][x].toBeRemoved = true;
			}
		}
	}

	for (int y=0;y<tiles.size();y++)
		for (int x=0;x<tiles[y].size();x++)
			if (tiles[y][x].toBeRemoved)
			{
				tiles[y][x].toBeRemoved = false;
				tiles[y][x].solid = false;
			}
			else if (!tiles[y][x].solid)
				tiles[y][x].marked = true;

}

void MazeGenerator::DrunkenWalk2(int density=1000)
{

	//force start in middle (more interesting)
	StartX = (XSize/2)+1;
	StartY = (YSize/2)+1;
	int numTries = density;//(XSize*YSize)/2;

	Tile current(StartX,StartY);
	Tile previous;

	int i = 0;

	do
	{
		tiles[current.y][current.x].solid = false;
		
		//rand dir
		int d = rand()%4;

		int count = 0;
		previous = current;

		switch (d)
		{
		case 0:
			case0:
			if (current.x - 1 > 0)
			{
					current = tiles[current.y][current.x-1];
					break;
			}
			count++;
			if (count > 3) break;

		case 1:
			if (current.x + 1 < this->XSize-1)
			{
					current = tiles[current.y][current.x+1];
					break;
			}
			count++;
			if (count > 3) break;

		case 2:
			if (current.y - 1 > 0)
			{
					current = tiles[current.y-1][current.x];
					break;
			}
			count++;
			if (count > 3) break;

		case 3:
			if (current.y + 1 < this->YSize-1)
			{
					current = tiles[current.y+1][current.x];
					break;
			}
			count++;
			if (count > 3) break;
			goto case0;
		}

		if (tiles[current.y][current.x].solid) i++;

	} while (i < numTries);

	EndX = previous.x;
	EndY = previous.y;

	return;
}

void MazeGenerator::DrunkenWalk2(int density, int numberStarts)
{

	//force start in middle (more interesting)
	StartX = (XSize/2)+1;
	StartY = (YSize/2)+1;
	int numTries = density;//(XSize*YSize)/2;

	Tile current(StartX,StartY);
	Tile previous;

	int i = 0;

	do
	{
		tiles[current.y][current.x].solid = false;

		if (numberStarts > 0)
		{
			bool contains = false;
			for (int i = 0; i < XStarts.size(); i++)
			{
				if (current.y ==  YStarts[i] && current.x == XStarts[i]) //if current point in vector
				{
					contains = true;
				}
			}
			if (!contains)
			{
				numberStarts--;
				XStarts.push_back(current.x);
				YStarts.push_back(current.y);
			}
		}
		
		//rand dir
		int d = rand()%4;

		int count = 0;
		previous = current;

		switch (d)
		{
		case 0:
			case0:
			if (current.x - 1 > 0)
			{
					current = tiles[current.y][current.x-1];
					break;
			}
			count++;
			if (count > 3) break;

		case 1:
			if (current.x + 1 < this->XSize-1)
			{
					current = tiles[current.y][current.x+1];
					break;
			}
			count++;
			if (count > 3) break;

		case 2:
			if (current.y - 1 > 0)
			{
					current = tiles[current.y-1][current.x];
					break;
			}
			count++;
			if (count > 3) break;

		case 3:
			if (current.y + 1 < this->YSize-1)
			{
					current = tiles[current.y+1][current.x];
					break;
			}
			count++;
			if (count > 3) break;
			goto case0;
		}

		if (tiles[current.y][current.x].solid) i++;

	} while (i < numTries);

	EndX = previous.x;
	EndY = previous.y;

	return;
}

vector<Tile> MazeGenerator::GetNeighbours(Tile n)
{
	vector<Tile> neighbourNodes;
	int y = n.y;
	int x = n.x;
	//up
	if (y-1 > -1)
		if (!tiles[y-1][x].solid)
		{
			Tile t;
			t.x = x;
			t.y = y-1;
			neighbourNodes.push_back(t);
		}
	//left
	if (x-1 > -1)
		if (!tiles[y][x-1].solid)
		{
			Tile t;
			t.x = x-1;
			t.y = y;
			neighbourNodes.push_back(t);
		}
	//right
	if (x+1 < tiles[y].size())
		if (!tiles[y][x+1].solid)
		{
			Tile t;
			t.x = x+1;
			t.y = y;
			neighbourNodes.push_back(t);
		}
	//down
	if (y+1 < tiles.size())
		if (!tiles[y+1][x].solid)
		{
			Tile t;
			t.x = x;
			t.y = y+1;
			neighbourNodes.push_back(t);
		}

	return neighbourNodes;
}

vector<Tile> MazeGenerator::GetDiagonalNeighbours(Tile n)
{
	vector<Tile> neighbourNodes;
	int y = n.y;
	int x = n.x;
	//up
	if (y-1 > -1)
	{
		if (!tiles[y-1][x].solid)
		{
			Tile t;
			t.x = x;
			t.y = y-1;
			neighbourNodes.push_back(t);
		}
		if (x-1 > -1)
			if (!tiles[y-1][x-1].solid)
			{
				Tile t;
				t.x = x-1;
				t.y = y-1;
				neighbourNodes.push_back(t);
			}
		if (x+1 < tiles[y].size())
			if (!tiles[y-1][x+1].solid)
			{
				Tile t;
				t.x = x-1;
				t.y = y-1;
				neighbourNodes.push_back(t);
			}
	}
	//left
	if (x-1 > -1)
		if (!tiles[y][x-1].solid)
		{
			Tile t;
			t.x = x-1;
			t.y = y;
			neighbourNodes.push_back(t);
		}
	//right
	if (x+1 < tiles[y].size())
		if (!tiles[y][x+1].solid)
		{
			Tile t;
			t.x = x+1;
			t.y = y;
			neighbourNodes.push_back(t);
		}
	//down
	if (y+1 < tiles.size())
	{
		if (!tiles[y+1][x].solid)
		{
			Tile t;
			t.x = x;
			t.y = y+1;
			neighbourNodes.push_back(t);
		}
		
		if (x-1 > -1)
			if (!tiles[y+1][x-1].solid)
			{
				Tile t;
				t.x = x-1;
				t.y = y+1;
				neighbourNodes.push_back(t);
			}
		if (x+1 < tiles[y].size())
			if (!tiles[y+1][x+1].solid)
			{
				Tile t;
				t.x = x-1;
				t.y = y+1;
				neighbourNodes.push_back(t);
			}
	}

	return neighbourNodes;
}

void MazeGenerator::CalculateOutsideMap()
{
	//left->right
	for (int i=0;i<tiles.size();i++)
		for (int j=0;j<tiles[i].size();j++)
			if (tiles[i][j].solid)
				j = tiles[i].size();
			else tiles[i][j].outsideOfMap = true;

	//right->left
	for (int i=tiles.size()-1;i>=0;i--)
		for (int j=tiles[i].size()-1;j>=0;j--)
			if (tiles[i][j].solid)
				j = -1;
			else tiles[i][j].outsideOfMap = true;

}

void MazeGenerator::AddBuildings(int probability)
{
	//4x3
	int numFourInARow = 0;
	int numThreeInARow = 0;
	for (int i=0;i<tiles.size();i++)
		for (int j=0;j<tiles[i].size();j++)
		{
			if (!tiles[i][j].outsideOfMap && !tiles[i][j].marked)
				numFourInARow++;
			else
			{
				numFourInARow = 0;
				numThreeInARow = 0;
			}

			if (numFourInARow == 4)
			{
				numThreeInARow++;
				i++;
				numFourInARow = 0;
					
				if (numThreeInARow == 3)
				{
					if (rand()%probability == 0)
					{
						tiles[i-2][j-3].isBuilding = true;
					}
					else
					{
						i -= numThreeInARow;
						j -= numFourInARow + 1;
						numFourInARow = 0;
						numThreeInARow = 0;
					}
				}
			}
		}
}

#endif

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