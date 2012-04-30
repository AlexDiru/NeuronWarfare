#include "math_enhancements.h"

int round (float f)
{
	return (f < ((int)f + 0.5)) ? (int)(f) : (int)(f)+1;
}

int get_sign ( float f )
{
	if ( f == 0 ) return 0;
	else if ( f < 0 ) return -1;
	else return 1;
}

int get_sign ( float f, float max_error )
{
	if ( f <= max_error || f >= -max_error  ) return 0;
	else if ( f < -max_error ) return -1;
	else return 1;
}

bool roughly_equal_to( float a, float b, float max_error )
{
	return a-b <= max_error;
}