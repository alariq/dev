//-----------------------------------------------------------------------------
// File: main.cpp
// Based on Microsoft SDK "Matrices" example
//-----------------------------------------------------------------------------
#include <windows.h>
#include <d3dx9.h>

#include "dconsole/graphic_console.h"
#include "console/dx_text_driver2.h"
#include "console/MyConsole.h"



 

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices

D3DXTextDriver2* g_pTextDriver = 0;		// driver to draw console
int g_Width = 800, g_Height = 600;


LPDIRECT3DDEVICE9 GetDevice() { return g_pd3dDevice; }
void UpdateConsoleInput(UINT vk);

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z;      // The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)




//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling, so we see the front and back of the triangle
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting, since we are providing our own vertex colors
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Initialize three vertices for rendering a triangle
    CUSTOMVERTEX g_Vertices[] =
    {
        { -1.0f,-1.0f, 0.0f, 0xffff0000, },
        {  1.0f,-1.0f, 0.0f, 0xff0000ff, },
        {  0.0f, 1.0f, 0.0f, 0xffffffff, },
    };

    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 3 * sizeof( CUSTOMVERTEX ),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof( g_Vertices ), ( void** )&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof( g_Vertices ) );
    g_pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform Matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIXA16 matWorld;

    // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    UINT iTime = timeGetTime() % 1000;
    FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 1000.0f;
    D3DXMatrixRotationY( &matWorld, fAngle );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer to a black color
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Setup the world, view, and projection Matrices
        SetupMatrices();

        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
        g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );

		// End the scene
        g_pd3dDevice->EndScene();
    }

	dconsole::Instance().Draw();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
		case WM_KEYDOWN:
			{
				//unsigned char scan_code = (unsigned char)((lParam >> 16) & 0xFF);
				//WORD ch[2];
				//int rv = ToAscii((UINT)wParam, 0, 0, ch, 0);
				UpdateConsoleInput((UINT)wParam);
				break;

			}
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

void UpdateConsoleInput(UINT vk)
{
	// update console
	//IDevice* pdev = IInput::Instance()->GetDevice(DT_Keyboard);
	short ms =  GetKeyState(VK_CONTROL);
	bool mod_pressed = (GetKeyState(VK_CONTROL) & 0x80)!=0;

	if(VK_TAB == vk)
	{
		bool is_opened = dconsole::Instance().isOpened();
		dconsole::Instance().setOpen(!is_opened);
	}
	else
	{
		// execute command
		bool catched = false;
		switch(vk & 0xFF)
		{
		case VK_RETURN:		dconsole::Console::onKbdExecute(); catched = true; break;
			// match list
		case VK_UP:			dconsole::Console::onKbdPrev(mod_pressed);catched = true;   break;
		case VK_DOWN:		dconsole::Console::onKbdNext(mod_pressed);catched = true;  break;
		case VK_PRIOR:		dconsole::Console::onKbdPrevPage(mod_pressed);catched = true;  break;
		case VK_NEXT:		dconsole::Console::onKbdNextPage(mod_pressed);catched = true;  break;
			// cmd line
		case VK_SPACE:		if(mod_pressed) { dconsole::Console::onKbdAutocomplete(); catched = true; } break;
		case VK_BACK:		dconsole::Console::onKbdBackspace(); catched = true; break;
		case VK_HOME:		dconsole::Console::onKbdMoveHome(); catched = true; break;
		case VK_END:		dconsole::Console::onKbdMoveEnd();catched = true;  break;
		case VK_LEFT:		dconsole::Console::onKbdMoveLeft(); catched = true; break;
		case VK_RIGHT:		dconsole::Console::onKbdMoveRight();catched = true;  break;
		case VK_DELETE:		dconsole::Console::onKbdDelete(); catched = true; break;
		}

		if(!mod_pressed && !catched)
		{
			//if(isascii(ch))
			{
				KeyEvent ke;
				ke.c = (char)MapVirtualKey((UINT)vk, MAPVK_VK_TO_CHAR);
				if(ke.c == 0)
					return;
				ke.mod = GetKeyState(VK_SHIFT) & 0x80 ? VK_SHIFT : 0;
				dconsole::Console::onKbdEvent(&ke);
			}
		}

		
	}
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        "console", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "console", "dconsole usage",
                              WS_OVERLAPPEDWINDOW, 100, 100, g_Width, g_Height,
                              NULL, NULL, wc.hInstance, NULL );

    RECT rc = { 0, 0, g_Width, g_Height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    MoveWindow( hWnd, 100, 100, rc.right - rc.left, rc.bottom - rc.top, TRUE);

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the scene geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
			g_pTextDriver = new D3DXTextDriver2();
			g_pTextDriver->init("FixedWidth.bmp", 8, 16, 16, g_Width, g_Height);
			GraphicsConsole::Instance().setTextDriver( g_pTextDriver );
			dconsole::test();
			//dconsole::Instance().setOpen(true);


            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }

			GraphicsConsole::Instance().setTextDriver( 0 );
			delete g_pTextDriver;
        }
    }

    UnregisterClass( "console", wc.hInstance );
    return 0;
}



