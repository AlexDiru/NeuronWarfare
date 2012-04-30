#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "SearchCell.h"
#include "Vector3.h"
#include <vector>

class PathFinding
{
public:
	PathFinding() {}

	PathFinding(const std::vector<std::string>& map, const std::string& unwalkableTiles, int Width, int Height)
				: XSize(Width), YSize(Height), Map(map), UnwalkableTiles(unwalkableTiles) 
	{
		m_intializedStartGoal = false;
		m_foundGoal = false;
	}
	~PathFinding(void);

	int XSize, YSize; //Map dimensions
	void FindPath(Vector3 currentPos, Vector3 targetPos);
	Vector3 NextPathPos();
	void ClearOpenList() { m_openList.clear(); }
	void ClearVisitedList() { m_visitedList.clear(); }
	void ClearPathToGoal() { m_pathToGoal.clear(); }
	bool m_intializedStartGoal;
	bool m_foundGoal;
	std::vector<Vector3*> m_pathToGoal;
	std::vector<std::string> Map;
	SearchCell *m_goalCell;
	std::string UnwalkableTiles; //tiles that are classed as solid

private:
	void SetStartAndGoal(SearchCell start, SearchCell goal);
	void PathOpened(int x, int z, float newCost, SearchCell *parent);
	SearchCell *GetNextCell();
	void ContinuePath();

	SearchCell *m_startCell;
	std::vector<SearchCell*> m_openList;
	std::vector<SearchCell*> m_visitedList;
};

#endif