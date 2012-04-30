#pragma once

#include <iostream>
#include <vector>
#include <string>

//makes sure the given coords won't break an array
bool boundaryCheck(int x,int y,int XSize,int YSize);

struct v2d { int X, Y; };

void GetFreeCellsRecurse( const std::vector<std::string>& map, int prevx, int prevy, int x, int y, int XSize, int YSize, int &number, std::vector<v2d>& stack );

//algorithm, given OPEN cell coordinates, find space around that cell that are open
std::vector<v2d> GetFreeCells( const std::vector<std::string>& map, int x, int y, int number);