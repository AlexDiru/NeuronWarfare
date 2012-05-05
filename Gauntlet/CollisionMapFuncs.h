#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdarg>

//gets the collision map of an input map
std::vector<std::string> getCollisionMap( const std::vector<std::vector<int>>& tiles, const std::vector<int>& solidTiles, const std::vector<int>& coverTiles );

//appends one collision map to another
void appendCollisionMap(std::vector<std::string>& target, std::vector<std::string>& tm);