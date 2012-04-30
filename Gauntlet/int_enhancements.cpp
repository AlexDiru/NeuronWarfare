#include "int_enhancements.h"

std::string toString( int n )
{
	char buffer[30];
	memset(buffer,'\0',30);
	return (std::string)itoa(n,buffer,10);
}