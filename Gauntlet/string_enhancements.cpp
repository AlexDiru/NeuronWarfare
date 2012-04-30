#include "string_enhancements.h"

std::wstring wstring_cast( const std::string& str )
{
	std::wstring wstr(str.length(),L' ');
	std::copy(str.begin(),str.end(),wstr.begin());
	return wstr;
}