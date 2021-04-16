#include <time.h>
#include "main.h"
#include "Vehicle.h"
#include "Waypoint.h"
#include "AIManager.h"
#include "Background.h"
#include "PickupItem.h"
#include "ImGuiManager.h"
#include "Logging/ErrorLogger.h"

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
HRESULT InitMesh();
HRESULT InitWorld( int width, int height );
void CleanupDevice();
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();

#pragma region GLOBAL_VARIABLES
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;

Microsoft::WRL::ComPtr<ID3D11Device> g_pd3dDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D11Device1> g_pd3dDevice1 = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pImmediateContext = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext1> g_pImmediateContext1 = nullptr;

IDXGISwapChain* g_pSwapChain = nullptr;
IDXGISwapChain1* g_pSwapChain1 = nullptr;

ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11Texture2D* g_pDepthStencil = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;

ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11SamplerState* g_pSamplerNormal = nullptr;
ID3D11InputLayout* g_pInputLayout = nullptr;

ID3D11Buffer* g_pConstantBuffer = nullptr;
ID3D11Buffer* g_pMaterialConstantBuffer = nullptr;
ID3D11Buffer* g_pLightConstantBuffer = nullptr;

XMMATRIX g_View;
XMMATRIX g_Projection;

int g_viewWidth;
int g_viewHeight;
vecDrawables g_GameObjects;
Background g_background;
AIManager g_AIManager;
ImGuiManager imgui;
#pragma endregion

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = { 0 };
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
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
    if( !RegisterClassEx( &wcex ) ) return E_FAIL;

    // Create window
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;
    g_hInst = hInstance;
    RECT rc = { 0, 0, width, height };

	g_viewWidth = width;
	g_viewHeight = height;

    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow(
        L"TutorialWindowClass", L"Direct3D 11 Tutorial 5",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr
    );
    if( !g_hWnd ) return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    return S_OK;
}

HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(
            nullptr, g_driverType, nullptr,
            createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel,
            &g_pImmediateContext
        );

        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(
                nullptr, g_driverType, nullptr,
                createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel,
                &g_pImmediateContext
            );
        }

        if( SUCCEEDED( hr ) ) break;
    }
    if( FAILED( hr ) ) return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void**>( &dxgiDevice ) );
        if ( SUCCEEDED( hr ) )
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if ( SUCCEEDED( hr ) )
            {
                hr = adapter->GetParent( __uuidof( IDXGIFactory1 ), reinterpret_cast<void**>( &dxgiFactory ) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if ( FAILED( hr ) ) return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof( IDXGIFactory2 ), reinterpret_cast<void**>( &dxgiFactory2 ) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof( ID3D11Device1 ), reinterpret_cast<void**>( g_pd3dDevice1.Get() ) );
        if ( SUCCEEDED( hr ) )
        {
            static_cast<void>( g_pImmediateContext->QueryInterface( __uuidof( ID3D11DeviceContext1 ),
                reinterpret_cast<void**>( g_pImmediateContext1.Get() ) ) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = { 0 };
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd( g_pd3dDevice.Get(), g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1 );
        if ( SUCCEEDED( hr ) )
            hr = g_pSwapChain1->QueryInterface( __uuidof( IDXGISwapChain ), reinterpret_cast<void**>( &g_pSwapChain ) );

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice.Get(), &sd, &g_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation( g_hWnd, DXGI_MWA_NO_ALT_ENTER );
    dxgiFactory->Release();
    if ( FAILED( hr ) ) return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) ) return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) ) return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = { 0 };
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, nullptr, &g_pDepthStencil );
    if( FAILED( hr ) ) return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) ) return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp = { 0 };
    vp.Width = static_cast<FLOAT>( width );
    vp.Height = static_cast<FLOAT>( height );
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

	hr = InitMesh();
	if ( FAILED( hr ) )
	{
		MessageBox( nullptr,
			L"Failed to initialise mesh.", L"Error", MB_OK );
		return hr;
	}

	hr = InitWorld( width, height );
	if ( FAILED( hr ) )
	{
		MessageBox( nullptr,
			L"Failed to initialise world.", L"Error", MB_OK );
		return hr;
	}

    // create the background (racetrack) 
    hr = g_background.InitMesh( g_pd3dDevice );
    if ( FAILED( hr ) )
    {
        MessageBox( nullptr,
            L"Failed to initialise background.", L"Error", MB_OK );
        return hr;
    }

    // initialise the AI / SceneManager
    g_AIManager.Initialise( g_hWnd, g_pd3dDevice, SCREEN_WIDTH, SCREEN_HEIGHT );

    // initialise imgui
    imgui.Initialize( g_hWnd, g_pd3dDevice.Get(), g_pImmediateContext.Get() );

    return S_OK;
}

HRESULT InitMesh()
{
	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	HRESULT hr = CompileShaderFromFile( L"Resources\\shader.fx", "VS", "vs_4_0", &pVSBlob );
	if ( FAILED( hr ) )
	{
		MessageBox( nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
            L"Error", MB_OK );
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader );
	if ( FAILED( hr ) )
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(
        layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pInputLayout
    );
	pVSBlob->Release();
	if ( FAILED( hr ) ) return hr;

	// Set the input layout ****
	g_pImmediateContext->IASetInputLayout( g_pInputLayout );

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile( L"Resources\\shader.fx", "PS", "ps_4_0", &pPSBlob );
	if ( FAILED( hr ) )
	{
		MessageBox( nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
            L"Error", MB_OK );
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader );
	pPSBlob->Release();
	if ( FAILED( hr ) ) return hr;

	// Create the constant buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( ConstantBuffer );
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer( &bd, nullptr, &g_pConstantBuffer );
	if ( FAILED( hr ) ) return hr;

	return hr;
}

HRESULT InitWorld( int width, int height )
{
    DirectX::XMFLOAT4 g_EyePosition( 0, 0, -200, 1 );

	// Initialize the view matrix
	XMVECTOR Eye = XMLoadFloat4( &g_EyePosition );
	XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	g_View = XMMatrixLookAtLH( Eye, At, Up );

	// Initialize the projection matrix
    g_Projection = XMMatrixOrthographicLH( static_cast<float>( width ), static_cast<float>( height ), 0.01f, 1000.0f );

	return S_OK;
}

void CleanupDevice()
{
    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain1 ) g_pSwapChain1->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, message, wParam, lParam ) )
        return true;

    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    {
        int xPos = GET_X_LPARAM( lParam );
        int yPos = GET_Y_LPARAM( lParam );
        
        // modify the x and y pos to match the scene coordinates
        xPos -= SCREEN_WIDTH / 2;
        yPos -= SCREEN_HEIGHT / 2;
        yPos *= -1;
	    if ( message == WM_LBUTTONUP )
	    {
            g_AIManager.SetCrosshair( xPos, yPos );
		    break;
	    }
        if ( message == WM_RBUTTONUP )
        {
            //g_AIManager.RightMouseUp( xPos, yPos );
            break;
        }
        break;
    }
    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_KEYDOWN:
        //g_AIManager.keyPress( wParam );
        break;

    case WM_KEYUP:
        g_AIManager.HandleKeyPresses( wParam );

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

void SetupTransformConstantBuffer( XMMATRIX world, XMMATRIX view, XMMATRIX projection )
{
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose( world );
	cb1.mView = XMMatrixTranspose( view );
	cb1.mProjection = XMMatrixTranspose( projection );
	g_pImmediateContext->UpdateSubresource( g_pConstantBuffer, 0, nullptr, &cb1, 0, 0 );

}

void DrawItem( DrawableGameObject* object )
{
    SetupTransformConstantBuffer( XMLoadFloat4x4( object->GetTransform() ), g_View, g_Projection );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
    g_pImmediateContext->PSSetShaderResources( 0, 1, object->GetTextureResourceView() );
    g_pImmediateContext->PSSetSamplers( 0, 1, object->GetTextureSamplerState() );

    // draw 
    object->Draw( g_pImmediateContext );
}

void AddItemToDrawList( DrawableGameObject* object )
{
    if ( object == nullptr )
        ErrorLogger::Log( std::string( "Object (" ) + typeid( object ).name() + ") was nullptr when trying to draw!" );
    g_GameObjects.push_back( object );
}

void Update( const float deltaTime )
{
    g_AIManager.Update( deltaTime );
}

void Render()
{
    // Update our time
    static float deltaTime = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if ( timeStart == 0 ) timeStart = timeCur;
    deltaTime = ( timeCur - timeStart ) / 1000.0f;
	timeStart = timeCur;

    imgui.BeginRender();
    Update( deltaTime );
    
    // Clear the back buffer
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::AntiqueWhite );

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
    g_pImmediateContext->VSSetShader( g_pVertexShader, nullptr, 0 );
    g_pImmediateContext->PSSetShader( g_pPixelShader, nullptr, 0 );

    // draw the background
    SetupTransformConstantBuffer( XMLoadFloat4x4( g_background.GetTransform() ), XMMatrixIdentity(), XMMatrixIdentity() );
    g_pImmediateContext->PSSetShaderResources( 0, 1, g_background.GetTextureResourceView() );
    g_pImmediateContext->PSSetSamplers( 0, 1, g_background.GetTextureSamplerState() );
    g_background.Draw( g_pd3dDevice, g_pImmediateContext );
    
    // draw the vehicles / nodes
	for( uint32_t i = 0; i < g_GameObjects.size(); i++ ) 
        DrawItem( g_GameObjects[i] );

    g_GameObjects.clear(); // the list of items to draw is cleared each frame

    // Present our back buffer to our front buffer
    imgui.EndRender();
    g_pSwapChain->Present( 0, 0 );
}