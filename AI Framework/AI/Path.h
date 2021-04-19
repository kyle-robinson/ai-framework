#pragma once
#ifndef PATH_H
#define PATH_H

#include <list>
#include "Math/Vector2D.h"

class Path
{
public:
	Path() : m_bLooped( false ) {}
    Path( int NumWaypoints, double MinX, double MinY, double MaxX, double MaxY, bool looped ) : m_bLooped( looped )
    {
        CreateRandomPath( NumWaypoints, MinX, MinY, MaxX, MaxY );
        curWaypoint = m_WayPoints.begin();
    }

    Vector2D CurrentWaypoint()const { return *curWaypoint; }
    bool Finished() { return !( curWaypoint != m_WayPoints.end() ); }
    void SetNextWaypoint();
    std::list<Vector2D> CreateRandomPath( int NumWaypoints,
        double MinX, double MinY, double MaxX, double MaxY );

    void LoopOn() { m_bLooped = true; }
    void LoopOff() { m_bLooped = false; }

    void AddWayPoint( Vector2D new_point ) { m_WayPoints.push_back( new_point ); };
    void Set( std::list<Vector2D> new_path ) { m_WayPoints = new_path; curWaypoint = m_WayPoints.begin(); }
    void Set( const Path& path ) { m_WayPoints = path.GetPath(); curWaypoint = m_WayPoints.begin(); }

    void Clear() { m_WayPoints.clear(); }
    std::list<Vector2D> GetPath()const { return m_WayPoints; }
private:
	bool m_bLooped;
	std::list<Vector2D> m_WayPoints;
	std::list<Vector2D>::iterator curWaypoint;
};

#endif