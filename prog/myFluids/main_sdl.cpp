/*
 * Fluids implementation example.
 * alariq@gmail.com
 *
 * SDL code based on Michael Vance's SDL Tutorial
 * Distributed under terms of the LGPL. 
 */


#include "dconsole/console.h"
#include "dconsole/command.h"

#include <windows.h>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "dconsole/graphic_console.h"
#include "dconsole/command.h"
#include "opengl_text_driver.h"
#include "shader_builder.h"
#include "utils/time_structs.h"
#include "input.h"

#include "Fluids.h"
#include "renderer.h"

#include "utils/ui.h"

/* Dimensions of our window. */
static int g_width = 800;
static int g_height = 600;
/* Color depth in bits of our window. */
static int g_bpp;
/* Flags we will pass into SDL_SetVideoMode. */
static int g_flags = 0;

static TimeInfo g_Time;

static OGLTextDriver* g_pTextDriver = 0;

static bool g_exit = false;

static Fluids g_fluids;
static glsl_program* draw_velocity = 0;
static glsl_program* draw_fluids = 0;

struct camera
{
    camera():rot_x(0), rot_y(0), dist(0) {}

    float rot_x;
    float rot_y;
    float dist;
};

camera g_camera;
mouse_data g_mouse;

GLint uni_mouse = 0;

time_t last_reload_time = 0;
time_t start_time = 0;


static int f_add(int a, int b)
{
    return a+b;
}


void reload_shaders()
{
	std::map<std::string, glsl_program*>::iterator it = glsl_program::s_programs.begin();
	std::map<std::string, glsl_program*>::iterator end = glsl_program::s_programs.end();
	for(;it!=end;++it)
	{
		it->second->reload();
	}
}

void register_commands()
{
	// add some test commands;
	dconsole::test();

	dconsole::Console& c = dconsole::Instance();
	dconsole::BaseCmd* pcmd = dconsole::createCmd("reload_shaders", "", reload_shaders);
	c.registerCommand(pcmd);
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
    default:
        break;
    }

}

static void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;
	g_mouse.rel_x  = g_mouse.rel_y = 0;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

		if(UI::Update(&event))
			continue;

        switch( event.type ) {
        case SDL_KEYDOWN:
            handle_key_down( &event.key.keysym );
            break;
        case SDL_MOUSEMOTION:
            g_mouse.x = event.motion.x;
            g_mouse.y = event.motion.y;
            g_mouse.rel_x = event.motion.xrel;
            g_mouse.rel_y = event.motion.yrel;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN)
                g_camera.dist += event.button.button == SDL_BUTTON_WHEELUP ? 2 : -2;
            else
                g_mouse.buttons[event.button.button-1] = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN)
                g_camera.dist += event.button.button == SDL_BUTTON_WHEELUP ? 2 : -2;
            else
                g_mouse.buttons[event.button.button-1] = false;
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
                g_width = event.resize.w;
                g_height = event.resize.h;
				//gluPerspective( 60.0, w/h, 1.0, 1024.0 );
                gluOrtho2D(-1.0*w/h,1.0*w/h,-1,1);
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

    /* Clear the color and depth buffers. */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* We don't want to modify the projection matrix. */
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

	g_fluids.drawBuffers(&g_mouse, g_Time.fElapsedTime);

	glViewport(0, 0, g_width, g_height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f*g_width/g_height,1.0f*g_width/g_height,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
	
	if(Fluids::DM_DENSITY == g_fluids.getViewMode())
	{
		GLuint d = g_fluids.getDensityAccumTexture();
		draw_fluids->apply();
		set_texture_for_sampler(draw_fluids, "d", 0, d);
		draw_quad(600.0f/800.0f); // compensate aspect (quad.vs tries to scale all to 0,1)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
	else if(Fluids::DM_VELOCITY0 == g_fluids.getViewMode())
	{
		GLuint u, v;
		g_fluids.getVelocityAccumTexture0(&u, &v);
		draw_velocity->apply();
		set_texture_for_sampler(draw_velocity, "u", 0, u);
		set_texture_for_sampler(draw_velocity, "v", 1, v);
		draw_quad(600.0f/800.0f); // compensate aspect (quad.vs tries to scale all to 0,1)
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	dconsole::Instance().Draw();
	UI::Draw();

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
    //glEnable( GL_CULL_FACE );

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

    //gluPerspective( 60.0, ratio, 1.0, 1024.0 );
    glOrtho( -1*ratio, 1*ratio, -1,1, 0.1, 100);
}

int main( int argc, char* argv[] )
{
    /* Information about the current video settings. */
    const SDL_VideoInfo* info = NULL;

    g_Time = InitTime();

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


    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        quit_tutorial(1);
    }

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    if (!GLEW_ARB_vertex_program || !GLEW_ARB_vertex_program)
    {
        fprintf(stderr, "No shader program support\n");
        quit_tutorial(1);
    }

    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {
        printf("OpenGL 2.0 not supported\n");
        quit_tutorial(1);
    }

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

	draw_velocity = glsl_program::makeProgram("drawVelocity", DATA_PATH"quad.vs", DATA_PATH"draw_velocity.fs");
	draw_fluids = glsl_program::makeProgram("drawFluids", DATA_PATH"quad.vs", DATA_PATH"draw_fluids.fs");
	assert(draw_velocity);

	start_time = time(0);

	g_fluids.createShaders();
	g_fluids.createBuffers();
	
	UI::Init(g_width, g_height);
	g_fluids.createUI();

	SDL_WM_SetCaption("console", 0);

    /*
     * Now we want to begin our normal app process--
     * an event loop with a lot of redrawing.
     */
    while( !g_exit ) {
        UpdateTime(&g_Time);
        /* Process incoming events. */
        process_events( );
        /* Draw the screen. */
        draw_screen( );
    }

	quit_tutorial(0);

    return 0;
}