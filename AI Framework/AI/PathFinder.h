#pragma once
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Waypoint.h"
#include "AIManager.h"

struct EdgeCost
{
	Waypoint* wayPointFROM;
	Waypoint* wayPointTO;
	double cost;

	EdgeCost( Waypoint* from, Waypoint* to, double newCost )
	{
		wayPointFROM = from;
		wayPointTO = to;
		cost = newCost;
	}
};

struct AStarNode
{
	Waypoint* internalWaypoint;
	AStarNode* parentWaypoint;
	double cost;

	AStarNode( Waypoint* currentWaypoint, AStarNode* parentalWaypoint, double newCost )
	{
		internalWaypoint = currentWaypoint;
		parentWaypoint = parentalWaypoint;
		cost = newCost;
	}
};

class Vector2D;

class PathFinder
{
public:
	PathFinder();
	~PathFinder();
	std::vector<Vector2D> GetPathBetween( Vector2D startPosition, Vector2D endPosition );
private:
	void SetEdgeCosts();
	vecWaypoints GetNeighbours( const int x, const int y );
	Waypoint* GetNearestWaypointToPosition( Vector2D position );
	
	double GetCostBetweenWaypoints( Waypoint* from, Waypoint* to );
	std::vector<Vector2D> ConstructPath( AStarNode* targetNode, Vector2D endPos );

	bool IsInList( std::vector<AStarNode*> listToCheck, Waypoint* waypointToCheck );
	double GetHeuristicCost( Vector2D pos1, Vector2D pos2 );
private:
	AIManager aiManager;
	std::vector<EdgeCost*> EDGECOST_List;
	std::vector<AStarNode*> OPEN_List;
	std::vector<AStarNode*> CLOSED_List;
};

#endif