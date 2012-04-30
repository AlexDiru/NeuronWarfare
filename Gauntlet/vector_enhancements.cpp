#include "vector_enhancements.h"

std::string VectorStringToString( const std::vector<std::string>& vstr)
{
	std::string str = "";

	for (int i =0; i < vstr.size(); i++)
		str += vstr[i];

	return str;
}