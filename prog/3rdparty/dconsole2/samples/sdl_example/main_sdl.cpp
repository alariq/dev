/*
 * Console implementation example.
 * alariq@gmail.com
 *
 * SDL code based on Michael Vance's SDL Tutorial
 * Distributed under terms of the LGPL.
 */


#include "dconsole/console.h"
#include "dconsole/command.h"

#include <windows.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>

#include "dconsole/graphic_console.h"
#include "opengl_text_driver.h"


/* Dimensions of our window. */
static int g_width = 800;
static int g_height = 600;
/* Color depth in bits of our window. */
static int g_bpp;
/* Flags we will pass into SDL_SetVideoMode. */
int g_flags = 0;

OGLTextDriver* g_pTextDriver = 0;

bool g_exit = false;

static GLboolean should_rotate = GL_TRUE;

static int f_add(int a, int b)
{
    return a+b;
}

void register_commands()
{
	// add some test commands;
	dconsole::test();

    dconsole::BaseCmd* pcmd = dconsole::createCmd("add", "an addition function", f_add);
	dconsole::Console& c = dconsole::Instance();
	c.registerCommand(pcmd);

	//pcmd = dconsole::createVariableUP("exit", "set to TRUE in order to exit the program", &g_exit);
	//c.registerCommand(pcmd);
}


static void quit_tutorial( int code )
{
	delete g_pTextDriver;

    /*
     * Quit SDL so we can release the fullscreen
     * mode and restore the previous video settings,
     * etc.
     */
    SDL_Quit( );

    /* Exit program. */
    exit( code );
}

static void handle_key_down( SDL_keysym* keysym )
{
	// handle only non-unicode symbols

	if(keysym->mod & KMOD_LALT && keysym->sym == SDLK_RETURN)
	{
		//SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
		//if(g_flags&SDL_FULLSCREEN)
			g_flags ^= SDL_FULLSCREEN;
			SDL_SetVideoMode(g_width, g_height, g_bpp, g_flags);
		return;
	}

	if(!dconsole::Instance().isOpened() && SDLK_TAB == keysym->sym)
	{
		dconsole::Instance().setOpen(true);
	}
	else if(dconsole::Instance().isOpened())
	{
		bool mod_pressed = !!(keysym->mod & KMOD_LCTRL);
		if(!mod_pressed) // things like "crtl+space" should not add space!
			dconsole::Console::onKbdEvent(keysym);

		switch( keysym->sym ) {
			// execute command
			case SDLK_RETURN: dconsole::Console::onKbdExecute(); break;
				// match list
			case SDLK_UP: dconsole::Console::onKbdPrev(mod_pressed); break;
			case SDLK_DOWN: dconsole::Console::onKbdNext(mod_pressed); break;
			case SDLK_PAGEUP: dconsole::Console::onKbdPrevPage(mod_pressed); break;
			case SDLK_PAGEDOWN: dconsole::Console::onKbdNextPage(mod_pressed); break;
				// cmd line
			case SDLK_SPACE: if(mod_pressed) dconsole::Console::onKbdAutocomplete(); break;
			case SDLK_BACKSPACE: dconsole::Console::onKbdBackspace(); break;
			case SDLK_HOME: dconsole::Console::onKbdMoveHome(); break;
			case SDLK_END: dconsole::Console::onKbdMoveEnd(); break;
			case SDLK_LEFT: dconsole::Console::onKbdMoveLeft(); break;
			case SDLK_RIGHT: dconsole::Console::onKbdMoveRight(); break;
			case SDLK_DELETE: dconsole::Console::onKbdDelete(); break;
		}
	}


    switch( keysym->sym ) {
    case SDLK_ESCAPE:
        quit_tutorial( 0 );
        break;
    /*case SDLK_SPACE:
        should_rotate = !should_rotate;
        break;*/
    default:
        break;
    }

}

static void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYDOWN:
            /* Handle key presses. */
            handle_key_down( &event.key.keysym );
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            quit_tutorial( 0 );
            break;
		case SDL_VIDEORESIZE:
			{
				glViewport(0, 0, (GLsizei) event.resize.w, (GLsizei) event.resize.h);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				float w = (float)event.resize.w;
				float h = (float)event.resize.h;
				gluPerspective( 60.0, w/h, 1.0, 1024.0 );
				glMatrixMode(GL_MODELVIEW);

				g_pTextDriver->getScreenParams(event.resize.w, event.resize.h);
			}
			break;
        }

    }

}

static void draw_screen( void )
{
    /* Our angle of rotation. */
    static float angle = 0.0f;

    /*
	 * change it to use compiled vertex arrays :-)
     */
    static GLfloat v0[] = { -1.0f, -1.0f,  1.0f };
    static GLfloat v1[] = {  1.0f, -1.0f,  1.0f };
    static GLfloat v2[] = {  1.0f,  1.0f,  1.0f };
    static GLfloat v3[] = { -1.0f,  1.0f,  1.0f };
    static GLfloat v4[] = { -1.0f, -1.0f, -1.0f };
    static GLfloat v5[] = {  1.0f, -1.0f, -1.0f };
    static GLfloat v6[] = {  1.0f,  1.0f, -1.0f };
    static GLfloat v7[] = { -1.0f,  1.0f, -1.0f };
    static GLubyte red[]    = { 255,   0,   0, 255 };
    static GLubyte green[]  = {   0, 255,   0, 255 };
    static GLubyte blue[]   = {   0,   0, 255, 255 };
    static GLubyte white[]  = { 255, 255, 255, 255 };
    static GLubyte yellow[] = {   0, 255, 255, 255 };
    static GLubyte black[]  = {   0,   0,   0, 255 };
    static GLubyte orange[] = { 255, 255,   0, 255 };
    static GLubyte purple[] = { 255,   0, 255,   0 };

    /* Clear the color and depth buffers. */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* We don't want to modify the projection matrix. */
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    /* Move down the z-axis. */
    glTranslatef( 0.0, 0.0, -5.0 );

    /* Rotate. */
    glRotatef( angle, 0.0, 1.0, 0.0 );

    if( should_rotate ) {

        if( ++angle > 360.0f ) {
            angle = 0.0f;
        }

    }

    /* Send our triangle data to the pipeline. */
    glBegin( GL_TRIANGLES );

    glColor4ubv( red );
    glVertex3fv( v0 );
    glColor4ubv( green );
    glVertex3fv( v1 );
    glColor4ubv( blue );
    glVertex3fv( v2 );

    glColor4ubv( red );
    glVertex3fv( v0 );
    glColor4ubv( blue );
    glVertex3fv( v2 );
    glColor4ubv( white );
    glVertex3fv( v3 );

    glColor4ubv( green );
    glVertex3fv( v1 );
    glColor4ubv( black );
    glVertex3fv( v5 );
    glColor4ubv( orange );
    glVertex3fv( v6 );

    glColor4ubv( green );
    glVertex3fv( v1 );
    glColor4ubv( orange );
    glVertex3fv( v6 );
    glColor4ubv( blue );
    glVertex3fv( v2 );

    glColor4ubv( black );
    glVertex3fv( v5 );
    glColor4ubv( yellow );
    glVertex3fv( v4 );
    glColor4ubv( purple );
    glVertex3fv( v7 );

    glColor4ubv( black );
    glVertex3fv( v5 );
    glColor4ubv( purple );
    glVertex3fv( v7 );
    glColor4ubv( orange );
    glVertex3fv( v6 );

    glColor4ubv( yellow );
    glVertex3fv( v4 );
    glColor4ubv( red );
    glVertex3fv( v0 );
    glColor4ubv( white );
    glVertex3fv( v3 );

    glColor4ubv( yellow );
    glVertex3fv( v4 );
    glColor4ubv( white );
    glVertex3fv( v3 );
    glColor4ubv( purple );
    glVertex3fv( v7 );

    glColor4ubv( white );
    glVertex3fv( v3 );
    glColor4ubv( blue );
    glVertex3fv( v2 );
    glColor4ubv( orange );
    glVertex3fv( v6 );

    glColor4ubv( white );
    glVertex3fv( v3 );
    glColor4ubv( orange );
    glVertex3fv( v6 );
    glColor4ubv( purple );
    glVertex3fv( v7 );

    glColor4ubv( green );
    glVertex3fv( v1 );
    glColor4ubv( red );
    glVertex3fv( v0 );
    glColor4ubv( yellow );
    glVertex3fv( v4 );

    glColor4ubv( green );
    glVertex3fv( v1 );
    glColor4ubv( yellow );
    glVertex3fv( v4 );
    glColor4ubv( black );
    glVertex3fv( v5 );

    glEnd( );

	dconsole::Instance().Draw();

    SDL_GL_SwapBuffers( );
}

static void setup_opengl( int width, int height )
{
    float ratio = (float) width / (float) height;

    /* Our shading model--Gouraud (smooth). */
    glShadeModel( GL_SMOOTH );

    /* Culling. */
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );

    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );

    /* Setup our viewport. */
    glViewport( 0, 0, width, height );

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    /*
     * EXERCISE:
     * Replace this with a call to glFrustum.
     */
    gluPerspective( 60.0, ratio, 1.0, 1024.0 );
}

int main( int argc, char* argv[] )
{
    /* Information about the current video settings. */
    const SDL_VideoInfo* info = NULL;


    /* First, initialize SDL's video subsystem. */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        /* Failed, exit. */
        fprintf( stderr, "Video initialization failed: %s\n",
             SDL_GetError( ) );
        quit_tutorial( 1 );
    }

    /* Let's get some video information. */
    info = SDL_GetVideoInfo( );

    if( !info ) {
        /* This should probably never happen. */
        fprintf( stderr, "Video query failed: %s\n",
             SDL_GetError( ) );
        quit_tutorial( 1 );
    }

    g_bpp = info->vfmt->BitsPerPixel;

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	g_flags = SDL_OPENGL|SDL_HWPALETTE|/*SDL_NOFRAME|*/SDL_HWSURFACE/*|SDL_RESIZABLE*//*| SDL_FULLSCREEN*/;

    /*
     * Set the video mode
     */
    if( SDL_SetVideoMode( g_width, g_height, g_bpp, g_flags ) == 0 ) {
        /* 
         * This could happen for a variety of reasons,
         * including DISPLAY not being set, the specified
         * resolution not being available, etc.
         */
        fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        quit_tutorial( 1 );
    }

    /*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    setup_opengl( g_width, g_height );

	/*
	 * Init OpenGL text driver which will 
	 * be used by GraphicsConsole
	 */
	g_pTextDriver = new OGLTextDriver();
	if( false == g_pTextDriver->init("FixedWidth.bmp", 8, 16, 16, g_width, g_height))
		quit_tutorial( 1 );


	register_commands();
	GraphicsConsole::Instance().setTextDriver(g_pTextDriver);
	SDL_EnableKeyRepeat(500, 30);

	SDL_WM_SetCaption("console", 0);

    /*
     * Now we want to begin our normal app process--
     * an event loop with a lot of redrawing.
     */
    while( !g_exit ) {
        /* Process incoming events. */
        process_events( );
        /* Draw the screen. */
        draw_screen( );
    }

	quit_tutorial(0);

    /*
     * EXERCISE:
     * Record timings using SDL_GetTicks() and
     * and print out frames per second at program
     * end.
     */

    /* Never reached. */
    return 0;
}