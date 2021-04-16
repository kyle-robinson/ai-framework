#pragma once
#ifndef ENTITYTEMPLATES_H
#define ENTITYTEMPLATES_H

#include "DrawableGameObject.h"

inline bool TwoCirclesOverlapped( Vector2D c1, float r1, Vector2D c2, float r2 )
{
    float DistBetweenCenters = sqrt( ( c1.x - c2.x ) * ( c1.x - c2.x ) +
        ( c1.y - c2.y ) * ( c1.y - c2.y ) );

    if ( ( DistBetweenCenters < ( r1 + r2 ) ) || ( DistBetweenCenters < fabs( r1 - r2 ) ) )
    {
        return true;
    }

    return false;
}

template <class T, class conT>
bool Overlapped( const T* ob, const conT& conOb, float MinDistBetweenObstacles = 40.0 )
{
    typename conT::const_iterator it;

    for ( it = conOb.begin(); it != conOb.end(); ++it )
    {
        if ( TwoCirclesOverlapped( ob->GetPosition(),
            ob->GetBoundingRadius() + MinDistBetweenObstacles,
            ( *it )->GetPosition(),
            ( *it )->GetBoundingRadius() ) )
        {
            return true;
        }
    }

    return false;
}

#endif