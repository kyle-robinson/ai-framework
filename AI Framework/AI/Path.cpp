#include "Path.h"
#include "Cgdi.h"
#include "Math/Transformations.h"
#include <cassert>

void Path::SetNextWaypoint()
{
    assert( m_WayPoints.size() > 0 );

    if ( ++curWaypoint == m_WayPoints.end() )
    {
        if ( m_bLooped )
        {
            curWaypoint = m_WayPoints.begin();
        }
    }
}

std::list<Vector2D> Path::CreateRandomPath( int NumWaypoints,
    double MinX, double MinY, double MaxX, double MaxY )
{
    m_WayPoints.clear();

    double midX = ( MaxX + MinX ) / 2.0;
    double midY = ( MaxY + MinY ) / 2.0;

    double smaller = min( midX, midY );
    double spacing = TwoPi / ( double )NumWaypoints;

    for ( int i = 0; i < NumWaypoints; ++i )
    {
        double RadialDist = RandInRange( smaller * 0.2f, smaller );
        Vector2D temp( RadialDist, 0.0f );
        Vec2DRotateAroundOrigin( temp, i * spacing );
        temp.x += midX; temp.y += midY;
        m_WayPoints.push_back( temp );
    }

    curWaypoint = m_WayPoints.begin();
    return m_WayPoints;
}