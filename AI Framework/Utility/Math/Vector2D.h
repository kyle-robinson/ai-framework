#pragma once
#ifndef VECTOR2D_H
#define VECTOR2D_H

//------------------------------------------------------------------------
//  Vector2d - Author: Mat Buckland
//------------------------------------------------------------------------

#include <limits>
#include <windows.h>

const float Minfloat = ( std::numeric_limits<float>::min )( );

inline bool isEqual( float a, float b )
{
    if ( fabs( a - b ) < 1E-12 )
        return true;

    return false;
}

struct Vector2D
{
    float x;
    float y;

    Vector2D() :x( 0.0 ), y( 0.0 ) {}
    Vector2D( float a, float b ) :x( a ), y( b ) {}

    void Zero() { x = 0.0; y = 0.0; }
    bool isZero()const { return ( x * x + y * y ) < Minfloat; }

    inline float Length()const;
    inline float LengthSq()const;
    inline void Normalize();
    inline float Dot( const Vector2D& v2 )const;

    inline int Sign( const Vector2D& v2 )const;
    inline Vector2D Perp()const;
    inline void Truncate( float max );

    inline float Distance( const Vector2D& v2 )const;
    inline float DistanceSq( const Vector2D& v2 )const;

    inline void Reflect( const Vector2D& norm );
    inline Vector2D  GetReverse()const;

    const Vector2D& operator+=( const Vector2D& rhs )
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    const Vector2D& operator-=( const Vector2D& rhs )
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    const Vector2D& operator*=( const Vector2D& rhs )
    {
        x *= rhs.x;
        y *= rhs.y;

        return *this;
    }

    const Vector2D& operator*=( const float& rhs )
    {
        x *= rhs;
        y *= rhs;

        return *this;
    }

    const Vector2D& operator/=( const float& rhs )
    {
        x /= rhs;
        y /= rhs;

        return *this;
    }

    bool operator==( const Vector2D& rhs )const
    {
        return ( isEqual( x, rhs.x ) && isEqual( y, rhs.y ) );
    }

    bool operator!=( const Vector2D& rhs )const
    {
        return ( x != rhs.x ) || ( y != rhs.y );
    }
};

inline Vector2D operator*( const Vector2D& lhs, float rhs );
inline Vector2D operator*( float lhs, const Vector2D& rhs );
inline Vector2D operator-( const Vector2D& lhs, const Vector2D& rhs );
inline Vector2D operator+( const Vector2D& lhs, const Vector2D& rhs );
inline Vector2D operator/( const Vector2D& lhs, float val );
std::ostream& operator<<( std::ostream& os, const Vector2D& rhs );
std::ifstream& operator>>( std::ifstream& is, Vector2D& lhs );

#pragma region MEMBER_FUNCTIONS

inline float Vector2D::Length()const
{
    return sqrt( x * x + y * y );
}

inline float Vector2D::LengthSq()const
{
    return ( x * x + y * y );
}

inline float Vector2D::Dot( const Vector2D& v2 )const
{
    return x * v2.x + y * v2.y;
}

enum { clockwise = 1, anticlockwise = -1 };

inline int Vector2D::Sign( const Vector2D& v2 )const
{
    if ( y * v2.x > x * v2.y )
    {
        return clockwise;
    }
    else
    {
        return anticlockwise;
    }
}

inline Vector2D Vector2D::Perp()const
{
    return Vector2D( -y, x );
}

inline float Vector2D::Distance( const Vector2D& v2 )const
{
    float ySeparation = v2.y - y;
    float xSeparation = v2.x - x;

    return sqrt( ySeparation * ySeparation + xSeparation * xSeparation );
}

inline float Vector2D::DistanceSq( const Vector2D& v2 )const
{
    float ySeparation = v2.y - y;
    float xSeparation = v2.x - x;

    return ySeparation * ySeparation + xSeparation * xSeparation;
}

inline void Vector2D::Truncate( float max )
{
    if ( this->Length() > max )
    {
        this->Normalize();

        *this *= max;
    }
}

inline void Vector2D::Reflect( const Vector2D& norm )
{
    *this += 2.0 * this->Dot( norm ) * norm.GetReverse();
}

inline Vector2D Vector2D::GetReverse()const
{
    return Vector2D( -this->x, -this->y );
}

inline void Vector2D::Normalize()
{
    float vector_length = this->Length();

    if ( vector_length > std::numeric_limits<float>::epsilon() )
    {
        this->x /= vector_length;
        this->y /= vector_length;
    }
}

#pragma endregion



#pragma region NON_MEMBER_FUNCTIONS

inline Vector2D Vec2DNormalize( const Vector2D& v )
{
    Vector2D vec = v;

    float vector_length = vec.Length();

    if ( vector_length > std::numeric_limits<float>::epsilon() )
    {
        vec.x /= vector_length;
        vec.y /= vector_length;
    }

    return vec;
}

inline float Vec2DDistance( const Vector2D& v1, const Vector2D& v2 )
{

    float ySeparation = v2.y - v1.y;
    float xSeparation = v2.x - v1.x;

    return sqrt( ySeparation * ySeparation + xSeparation * xSeparation );
}

inline float Vec2DDistanceSq( const Vector2D& v1, const Vector2D& v2 )
{

    float ySeparation = v2.y - v1.y;
    float xSeparation = v2.x - v1.x;

    return ySeparation * ySeparation + xSeparation * xSeparation;
}

inline float Vec2DLength( const Vector2D& v )
{
    return sqrt( v.x * v.x + v.y * v.y );
}

inline float Vec2DLengthSq( const Vector2D& v )
{
    return ( v.x * v.x + v.y * v.y );
}


inline Vector2D POINTStoVector( const POINTS& p )
{
    return Vector2D( p.x, p.y );
}

inline Vector2D POINTtoVector( const POINT& p )
{
    return Vector2D( static_cast<float>( p.x ), static_cast<float>( p.y ) );
}

inline POINTS VectorToPOINTS( const Vector2D& v )
{
    POINTS p;
    p.x = static_cast<short>( v.x );
    p.y = static_cast<short>( v.y );

    return p;
}

inline POINT VectorToPOINT( const Vector2D& v )
{
    POINT p;
    p.x = static_cast<long>( v.x );
    p.y = static_cast<long>( v.y );

    return p;
}

#pragma endregion



#pragma region OPERATOR_OVERLOADS

inline Vector2D operator*( const Vector2D& lhs, float rhs )
{
    Vector2D result( lhs );
    result *= rhs;
    return result;
}

inline Vector2D operator*( float lhs, const Vector2D& rhs )
{
    Vector2D result( rhs );
    result *= lhs;
    return result;
}

inline Vector2D operator-( const Vector2D& lhs, const Vector2D& rhs )
{
    Vector2D result( lhs );
    result.x -= rhs.x;
    result.y -= rhs.y;

    return result;
}

inline Vector2D operator+( const Vector2D& lhs, const Vector2D& rhs )
{
    Vector2D result( lhs );
    result.x += rhs.x;
    result.y += rhs.y;

    return result;
}

inline Vector2D operator/( const Vector2D& lhs, float val )
{
    Vector2D result( lhs );
    result.x /= val;
    result.y /= val;

    return result;
}

inline void WrapAround( Vector2D& pos, int MaxX, int MaxY )
{
    if ( pos.x > MaxX ) { pos.x = -MaxX; }
    if ( pos.x < -MaxX ) { pos.x = static_cast<float>( MaxX ); }
    if ( pos.y < -MaxY ) { pos.y = static_cast<float>( MaxY ); }
    if ( pos.y > MaxY ) { pos.y = -MaxY; }
}

#pragma endregion

#endif