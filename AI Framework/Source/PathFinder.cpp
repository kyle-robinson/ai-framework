#include "PathFinder.h"

PathFinder::PathFinder()
{
	//SetEdgeCosts();
}

PathFinder::~PathFinder()
{
	EDGECOST_List.clear();
	OPEN_List.clear();
	CLOSED_List.clear();
}

/*std::vector<Vector2D> PathFinder::GetPathBetween( Vector2D startPosition, Vector2D endPosition )
{
	OPEN_List.clear();
	CLOSED_List.clear();

	std::vector<Vector2D> path;
	Waypoint* nearestToCar = GetNearestWaypointToPosition( startPosition );
	Waypoint* nearestToEnd = GetNearestWaypointToPosition( endPosition );

	if ( nearestToCar == NULL ||
		 nearestToEnd == NULL ||
		 nearestToCar == nearestToEnd )
	{
		path.push_back( endPosition );
		return path;
	}

	AStarNode* startWaypoint;
	startWaypoint->parentWaypoint = NULL;
	startWaypoint->cost = 0.0;

	OPEN_List.push_back( new AStarNode( nearestToCar, NULL, 0.0 ) );

	AStarNode* currentNode;
	while ( !OPEN_List.empty() )
	{
		for ( uint32_t i = 0u; i < OPEN_List.size(); i++ )
		{
			if ( currentNode == NULL || OPEN_List[i]->cost < currentNode->cost )
			{
				currentNode = OPEN_List[i];
			}
			if ( currentNode == OPEN_List.back() )
			{
				path = ConstructPath( currentNode, endPosition );
				return path;
			}
			double g = currentNode->cost;
			double h = GetHeuristicCost(
				Vector2D( currentNode->internalWaypoint->getPosition()->x, currentNode->internalWaypoint->getPosition()->y ),
				Vector2D( nearestToEnd->getPosition()->x, nearestToEnd->getPosition()->y ) );
			double f = g + h;
			AStarNode* connectedWaypoint = currentNode->parentWaypoint;
			connectedWaypoint->cost = f;
			OPEN_List.push_back( std::move( connectedWaypoint ) );
		}
		std::vector<AStarNode*>::iterator it = OPEN_List.begin();
		while ( it != OPEN_List.end() )
		{
			if ( *it == currentNode )
				it = OPEN_List.erase( it );
			else
				++it;
		}
		currentNode = NULL;
	}
	
	return path;
}

void PathFinder::SetEdgeCosts()
{
	vecWaypoints waypoints = aiManager.GetWaypoints();

	for ( uint32_t i = 0u; i < waypoints.size(); i++ )
	{
		vecWaypoints neighbours = aiManager.GetNeighbours( waypoints[i]->getPosition()->x, waypoints[i]->getPosition()->y );
		for ( uint32_t j = 0u; j < neighbours.size(); j++ )
		{
			Waypoint* waypointFrom = waypoints[i];
			Waypoint* waypointTo = waypoints[j];
			EDGECOST_List.push_back( new EdgeCost( waypointFrom, waypointTo, 1.0f ) );
		}
	}
}

Waypoint* PathFinder::GetNearestWaypointToPosition( Vector2D position )
{
	vecWaypoints waypoints = aiManager.GetWaypoints();

	float nearestDist = FLT_MAX;
	Waypoint* nearestWp = NULL;

	for ( uint32_t i = 0u; i < waypoints.size(); i++ )
	{
		Vector2D vecBetweenPoints = position - Vector2D( waypoints[i]->getPosition()->x, waypoints[i]->getPosition()->y );
		double vecLength = vecBetweenPoints.Length();
		if ( vecLength < nearestDist )
		{
			nearestDist = vecLength;
			nearestWp = waypoints[i];
		}
	}
	
	return nearestWp;
}

double PathFinder::GetCostBetweenWaypoints( Waypoint* from, Waypoint* to )
{
	for ( uint32_t i = 0u; i < EDGECOST_List.size(); i++ )
		if ( EDGECOST_List[i]->wayPointFROM == from || EDGECOST_List[i]->wayPointTO == to )
			return EDGECOST_List[i]->cost;
	return FLT_MAX;
}

std::vector<Vector2D> PathFinder::ConstructPath( AStarNode* targetNode, Vector2D endPos )
{
	std::vector<Vector2D> path, pathInReverse;
	pathInReverse.push_back( endPos );

	AStarNode* currentNode = targetNode;

	while ( currentNode != NULL )
	{
		pathInReverse.push_back( Vector2D( currentNode->internalWaypoint->getPosition()->x,
			currentNode->internalWaypoint->getPosition()->y ) );
		currentNode = currentNode->parentWaypoint;
	}

	std::vector<Vector2D>::iterator it = pathInReverse.end();
	while ( it != pathInReverse.begin() )
		path.push_back( *( --it ) );

	return path;
}

bool PathFinder::IsInList( std::vector<AStarNode*> listToCheck, Waypoint* waypointToCheck )
{
	for ( uint32_t i = 0u; i < listToCheck.size(); i++ )
		if ( listToCheck[i]->internalWaypoint == waypointToCheck )
			return true;
	return false;
}

double PathFinder::GetHeuristicCost( Vector2D pos1, Vector2D pos2 )
{
	Vector2D vecBetweenPoints = pos2 - pos1;
	return vecBetweenPoints.Length();
}*/