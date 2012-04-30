#include "GetFreeCells.h"

//makes sure the given coords won't break an array
bool boundaryCheck(int x,int y,int XSize,int YSize)
{
	return (x>-1&&x<XSize&&y>-1&&y<YSize);
}

void GetFreeCellsRecurse( const std::vector<std::string>& map, int prevx, int prevy, int x, int y, int XSize, int YSize, int &number, std::vector<v2d>& stack )
{
	if (number < 1)
		return;
	else
	{
		for (int Y=-1;Y<2 && number > 0;Y++)
			for (int X=-1;X<2 && number > 0; X++)
				if ( abs(X) != abs(Y) )
					//check all directions except the direction previously came from
					if (boundaryCheck(x+X,y+Y,XSize,YSize))
						if ( ! (x+X == prevx && y+Y == prevy) ) //left
							if ( map[y+Y][x+X] == '.' )
							{
								v2d temp;
								temp.X=x+X;
								temp.Y=y+Y;
								number--;
								stack.push_back(temp);
								GetFreeCellsRecurse(map,temp.X,temp.Y,temp.X,temp.Y,XSize,YSize,number,stack);
							}
	}
}

//algorithm, given OPEN cell coordinates, find space around that cell that are open
std::vector<v2d> GetFreeCells( const std::vector<std::string>& map, int x, int y, int number)
{
	std::vector<v2d> stack;
	GetFreeCellsRecurse(map,x,y,x,y,map[0].size(),map.size(),number,stack);
	return stack;
}