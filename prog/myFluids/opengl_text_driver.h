#ifndef __OPENGL_TEXT_DRIVER__
#define __OPENGL_TEXT_DRIVER__

#include "dconsole/i_text_driver.h"
#include <windows.h>
#include <gl/gl.h>
#include "image.h"

struct SDL_Surface;

class OGLTextDriver: public ITextDriver {

	GLuint			texID_;  // OpenGl font texture
	//SDL_Surface*	fontTexture_;
	GLint			fontHeight_;
	GLint			fontWidth_;
	int				charStep_;
	std::string		texName_;

	int				scrWidth_;
	int				scrHeight_;
	Image*			fontTexture_;

public:
					OGLTextDriver();
					~OGLTextDriver();

	virtual bool	beginDraw();
	virtual void	endDraw();

	virtual void	printString(float x, float y, const char* s, const unsigned char* color = 0);

	virtual size_t	getWidth(const char* s, size_t len = -1);


	virtual void	getScreenParams(int& width, int& height);
	virtual void	getFontParams(int& font_width, int& font_height);

			bool	init(const char* fontTextureName, int charStep, int fontWidth, int fontHeight, int screenWidth, int screenHeight);
			void	setScreenParams(int w, int h);


};

#endif // __OPENGL_TEXT_DRIVER__
