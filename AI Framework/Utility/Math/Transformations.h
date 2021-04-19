#pragma once
#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

//------------------------------------------------------------------------
//  Author: Mat Buckland 2002
//------------------------------------------------------------------------

#include <vector>
#include "Vector2D.h"
#include "C2DMatrix.h"

inline Vector2D PointToWorldSpace( const Vector2D& point, const Vector2D& heading,
    const Vector2D& side, const Vector2D& pos )
{
    Vector2D newPoint = point;

    C2DMatrix matTransform;
    matTransform.Rotate( heading, side );
    matTransform.Translate( pos.x, pos.y );
    matTransform.TransformVector2Ds( newPoint );

    return newPoint;
}

inline Vector2D PointToLocalSpace( const Vector2D& point, const Vector2D& heading,
    const Vector2D& side, const Vector2D& position )
{
    Vector2D TransPoint = point;
    C2DMatrix matTransform;

    float Tx = -position.Dot( heading );
    float Ty = -position.Dot( side );

    // transformation matrix
    matTransform._11( heading.x );
    matTransform._12( side.x );

    matTransform._21( heading.y );
    matTransform._22( side.y );

    matTransform._31( Tx );
    matTransform._32( Ty );

    matTransform.TransformVector2Ds( TransPoint );
    return TransPoint;
}

inline Vector2D VectorToWorldSpace( const Vector2D& vec, const Vector2D& heading, const Vector2D& side )
{
    Vector2D newVec = vec;

    C2DMatrix matTransform;
    matTransform.Rotate( heading, side );
    matTransform.TransformVector2Ds( newVec );

    return newVec;
}

inline void Vec2DRotateAroundOrigin( Vector2D& v, double ang )
{
    C2DMatrix mat;
    mat.Rotate( ang );
    mat.TransformVector2Ds( v );
}

#endif