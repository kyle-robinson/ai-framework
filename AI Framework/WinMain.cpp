#include "WinMain.h"
#include "Graphics.h"
#include "AIManager.h"
#include "Background.h"
#include "Logging/Timer.h"
#include "DrawableGameObject.h"
#include "Logging/ErrorLogger.h"

#pragma region GLOBAL_VARIABLES
void RenderFrame();
void UpdateProjection( int width, int height );
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

HWND g_hWnd;
HINSTANCE g_hInst;

XMMATRIX m_mView;
XMMATRIX m_mProjection;
vecDrawables m_gameObjects;

Timer g_timer;
Graphics g_graphics;
AIManager g_aiManager;
Background g_background;

Microsoft::WRL::ComPtr<ID3D11Buffer> g_pConstantBuffer;
#pragma endregion

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    // create application
    if ( FAILED( InitWindow( g_hInst, nCmdShow ) ) ) return 0;
    if ( !g_graphics.Initialize( g_hWnd, SCREEN_WIDTH, SCREEN_HEIGHT ) ) return 0;
    UpdateProjection( SCREEN_WIDTH, SCREEN_HEIGHT );

    // create constant buffer
    D3D11_BUFFER_DESC bd = { 0 };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( ConstantBuffer );
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HRESULT hr = g_graphics.GetDevice()->CreateBuffer( &bd, nullptr, &g_pConstantBuffer );
    if ( FAILED( hr ) ) return 0;

    // create objects
    hr = g_aiManager.Initialise( g_hWnd, g_graphics.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT );
    hr = g_background.InitMesh( g_graphics.GetDevice() );

    // message loop
    MSG msg = { 0 };
    while ( WM_QUIT != msg.message )
    {
        if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            RenderFrame();
        }
    }

    return 0;
}

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if ( !RegisterClassEx( &wcex ) ) return E_FAIL;

    // create window
    g_hInst = hInstance;
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    int centerScreenX = GetSystemMetrics( SM_CXSCREEN ) / 2 - width / 2;
    int centerScreenY = GetSystemMetrics( SM_CYSCREEN ) / 2 - height / 2;

    RECT windowRect;
    windowRect.left = centerScreenX;
    windowRect.top = centerScreenY;
    windowRect.right = windowRect.left + width;
    windowRect.bottom = windowRect.top + height;
    AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );

    AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow(
        L"TutorialWindowClass",
        L"DirectX 11 Artificial Intelligence Framework",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    if ( !g_hWnd ) return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    return S_OK;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, message, wParam, lParam ) )
        return true;

    switch ( message )
    {
    case WM_PAINT:
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;
    case WM_LBUTTONUP:
    {
        int xPos = GET_X_LPARAM( lParam );
        int yPos = GET_Y_LPARAM( lParam );
        xPos -= SCREEN_WIDTH / 2;
        yPos -= SCREEN_HEIGHT / 2;
        yPos *= -1;
        g_aiManager.SetCrosshair( xPos, yPos );
        break;
    }
    case WM_KEYUP:
        g_aiManager.HandleKeyPresses( wParam );
        break;
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
}

void SetupTransformConstantBuffer( XMMATRIX world, XMMATRIX view, XMMATRIX projection )
{
    ConstantBuffer cb1;
    cb1.mWorld = XMMatrixTranspose( world );
    cb1.mView = XMMatrixTranspose( view );
    cb1.mProjection = XMMatrixTranspose( projection );
    g_graphics.GetContext()->UpdateSubresource( g_pConstantBuffer.Get(), 0, nullptr, &cb1, 0, 0 );
}

void DrawItem( DrawableGameObject* object )
{
    SetupTransformConstantBuffer( XMLoadFloat4x4( object->GetTransform() ), m_mView, m_mProjection );
    g_graphics.GetContext()->VSSetConstantBuffers( 0, 1, g_pConstantBuffer.GetAddressOf() );
    g_graphics.GetContext()->PSSetShaderResources( 0, 1, object->GetTextureResourceView() );
    g_graphics.GetContext()->PSSetSamplers( 0, 1, object->GetTextureSamplerState() );
    object->Draw( g_graphics.GetContext() );
}

void UpdateProjection( int width, int height )
{
    DirectX::XMFLOAT4 g_EyePosition( 0, 0, -200, 1 );
    XMVECTOR Eye = XMLoadFloat4( &g_EyePosition );
    XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

    m_mView = XMMatrixLookAtLH( Eye, At, Up );
    m_mProjection = XMMatrixOrthographicLH( static_cast< float >( width ),
        static_cast< float >( height ), 0.01f, 1000.0f );
}

void AddItemToDrawList( DrawableGameObject* object )
{
    if ( object == nullptr )
        ErrorLogger::Log( std::string( "Object (" ) + typeid( object ).name() + ") was nullptr when trying to draw!" );
    m_gameObjects.push_back( object );
}

void RenderFrame()
{
    float dt = g_timer.GetMilliSecondsElapsed();
    g_timer.Restart();
    dt *= 0.0015f;

    g_graphics.BeginFrame();

    // draw the background
    SetupTransformConstantBuffer( XMLoadFloat4x4( g_background.GetTransform() ), XMMatrixIdentity(), XMMatrixIdentity() );
    g_graphics.GetContext()->PSSetShaderResources( 0, 1, g_background.GetTextureResourceView() );
    g_graphics.GetContext()->PSSetSamplers( 0, 1, g_background.GetTextureSamplerState() );
    g_background.Draw( g_graphics.GetDevice(), g_graphics.GetContext() );

    // draw the vehicles/nodes
    g_aiManager.Update( dt );
    for ( uint32_t i = 0; i < m_gameObjects.size(); i++ )
        DrawItem( m_gameObjects[i] );
    m_gameObjects.clear();

    g_graphics.EndFrame();
}