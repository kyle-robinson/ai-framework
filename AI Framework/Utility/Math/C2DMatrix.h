#pragma once
#ifndef C2DMATRIX_H
#define C2DMATRIX_H

//------------------------------------------------------------------------
//  Author: Mat Buckland 2002
//------------------------------------------------------------------------

#include <math.h>
#include <vector>
#include "Utility.h"
#include "Vector2D.h"

class C2DMatrix
{
private:
    struct Matrix
    {
        float _11, _12, _13;
        float _21, _22, _23;
        float _31, _32, _33;

        Matrix()
        {
            _11 = 0.0; _12 = 0.0; _13 = 0.0;
            _21 = 0.0; _22 = 0.0; _23 = 0.0;
            _31 = 0.0; _32 = 0.0; _33 = 0.0;
        }
    };
    Matrix m_Matrix;
    inline void  MatrixMultiply( Matrix& mIn );

public:

    C2DMatrix() { Identity(); }

    // create an identity matrix
    inline void Identity();

    // create a transformation matrix
    inline void Translate( float x, float y );

    // create a scale matrix
    inline void Scale( float xScale, float yScale );

    // create a rotation matrix
    inline void  Rotate( float rotation );

    // create a rotation matrix from a fwd and side 2D vector
    inline void  Rotate( const Vector2D& fwd, const Vector2D& side );

    // applys a transformation matrix to a std::vector of points
    inline void TransformVector2Ds( std::vector<Vector2D>& vPoints );

    // applys a transformation matrix to a point
    inline void TransformVector2Ds( Vector2D& vPoint );

    // accessors to the matrix elements
    void _11( float val ) { m_Matrix._11 = val; }
    void _12( float val ) { m_Matrix._12 = val; }
    void _13( float val ) { m_Matrix._13 = val; }

    void _21( float val ) { m_Matrix._21 = val; }
    void _22( float val ) { m_Matrix._22 = val; }
    void _23( float val ) { m_Matrix._23 = val; }

    void _31( float val ) { m_Matrix._31 = val; }
    void _32( float val ) { m_Matrix._32 = val; }
    void _33( float val ) { m_Matrix._33 = val; }
};

// multiply two matrices together
inline void C2DMatrix::MatrixMultiply( Matrix& mIn )
{
    C2DMatrix::Matrix mat_temp;

    mat_temp._11 = ( m_Matrix._11 * mIn._11 ) + ( m_Matrix._12 * mIn._21 ) + ( m_Matrix._13 * mIn._31 );
    mat_temp._12 = ( m_Matrix._11 * mIn._12 ) + ( m_Matrix._12 * mIn._22 ) + ( m_Matrix._13 * mIn._32 );
    mat_temp._13 = ( m_Matrix._11 * mIn._13 ) + ( m_Matrix._12 * mIn._23 ) + ( m_Matrix._13 * mIn._33 );

    mat_temp._21 = ( m_Matrix._21 * mIn._11 ) + ( m_Matrix._22 * mIn._21 ) + ( m_Matrix._23 * mIn._31 );
    mat_temp._22 = ( m_Matrix._21 * mIn._12 ) + ( m_Matrix._22 * mIn._22 ) + ( m_Matrix._23 * mIn._32 );
    mat_temp._23 = ( m_Matrix._21 * mIn._13 ) + ( m_Matrix._22 * mIn._23 ) + ( m_Matrix._23 * mIn._33 );

    mat_temp._31 = ( m_Matrix._31 * mIn._11 ) + ( m_Matrix._32 * mIn._21 ) + ( m_Matrix._33 * mIn._31 );
    mat_temp._32 = ( m_Matrix._31 * mIn._12 ) + ( m_Matrix._32 * mIn._22 ) + ( m_Matrix._33 * mIn._32 );
    mat_temp._33 = ( m_Matrix._31 * mIn._13 ) + ( m_Matrix._32 * mIn._23 ) + ( m_Matrix._33 * mIn._33 );

    m_Matrix = mat_temp;
}

// applies a 2D transformation matrix to a std::vector of Vector2Ds
inline void C2DMatrix::TransformVector2Ds( std::vector<Vector2D>& vPoint )
{
    for ( uint32_t i = 0u; i < vPoint.size(); ++i )
    {
        float tempX = ( m_Matrix._11 * vPoint[i].x ) + ( m_Matrix._21 * vPoint[i].y ) + ( m_Matrix._31 );
        float tempY = ( m_Matrix._12 * vPoint[i].x ) + ( m_Matrix._22 * vPoint[i].y ) + ( m_Matrix._32 );
        vPoint[i].x = tempX;
        vPoint[i].y = tempY;
    }
}

// applies a 2D transformation matrix to a single Vector2D
inline void C2DMatrix::TransformVector2Ds( Vector2D& vPoint )
{
    float tempX = ( m_Matrix._11 * vPoint.x ) + ( m_Matrix._21 * vPoint.y ) + ( m_Matrix._31 );
    float tempY = ( m_Matrix._12 * vPoint.x ) + ( m_Matrix._22 * vPoint.y ) + ( m_Matrix._32 );
    vPoint.x = tempX;
    vPoint.y = tempY;
}

// create an identity matrix
inline void C2DMatrix::Identity()
{
    m_Matrix._11 = 1; m_Matrix._12 = 0; m_Matrix._13 = 0;
    m_Matrix._21 = 0; m_Matrix._22 = 1; m_Matrix._23 = 0;
    m_Matrix._31 = 0; m_Matrix._32 = 0; m_Matrix._33 = 1;
}

// create a transformation matrix
inline void C2DMatrix::Translate( float x, float y )
{
    Matrix mat;
    mat._11 = 1; mat._12 = 0; mat._13 = 0;
    mat._21 = 0; mat._22 = 1; mat._23 = 0;
    mat._31 = x;    mat._32 = y;    mat._33 = 1;
    MatrixMultiply( mat );
}

//create a scale matrix
inline void C2DMatrix::Scale( float xScale, float yScale )
{
    C2DMatrix::Matrix mat;
    mat._11 = xScale; mat._12 = 0; mat._13 = 0;
    mat._21 = 0; mat._22 = yScale; mat._23 = 0;
    mat._31 = 0; mat._32 = 0; mat._33 = 1;
    MatrixMultiply( mat );
}

//create a rotation matrix
inline void C2DMatrix::Rotate( float rot )
{
    C2DMatrix::Matrix mat;

    float Sin = sin( rot );
    float Cos = cos( rot );

    mat._11 = Cos;  mat._12 = Sin; mat._13 = 0;
    mat._21 = -Sin; mat._22 = Cos; mat._23 = 0;
    mat._31 = 0; mat._32 = 0; mat._33 = 1;

    MatrixMultiply( mat );
}

// create a rotation matrix from a 2D vector
inline void C2DMatrix::Rotate( const Vector2D& fwd, const Vector2D& side )
{
    C2DMatrix::Matrix mat;
    mat._11 = fwd.x;  mat._12 = fwd.y; mat._13 = 0;
    mat._21 = side.x; mat._22 = side.y; mat._23 = 0;
    mat._31 = 0; mat._32 = 0; mat._33 = 1;
    MatrixMultiply( mat );
}

#endif