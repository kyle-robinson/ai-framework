#include "AIManager.h"
#include "PathFinder.h"
#include "ErrorLogger.h"
#include "PickupItem.h"
#include "SteeringBehaviour.h"
#include <sstream>
#include "main.h"
#include <imgui/imgui.h>

inline bool TwoCirclesOverlapped( Vector2D c1, double r1,
    Vector2D c2, double r2 )
{
    double DistBetweenCenters = sqrt( ( c1.x - c2.x ) * ( c1.x - c2.x ) +
        ( c1.y - c2.y ) * ( c1.y - c2.y ) );

    if ( ( DistBetweenCenters < ( r1 + r2 ) ) || ( DistBetweenCenters < fabs( r1 - r2 ) ) )
    {
        return true;
    }

    return false;
}

template <class T, class conT>
bool Overlapped( const T* ob, const conT& conOb, double MinDistBetweenObstacles = 40.0 )
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
    this->device = pd3dDevice;
    
    // create a pickup item ----------------------------------------------
    //PickupItem* pPickup = new PickupItem();
    //HRESULT hr = pPickup->initMesh( pd3dDevice );
    //m_pickups.push_back( pPickup );

    // create vehicles
    m_pCarBlue = new Vehicle( this, { 0.0, 0.0 }, RandFloat() * TwoPi, { 0.0, 0.0 }, 1.0, 50.0, 150.0, 20.0 );
    HRESULT hr = m_pCarBlue->InitMesh( pd3dDevice.Get(), L"Resources\\Textures\\car_blue.dds" );
    m_pCarBlue->Steering()->WanderOn();

    m_pCarRed = new Vehicle( this, { 0.0, 0.0 }, RandFloat() * TwoPi, { 0.0, 0.0 }, 1.0, 50.0, 150.0, 20.0 );
    hr = m_pCarRed->InitMesh( pd3dDevice.Get(), L"Resources\\Textures\\car_red.dds" );
    m_pCarRed->Steering()->PursuitOn( m_pCarBlue );

    // create waypoints
    /*float xGap = SCREEN_WIDTH / WAYPOINT_RESOLUTION;
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

    return hr;
}

void AIManager::CreateObstacles()
{
    int noOfObstacles = 7;
    for ( int i = 0; i < noOfObstacles; ++i )
    {
        bool overlapped = true;
        bool bOverlapped = true;

        //keep creating tiddlywinks until we find one that doesn't overlap
        //any others.Sometimes this can get into an endless loop because the
        //obstacle has nowhere to fit. We test for this case and exit accordingly

        int NumTrys = 0; int NumAllowableTrys = 2000;

        while ( bOverlapped )
        {
            NumTrys++;

            if ( NumTrys > NumAllowableTrys ) return;

            int radius = RandInt( ( int )10, ( int )30 );

            const int border = 10;
            const int MinGapBetweenObstacles = 20;

            Obstacle* ob = new Obstacle( RandInt( radius + border, width - radius - border ),
                RandInt( radius + border, height - radius - 30 - border ),
                radius );

            HRESULT hr = ob->InitMesh( device.Get(), L"Resources\\Textures\\stone.dds" );
            if ( FAILED( hr ) ) return;

            if ( !Overlapped( ob, m_obstacles, MinGapBetweenObstacles ) )
            {
                //its not overlapped so we can add it
                m_obstacles.push_back( ob );

                bOverlapped = false;
            }
            else
            {
                delete ob;
            }
        }
    }
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
    if ( !IsPaused() )
    {
        m_pCarBlue->Update( fDeltaTime );
        if ( m_bEnableRedCar )
            m_pCarRed->Update( fDeltaTime );
        for ( uint32_t i = 0; i < m_obstacles.size(); i++ )
            m_obstacles[i]->update( fDeltaTime );
    }

    AddItemToDrawList( m_pCarBlue );
    if ( m_bEnableRedCar )
        AddItemToDrawList( m_pCarRed );

    for ( uint32_t i = 0; i < m_obstacles.size(); i++ )
        AddItemToDrawList( m_obstacles[i] );

    SpawnControlWindow();
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
    case 'P':
        TogglePause();
        break;

    case 'Y':
        m_bShowObstacles = !m_bShowObstacles;
        if ( !m_bShowObstacles )
        {
            m_obstacles.clear();
            m_pCarBlue->Steering()->ObstacleAvoidanceOff();
        }
        else
        {
            CreateObstacles();
            m_pCarBlue->Steering()->ObstacleAvoidanceOn();
        }
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

void AIManager::SpawnControlWindow()
{
    if ( ImGui::Begin( "Steering", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        // wander
        static bool wanderBool = true;
        ImGui::Checkbox( "Wander", &wanderBool );
        wanderBool ? m_pCarBlue->Steering()->WanderOn() : m_pCarBlue->Steering()->WanderOff();

        // arrive
        static bool arriveBool = false;
        ImGui::Checkbox( "Arrive", &arriveBool );
        arriveBool ? m_pCarBlue->Steering()->ArriveOn() : m_pCarBlue->Steering()->ArriveOff();
            
        if ( m_pCarBlue->Steering()->IsArriveOn() )
        {
            ImGui::Text( "Deceleration" );

            if ( ImGui::Button( "Fast" ) )
                m_pCarBlue->Steering()->SetDeceleration( static_cast< SteeringBehaviour::Deceleration >( 3 ) );
                
            ImGui::SameLine();
            if ( ImGui::Button( "Normal" ) )
                m_pCarBlue->Steering()->SetDeceleration( static_cast< SteeringBehaviour::Deceleration >( 2 ) );
                
            ImGui::SameLine();
            if ( ImGui::Button( "Slow" ) )
                m_pCarBlue->Steering()->SetDeceleration( static_cast< SteeringBehaviour::Deceleration >( 1 ) );
        }

        // seek
        static bool seekBool = false;
        ImGui::Checkbox( "Seek", &seekBool );
        seekBool ? m_pCarBlue->Steering()->SeekOn() : m_pCarBlue->Steering()->SeekOff();

        // flee
        static bool fleeBool = false;
        ImGui::Checkbox( "Flee", &fleeBool );
        fleeBool ? m_pCarBlue->Steering()->FleeOn() : m_pCarBlue->Steering()->FleeOff();

        // pursuit
        static bool pursuitBool = false;
        ImGui::Checkbox( "Pursuit", &pursuitBool );
        m_bEnableRedCar = pursuitBool;
    }
    ImGui::End();
}