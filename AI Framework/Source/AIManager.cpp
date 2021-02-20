#include "AIManager.h"
#include "Vehicle.h"
#include "PickupItem.h"
#include "ErrorLogger.h"
#include "PathFinder.h"
#include "Vector2D.h"

#include "main.h"
#include <sstream>

Waypoint* AIManager::GetWaypoint( const int x, const int y )
{
    static bool runOnce = true;
    bool xOutOfRange = ( x < 0 || x > 19 ) ? true : false;
    bool yOutOfRange = ( y < 0 || y > 19 ) ? true : false;
    if ( ( xOutOfRange || yOutOfRange ) && runOnce )
    {
        ErrorLogger::Log( std::string( "Waypoint out of range! (" ) + std::to_string( x ) + ", " + std::to_string( y ) + ')' );
        runOnce = false;
        return nullptr;
    }
    return m_waypoints.at( y * WAYPOINT_RESOLUTION + x );
}

vecWaypoints AIManager::GetNeighbours( const int x, const int y )
{
    std::vector<Waypoint*> neighbours;
    int waypointIndex = y * WAYPOINT_RESOLUTION + x;

    for ( int i = x - 1; i <= x + 1; i++ )
    {
        for ( int j = y - 1; j <= y + 1; j++ )
        {
            int neighbourIndex = j * WAYPOINT_RESOLUTION + i;
            if ( waypointIndex != neighbourIndex )
            {
                if ( !m_waypoints[neighbourIndex] )
                    continue;
                neighbours.push_back( m_waypoints[neighbourIndex] );
            }
        }
    }

    return neighbours;
}

HRESULT AIManager::initialise(ID3D11Device* pd3dDevice, UINT width, UINT height)
{
    this->width = width;
    this->height = height;
    
    // create a pickup item ----------------------------------------------
    PickupItem* pPickup = new PickupItem();
    HRESULT hr = pPickup->initMesh(pd3dDevice);
    m_pickups.push_back(pPickup);


    // create the vehicle ------------------------------------------------
    float xPos = 0;
    float yPos = 0;

    m_pCar = new Vehicle();
    hr = m_pCar->initMesh(pd3dDevice, L"Resources\\Textures\\car_blue.dds");
    m_pCar->setPosition(XMFLOAT3(xPos, yPos, 0));
    if ( FAILED( hr ) ) return hr;

    m_pCar2 = new Vehicle();
    hr = m_pCar2->initMesh( pd3dDevice, L"Resources\\Textures\\car_red.dds" );
    m_pCar2->setPositionTo( XMFLOAT3( width / 2, 0.0f, 0 ) );
    if ( FAILED( hr ) ) return hr;

    // create the waypoints
    float xGap = SCREEN_WIDTH / WAYPOINT_RESOLUTION;
    float yGap = SCREEN_HEIGHT / WAYPOINT_RESOLUTION;
    float xStart = -(SCREEN_WIDTH / 2) + (xGap / 2);
    float yStart = -(SCREEN_HEIGHT / 2) + (yGap / 2);

    unsigned int index = 0;
    for (unsigned int j = 0; j < WAYPOINT_RESOLUTION; j++) {
        for (unsigned int i = 0; i < WAYPOINT_RESOLUTION; i++) {
            Waypoint* wp = new Waypoint();
            hr = wp->initMesh(pd3dDevice, index++);
            wp->setPosition(XMFLOAT3(xStart + (xGap * i), yStart + (yGap * j), 0));
            m_waypoints.push_back(wp);
        }
    }

    return hr;
}

void AIManager::update(const float fDeltaTime)
{
    // waypoints
    for (unsigned int i = 0; i < m_waypoints.size(); i++) {
        m_waypoints[i]->update(fDeltaTime);
        AddItemToDrawList(m_waypoints[i]); // if you comment this in, it will display the waypoints
    }

    AddItemToDrawList( GetWaypoint( 9, 1 ) );
    AddItemToDrawList( GetWaypoint( 5, 1 ) );

    vecWaypoints neighbours = GetNeighbours( 19, 10 );
    for ( unsigned int i = 0; i < neighbours.size(); i++ )
        AddItemToDrawList( neighbours[i] );

    // pickups
    for (unsigned int i = 0; i < m_pickups.size(); i++) {
        m_pickups[i]->update(fDeltaTime);
        AddItemToDrawList(m_pickups[i]);
    }

    // cars
    checkWallWrapping( m_pCar );
    m_pCar->update( fDeltaTime );
    checkForCollisions( m_pCar );
    AddItemToDrawList( m_pCar );


    //checkWallWrapping( m_pCar2 );
    m_pCar2->update( fDeltaTime );
    Wander( m_pCar2 );
    checkForCollisions( m_pCar2 );
    AddItemToDrawList( m_pCar2 );
}

void AIManager::LeftMouseUp( const int x, const int y )
{
    //m_pCar->setPositionTo( Vector2D( x, y ) );

    static PathFinder pathFinder;
    path.push_back(
        pathFinder.GetPathBetween(
            Vector2D( m_pCar->getPosition()->x, m_pCar->getPosition()->y ),
            Vector2D( x, y )
        )[0]
    );
}

void AIManager::RightMouseUp( const int x, const int y )
{
    //m_pCar2->setPositionTo( Vector2D( x, y ) );

    static int count = 0;
    if( count < path.size() )
        m_pCar->setPositionTo( path[count++] );
}

void AIManager::keyPress(WPARAM param)
{
    switch (param)
    {
        case VK_NUMPAD0:
        {
            OutputDebugStringA("0 pressed \n");
            break;
        }
        case VK_NUMPAD1:
        {
            // Vector2D Distance Calculation
            Vector2D vec1 = { 1.0f, 2.0f };
            Vector2D vec2 = { 7.0f, 4.0f };
            double distance = vec1.Distance( vec2 );
            wstringstream oss;
            oss << "Distance Between Vectors (" << vec1.x << ',' << vec1.y << ") + (" << vec2.x << ',' << vec2.y << ") = " << distance;
            MessageBox( nullptr, oss.str().c_str(), L"Vector2D Distance", MB_OK );
            break;
        }
        case VK_NUMPAD2:
        {
            // Vector2D Addition
            Vector2D vec1 = { 1.0f, 2.0f };
            Vector2D vec2 = { 7.0f, 4.0f };
            Vector2D vec3 = vec1 + vec2;
            wstringstream oss;
            oss << '(' << vec1.x << ',' << vec1.y << ") + (" << vec2.x << ',' << vec2.y << ") = (" << vec3.x << ',' << vec3.y << ')';
            MessageBox( nullptr, oss.str().c_str(), L"Vector2D Addition", MB_OK );
            break;
        }
        case VK_NUMPAD3:
        {
            // seek the red car
            m_pCar->setPositionTo( *m_pCar2->getPosition() );
            break;
        }
        case VK_NUMPAD4:
        {
            // flee the red car
            m_pCar->setPositionTo( Flee( *m_pCar2->getPosition() ) );
            break;
        }
        case VK_NUMPAD5:
        {
            // arrive at the red car
            m_pCar->setPositionTo( Arrive( *m_pCar2->getPosition(), slow ) );
            break;
        }
        default:
            break;
    }
}

void AIManager::checkWallWrapping( Vehicle* car )
{
    int offset = 10.0f;
    int xBound = width / 2;
    if ( car->getPosition()->x > xBound - offset )
        car->setPosition( XMFLOAT3( -xBound, car->getPosition()->y, car->getPosition()->z ) );
    else if ( car->getPosition()->x < -xBound + offset )
        car->setPosition( XMFLOAT3( xBound, car->getPosition()->y, car->getPosition()->z ) );

    int yBound = height / 2;
    if ( car->getPosition()->y < -yBound + offset )
        car->setPosition( XMFLOAT3( car->getPosition()->x, yBound, car->getPosition()->z ) );
    else if ( car->getPosition()->y > yBound - offset )
        car->setPosition( XMFLOAT3( car->getPosition()->x, -yBound, car->getPosition()->z ) );
}

bool AIManager::checkForCollisions( Vehicle* car )
{
    if (m_pickups.size() == 0)
        return false;

    XMVECTOR dummy;

    // the car
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose(
        &carScale,
        &dummy,
        &carPos,
        XMLoadFloat4x4( car->getTransform() )
    );

    XMFLOAT3 scale;
    XMStoreFloat3(&scale, carScale);
    BoundingSphere boundingSphereCar;
    XMStoreFloat3(&boundingSphereCar.Center, carPos);
    boundingSphereCar.Radius = scale.x;

    // a pickup - !! NOTE it is only referring the first one in the list !!
    XMVECTOR puPos;
    XMVECTOR puScale;
    XMMatrixDecompose(
        &puScale,
        &dummy,
        &puPos,
        XMLoadFloat4x4(m_pickups[0]->getTransform())
    );

    XMStoreFloat3(&scale, puScale);
    BoundingSphere boundingSpherePU;
    XMStoreFloat3(&boundingSpherePU.Center, puPos);
    boundingSpherePU.Radius = scale.x;

    if (boundingSphereCar.Intersects(boundingSpherePU))
    {
        OutputDebugStringA("Collision!\n");
        return true;
    }

    return false;
}

Vector2D AIManager::Flee( Vector2D TargetPos )
{
    const float max_distance = 400.0f;
    Vector2D distToTarget( *m_pCar->getPosition() - TargetPos );
    Vector2D DesiredVelocity = Vec2DNormalize( distToTarget ) * max_distance;
    return ( DesiredVelocity - *m_pCar->getDirection() );
}

Vector2D AIManager::Arrive( Vector2D TargetPos, Deceleration deceleration )
{
    Vector2D ToTarget = TargetPos - *m_pCar->getPosition();
    double dist = ToTarget.Length();

    if ( dist > 0 )
    {
        const double DecelerationTweaker = 0.3;
        double speed = dist / ( static_cast<double>( deceleration ) * DecelerationTweaker );
        speed = min( speed, m_pCar->getMaxSpeed() );

        Vector2D DesiredVelocity = ToTarget * speed / dist;
        return DesiredVelocity;
    }

    return Vector2D( 0, 0 );
}

void AIManager::Wander( Vehicle* car )
{
    int xBound = width / 2;
    if ( car->getPosition()->x >= xBound )
    {
        car->setPositionTo( XMFLOAT3( -xBound, car->getPosition()->y, car->getPosition()->z ) );
    }
    else if ( car->getPosition()->x <= -xBound )
    {
        car->setPositionTo( XMFLOAT3( xBound, car->getPosition()->y, car->getPosition()->z ) );
    }
}