#include "AIManager.h"
#include "PathFinder.h"
#include "ErrorLogger.h"
#include "PickupItem.h"
#include "SteeringBehaviour.h"
#include <sstream>
#include "main.h"

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

HRESULT AIManager::Initialise( HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> pd3dDevice, UINT width, UINT height )
{
    this->hWnd = hWnd;
    this->width = width;
    this->height = height;
    
    // create a pickup item ----------------------------------------------
    //PickupItem* pPickup = new PickupItem();
    //HRESULT hr = pPickup->initMesh( pd3dDevice );
    //m_pickups.push_back( pPickup );

    // create the vehicle ------------------------------------------------
    double xPos = 0.0;
    double yPos = 0.0;

    m_pCar = new Vehicle( this, { xPos, yPos }, RandFloat() * TwoPi, { 0.0, 0.0 }, 1.0, 50.0, 150.0, 200.0 );
    HRESULT hr = m_pCar->InitMesh( pd3dDevice.Get(), L"Resources\\Textures\\car_blue.dds" );
    m_pCar->Steering()->ArriveOn();

    /*m_pCar2 = new Vehicle();
    hr = m_pCar2->initMesh( pd3dDevice.Get(), L"Resources\\Textures\\car_red.dds" );
    m_pCar2->setPositionTo( XMFLOAT3( width / 2, 0.0f, 0 ) );
    if ( FAILED( hr ) ) return hr;

    // create the waypoints
    float xGap = SCREEN_WIDTH / WAYPOINT_RESOLUTION;
    float yGap = SCREEN_HEIGHT / WAYPOINT_RESOLUTION;
    float xStart = -( SCREEN_WIDTH / 2 ) + ( xGap / 2 );
    float yStart = -( SCREEN_HEIGHT / 2 ) + ( yGap / 2 );

    uint32_t index = 0;
    for ( uint32_t j = 0; j < WAYPOINT_RESOLUTION; j++ )
    {
        for ( uint32_t i = 0; i < WAYPOINT_RESOLUTION; i++ )
        {
            Waypoint* wp = new Waypoint();
            hr = wp->initMesh( pd3dDevice, index++ );
            wp->setPosition( { xStart + ( xGap * i ), yStart + ( yGap * j ), 0 } );
            m_waypoints.push_back( wp );
        }
    }*/

    return S_OK;
}

void AIManager::Update( const float fDeltaTime )
{
    // waypoints
    /*for ( uint32_t i = 0; i < m_waypoints.size(); i++ )
    {
        m_waypoints[i]->update( fDeltaTime );
        AddItemToDrawList( m_waypoints[i] ); // if you comment this in, it will display the waypoints
    }

    AddItemToDrawList( GetWaypoint( 9, 1 ) );
    AddItemToDrawList( GetWaypoint( 5, 1 ) );

    //vecWaypoints neighbours = GetNeighbours( 19, 10 );
    for ( uint32_t i = 0; i < neighbours.size(); i++ )
        AddItemToDrawList( neighbours[i] );

    // pickups
    for ( uint32_t i = 0; i < m_pickups.size(); i++ )
    {
        m_pickups[i]->update( fDeltaTime );
        AddItemToDrawList( m_pickups[i] );
    }*/

    // cars
    //if ( !m_paused )
    m_pCar->Update( fDeltaTime );
    AddItemToDrawList( m_pCar );

    //m_pCar2->update( fDeltaTime );
    //AddItemToDrawList( m_pCar2 );
}

/*void AIManager::LeftMouseUp( const int x, const int y )
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
}*/

void AIManager::HandleKeyPresses( WPARAM param )
{
    switch ( param )
    {
        /*case VK_NUMPAD0:
        {
            OutputDebugStringA( "0 pressed \n" );
            break;
        }
        case VK_NUMPAD1:
        {
            // Vector2D Distance Calculation
            Vector2D vec1 = { 1.0f, 2.0f };
            Vector2D vec2 = { 7.0f, 4.0f };
            double distance = vec1.Distance( vec2 );
            std::wstringstream oss;
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
            std::wstringstream oss;
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
        }*/
    case 'P':
        TogglePause();
        break;
    }
}

bool AIManager::checkForCollisions( Vehicle* car )
{
    if ( m_pickups.size() == 0 )
        return false;

    XMVECTOR dummy;

    // the car
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose( &carScale, &dummy, &carPos,
        XMLoadFloat4x4( car->getTransform() )
    );

    XMFLOAT3 scale;
    XMStoreFloat3( &scale, carScale );
    BoundingSphere boundingSphereCar;
    XMStoreFloat3( &boundingSphereCar.Center, carPos );
    boundingSphereCar.Radius = scale.x;

    // a pickup - !! NOTE it is only referring the first one in the list !!
    XMVECTOR puPos;
    XMVECTOR puScale;
    XMMatrixDecompose( &puScale, &dummy, &puPos,
        XMLoadFloat4x4( m_pickups[0]->getTransform() )
    );

    XMStoreFloat3( &scale, puScale );
    BoundingSphere boundingSpherePU;
    XMStoreFloat3( &boundingSpherePU.Center, puPos );
    boundingSpherePU.Radius = scale.x;

    if ( boundingSphereCar.Intersects( boundingSpherePU ) )
    {
        OutputDebugStringA( "Collision!\n" );
        return true;
    }

    return false;
}