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

template <class T, class conT>
void TagNeighbors( const T& entity, conT& ContainerOfEntities, float radius )
{
    //iterate through all entities checking for range
    for ( typename conT::iterator curEntity = ContainerOfEntities.begin();
        curEntity != ContainerOfEntities.end();
        ++curEntity )
    {
        //first clear any current tag
        ( *curEntity )->UnTag();

        Vector2D to = ( *curEntity )->GetPosition() - entity->GetPosition();

        //the bounding radius of the other is taken into account by adding it 
        //to the range
        float range = radius + ( *curEntity )->GetBoundingRadius();

        //if entity within range, tag for further consideration. (working in
        //distance-squared space to avoid sqrts)
        if ( ( ( *curEntity ) != entity ) && ( to.LengthSq() < range * range ) )
        {
            ( *curEntity )->Tag();
        }

    }//next entity
}

#endif