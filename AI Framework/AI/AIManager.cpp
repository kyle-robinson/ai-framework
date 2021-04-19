#include "AIManager.h"
#include "SteeringBehaviour.h"
#include "PickupItem.h"
#include "Waypoint.h"
#include "WinMain.h"
#include <imgui/imgui.h>

// globals
const float xGap = SCREEN_WIDTH / WAYPOINT_RESOLUTION;
const float yGap = SCREEN_HEIGHT / WAYPOINT_RESOLUTION;
const float xStart = -( SCREEN_WIDTH / 2 ) + ( xGap / 2 );
const float yStart = -( SCREEN_HEIGHT / 2 ) + ( yGap / 2 );
std::vector<std::vector<int>> g_vWaypoints{
    { 11, 15 }, { 2, 18 }, { 0, 12 }, { 2, 8 }, { 4, 2 }, { 7, 2 }, { 7, 4 },
    { 4, 13 }, { 9, 10 }, { 16, 8 }, { 13, 4 }, { 14, 2 }, { 18, 2 }, { 19, 7 }, { 19, 11 },
};

HRESULT AIManager::Initialise( HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> device, UINT width, UINT height )
{
    this->hWnd = hWnd;
    this->width = width;
    this->height = height;
    this->m_pDevice = device;

    // create waypoints
    uint32_t index = 0;
    for ( uint32_t j = 0; j < WAYPOINT_RESOLUTION; j++ )
    {
        for ( uint32_t i = 0; i < WAYPOINT_RESOLUTION; i++ )
        {
            Waypoint* wp = new Waypoint();
            HRESULT hr = wp->InitMesh( device, index++ );
            wp->SetPosition( { xStart + ( xGap * i ), yStart + ( yGap * j ) } );
            m_waypoints.push_back( std::move( wp ) );
        }
    }

    // create pickups
    index = 0;
    for ( uint32_t i = 0; i < WAYPOINT_RESOLUTION; i++ )
    {
        for ( uint32_t j = 0; j < WAYPOINT_RESOLUTION; j++ )
        {
            PickupItem* pickup = new PickupItem( xStart + ( xGap * j ), yStart + ( yGap * i ), 10 );
            if ( FAILED( pickup->InitMesh( device, index++ ) ) )
            {
                delete pickup;
                continue;
            }
            m_pickups.push_back( std::move( pickup ) );
        }
    }

    // create path
    m_pPath = new Path();
    for ( uint32_t i = 0; i < g_vWaypoints.size(); i++ )
        m_pPath->AddWayPoint( { xStart + ( xGap * g_vWaypoints[i][0] ), yStart + ( yGap * g_vWaypoints[i][1] ) } );

    // create vehicles
    m_pCarBlue = new Vehicle( this, { 120, 180 }, RandFloat() * TwoPi, { 0, 0 }, 1, 25, 50, 20 );
    HRESULT hr = m_pCarBlue->InitMesh( device.Get(), L"Resources\\Textures\\car_blue.dds" );
    m_pCarBlue->Steering()->SetPath( m_pPath->GetPath() );

    m_pCarRed = new Vehicle( this, { 160, 140 }, RandFloat() * TwoPi, { 0, 0 }, 1, 25, 50, 20 );
    hr = m_pCarRed->InitMesh( device.Get(), L"Resources\\Textures\\car_red.dds" );
    m_pCarRed->Steering()->PursuitOn( m_pCarBlue );

    return hr;
}

void AIManager::CreateObstacles()
{
    int noOfObstacles = m_iObstacleCount;
    for ( uint32_t i = 0; i < noOfObstacles; ++i )
    {
        bool overlapped = true;
        bool bOverlapped = true;
        int allowableTrys = 2000;
        int numTrys = 0;

        while ( bOverlapped )
        {
            numTrys++;
            if ( numTrys > allowableTrys ) return;

            RECT rect;
            GetClientRect( GetHWND(), &rect );
            int cxClient = rect.right;
            int cyClient = rect.bottom;

            // randomly place obstacles
            int radius = 50;
            const int border = 10;
            Obstacle* ob = new Obstacle(
                RandInt( ( -cxClient / 2 ) + radius + border, ( cxClient / 2 ) - radius - border ),
                RandInt( ( -cyClient / 2 ) + radius + border, ( cyClient / 2 ) - radius - 30 - border ),
                radius
            );

            HRESULT hr = ob->InitMesh( m_pDevice.Get() );
            if ( FAILED( hr ) ) return;

            const int minimumGap = 20;
            if ( !Overlapped( ob, m_obstacles, minimumGap ) )
            {
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

void AIManager::Update( const float dt )
{
    // UPDATE
    {
        // cars
        if ( !m_bPaused )
        {
            m_pCarBlue->Update( dt );
            if ( m_bEnableRedCar )
                m_pCarRed->Update( dt );
        }

        // waypoints
        for ( uint32_t i = 0; i < m_waypoints.size(); i++ )
            m_waypoints[i]->Update( dt );

        // obstacles
        for ( uint32_t i = 0; i < m_obstacles.size(); i++ )
            m_obstacles[i]->Update( dt );

        // pickups
        for ( uint32_t i = 0; i < m_pickups.size(); i++ )
            m_pickups[i]->Update( dt );

        // collisions
        static int pickupTimer;
        if ( CheckForCollisions( m_pCarBlue ) )
        {
            OutputDebugStringA( "Item Acquired: Speed Increase!\n" );
            m_bItemPickedUp = true;
        }

        // update car parameters on item pickup
        if ( m_bItemPickedUp && pickupTimer > 0 )
        {
            pickupTimer--;
            m_pCarBlue->SetMaxForce( 50 );
            m_pCarBlue->SetMaxSpeed( 100 );
        }
        else
        {
            pickupTimer = 150;
            m_bItemPickedUp = false;
            m_pCarBlue->SetMaxForce( 25 );
            m_pCarBlue->SetMaxSpeed( 50 );
        }
    }

    // RENDER
    {
        // cars
        AddItemToDrawList( m_pCarBlue );
        if ( m_bEnableRedCar )
            AddItemToDrawList( m_pCarRed );

        // pickups
        for ( uint32_t i = 0; i < m_pickups.size(); i++ )
            AddItemToDrawList( m_pickups[i] );

        // obstacles
        for ( uint32_t i = 0; i < m_obstacles.size(); i++ )
            AddItemToDrawList( m_obstacles[i] );

        // waypoints
        if ( m_waypoints[0]->IsVisible() )
            for ( uint32_t i = 0; i < m_waypoints.size(); i++ )
                AddItemToDrawList( m_waypoints[i] );

        // imgui
        SpawnBehaviourWindow();
        SpawnObstacleWindow();
        SpawnControlWindow();
    }
}

bool AIManager::CheckForCollisions( Vehicle* car )
{
    if ( m_pickups.size() == 0 ) return false;

    // the car
    XMVECTOR dummy;
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose( &carScale, &dummy, &carPos,
        XMLoadFloat4x4( car->GetTransform() )
    );

    XMFLOAT3 scale;
    XMStoreFloat3( &scale, carScale );
    BoundingSphere boundingSphereCar;
    XMStoreFloat3( &boundingSphereCar.Center, carPos );
    boundingSphereCar.Radius = scale.x;

    for ( uint32_t i = 0; i < m_pickups.size(); i++ )
    {
        // a pickup
        XMVECTOR puPos;
        XMVECTOR puScale;
        XMMatrixDecompose( &puScale, &dummy, &puPos,
            XMLoadFloat4x4( m_pickups[i]->GetTransform() )
        );

        XMStoreFloat3( &scale, puScale );
        BoundingSphere boundingSpherePU;
        XMStoreFloat3( &boundingSpherePU.Center, puPos );
        boundingSpherePU.Radius = scale.x;

        // collision occurred
        if ( boundingSphereCar.Intersects( boundingSpherePU ) )
            return true;
    }

    return false;
}

void AIManager::SpawnBehaviourWindow()
{
    if ( ImGui::Begin( "Behaviours", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        // wander
        static bool wanderBool = m_pCarBlue->Steering()->IsWanderOn();
        ImGui::Checkbox( "Wander", &wanderBool );
        wanderBool ? m_pCarBlue->Steering()->WanderOn() : m_pCarBlue->Steering()->WanderOff();

        // arrive
        static bool arriveBool = m_pCarBlue->Steering()->IsArriveOn();
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
        static bool seekBool = m_pCarBlue->Steering()->IsSeekOn();
        ImGui::Checkbox( "Seek", &seekBool );
        seekBool ? m_pCarBlue->Steering()->SeekOn() : m_pCarBlue->Steering()->SeekOff();

        // flee
        static bool fleeBool = m_pCarBlue->Steering()->IsFleeOn();
        ImGui::Checkbox( "Flee", &fleeBool );
        fleeBool ? m_pCarBlue->Steering()->FleeOn() : m_pCarBlue->Steering()->FleeOff();

        // pursuit
        static bool pursuitBool = m_bEnableRedCar;
        ImGui::Checkbox( "Pursuit", &pursuitBool );
        m_bEnableRedCar = pursuitBool;

        // path following
        static bool pathFollowingBool = m_pCarBlue->Steering()->IsFollowPathOn();
        ImGui::Checkbox( "Path Following", &pathFollowingBool );
        pathFollowingBool ? m_pCarBlue->Steering()->FollowPathOn() : m_pCarBlue->Steering()->FollowPathOff();
    }
    ImGui::End();
}

void AIManager::SpawnObstacleWindow()
{
    if ( ImGui::Begin( "Obstacles", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        if ( !m_bShowObstacles )
            ImGui::SliderInt( "Amount", &m_iObstacleCount, 3, 10 );

        if ( ImGui::Button( m_bShowObstacles ? "Clear Obstacles" : "Create Obstacles" ) )
        {
            if ( !m_bShowObstacles )
            {
                CreateObstacles();
                m_bShowObstacles = true;
                m_pCarBlue->Steering()->ObstacleAvoidanceOn();
            }
            else
            {
                m_obstacles.clear();
                m_bShowObstacles = false;
                m_pCarBlue->Steering()->ObstacleAvoidanceOff();
            }
        }
    }
    ImGui::End();
}

void AIManager::SpawnControlWindow()
{
    if ( ImGui::Begin( "Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::Checkbox( "Paused", &m_bPaused );

        static bool showWaypoints = m_waypoints[0]->IsVisible();
        ImGui::Checkbox( "Show Waypoints", &showWaypoints );
        m_waypoints[0]->SetVisible( showWaypoints );
        
        // car parameters
        if ( ImGui::CollapsingHeader( "Car Controls" ) )
        {
            if ( m_pCarBlue->Steering()->IsFollowPathOn() )
            {
                if ( ImGui::Button( "Follow Track" ) )
                {
                    delete m_pPath;
                    m_pPath = new Path();
                    for ( uint32_t i = 0; i < g_vWaypoints.size(); i++ )
                        m_pPath->AddWayPoint( { xStart + ( xGap * g_vWaypoints[i][0] ), yStart + ( yGap * g_vWaypoints[i][1] ) } );
                    m_pCarBlue->Steering()->SetPath( m_pPath->GetPath() );
                }
                if ( ImGui::Button( "Return to Start" ) )
                {
                    delete m_pPath;
                    m_pPath = new Path();
                    m_pPath->AddWayPoint( { xStart + ( xGap * g_vWaypoints[0][0] ), yStart + ( yGap * g_vWaypoints[0][1] ) } );
                    m_pCarBlue->World()->SetCrosshair( xStart + ( xGap * g_vWaypoints[0][0] ), yStart + ( yGap * g_vWaypoints[0][1] ) );
                    m_pCarBlue->Steering()->SetPath( m_pPath->GetPath() );
                }
            }

            static float carSpeed = m_pCarBlue->GetMaxSpeed();
            ImGui::SliderFloat( "Max Speed", &carSpeed, 25.0f, 100.0f );
            if ( !m_bItemPickedUp )
                m_pCarBlue->SetMaxSpeed( carSpeed );

            static float carForce = m_pCarBlue->GetMaxForce();
            ImGui::SliderFloat( "Max Force", &carForce, 10.0f, 50.0f );
            if ( !m_bItemPickedUp )
                m_pCarBlue->SetMaxForce( carForce );
        }
    }
    ImGui::End();
}