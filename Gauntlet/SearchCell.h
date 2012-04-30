#ifndef SEARCHCELL_H
#define SEARCHCELL_H

#include <math.h>

struct SearchCell
{
public:
	int m_xcoord, m_zcoord;
	int m_id;
	SearchCell *parent;
	float G;
	float H;

	SearchCell() : parent(0) {}
	SearchCell(int x, int z, SearchCell *_parent = 0, int Width=0) 
		: m_xcoord(x), 
		  m_zcoord(z),
		  parent(_parent), 
		  m_id(z * Width + x), 
		  G(0), 
		  H(0) {}

	float GetF() { return G + H; }
	int ManHattanDistnce(SearchCell *nodeEnd)
	{
		int x = (abs((this->m_xcoord - nodeEnd->m_xcoord)));
		int z = (abs((this->m_zcoord - nodeEnd->m_zcoord)));

		return x + z;
	}
};

#endif