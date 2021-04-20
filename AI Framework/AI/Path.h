#pragma once
#ifndef PATH_H
#define PATH_H

#include <list>
#include <cassert>
#include "Math/Vector2D.h"

class Path
{
public:
	Path() {}
    Vector2D CurrentWaypoint() const noexcept { return *curWaypoint; }
    bool Finished() { return !( curWaypoint != m_WayPoints.end() ); }
    void SetNextWaypoint()
    {
        assert( m_WayPoints.size() > 0 );
        if ( ++curWaypoint == m_WayPoints.end() )
            curWaypoint = m_WayPoints.begin();
    }
    void Clear() { m_WayPoints.clear(); }
    std::vector<Vector2D> GetPath() const noexcept { return m_WayPoints; }
    void AddWayPoint( Vector2D new_point ) { m_WayPoints.push_back( new_point ); };
    void Set( std::vector<Vector2D> new_path ) { m_WayPoints = new_path; curWaypoint = m_WayPoints.begin(); }
private:
	std::vector<Vector2D> m_WayPoints;
	std::vector<Vector2D>::iterator curWaypoint;
};

#endif