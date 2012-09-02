#include <windows.h>
#include <gl/gl.h>
#include "sdl/SDL_keyboard.h"
#include "sdl/SDL.h"
#include <string>

#include "opengl_text_driver.h"



OGLTextDriver::OGLTextDriver():
texID_(0),
fontTexture_(0),
fontHeight_(0),
fontWidth_(0),
scrWidth_(0),
scrHeight_(0),
charStep_(0)
{
}

OGLTextDriver::~OGLTextDriver()
{
	glDeleteTextures(1, &texID_);
	//SDL_FreeSurface(fontTexture_);
	delete fontTexture_;
}

size_t OGLTextDriver::getWidth(const char* s, size_t len)
{
	if(!s) return 0;
	if(-1 == len) return strlen(s)*charStep_;

	// as we are using in this implementation fixed with font we can just return this
	return len*charStep_;
}

bool OGLTextDriver::init(const char* fontTextureName, int charStep, int fontWidth, int fontHeight, int screenWidth, int screenHeight)
{
	if(!fontTextureName) return false;
	texName_ = fontTextureName;

	//fontTexture_ = SDL_LoadBMP(texName_.c_str());
	//if(!fontTexture_) return false;

	fontTexture_ = new Image();
	if( false == fontTexture_->loadFromFile( texName_.c_str() ) )
		return false;
	
	// support only 32bit textures :P

	//int bpp = fontTexture_->format->BytesPerPixel;
	//if(4 != bpp) return false; 
	if( 4 != getBytesPerPixel(fontTexture_->getFormat()))
		return false;

	glGenTextures(1, &texID_);
	glBindTexture(GL_TEXTURE_2D, texID_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fontTexture_->getWidth(), 
		fontTexture_->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 
		fontTexture_->getPixels());

	fontWidth_ = fontWidth;
	fontHeight_ = fontHeight;

	scrWidth_ = screenWidth;
	scrHeight_ = screenHeight;

	charStep_ = charStep;

	return true;
}

bool OGLTextDriver::beginDraw()
{
	GLfloat white[3] = { 1.0, 0.0, 0.0 };

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho (0.0, scrWidth_, 0.0, scrHeight_, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	   
	glColor3fv(white);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, texID_);

	return true;
}

void OGLTextDriver::endDraw()
{
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void	OGLTextDriver::printString(float x, float y, const char* s, const unsigned char* color)
{
	if(!s) return;
	size_t len = strlen(s);
	if(len==0) return;

	if(color)
		glColor4ubv((const GLubyte*)color);
	else
		glColor4f(1.0f,1.0f,1.0f,1.0f);

	glBegin(GL_QUADS);
	const float du = fontWidth_/(float)fontTexture_->getWidth();
	const float dv = fontHeight_/(float)fontTexture_->getHeight();

	//x+=0.5f;
	for(size_t i=0;i<len;i++)
	{
		float u,v;
		u = (s[i] % fontWidth_)/(float)fontWidth_;
		v = (s[i] / fontHeight_)/(float)fontHeight_;

		glTexCoord2f(u,v+dv);
		glVertex2f(x,y);

		glTexCoord2f(u+du,v+dv);
		glVertex2f(x+fontWidth_,y);

		glTexCoord2f(u+du,v);
		glVertex2f(x+fontWidth_,y+fontHeight_);

		glTexCoord2f(u,v);
		glVertex2f(x,y+fontHeight_);

		x += charStep_;
	}
	glEnd();

}

void OGLTextDriver::getScreenParams(int& width, int& height)
{
	width = scrWidth_;
	height = scrHeight_;
}

void OGLTextDriver::setScreenParams(int w, int h)
{
	scrWidth_ = w;
	scrHeight_ = h;
}

void OGLTextDriver::getFontParams(int& font_width, int& font_height) 
{
	font_width = fontWidth_;
	font_height = fontHeight_;
}

