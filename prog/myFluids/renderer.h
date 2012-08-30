#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <windows.h>
#include <GL/gl.h>
#include <iostream>
struct glsl_program;

GLuint createRenderTexture(int w, int h, int fmt = GL_RGB8, int int_fmt = GL_RGB, int type = GL_UNSIGNED_BYTE);
GLuint createRenderBuffer(int w, int h, int format = -1);

void setRenderTexture(int tex, int index = 0);
void setRenderBuffer(int buffer, int index = -1);

GLuint createFrameBuffer();
void setFrameBuffer(GLuint fb);

///////////////////////////////////////////////////////////////////////////////
// check FBO completeness
///////////////////////////////////////////////////////////////////////////////
bool checkFramebufferStatus();

void draw(GLuint textureId);
void draw_quad(GLuint texture, float aspect);
void draw_quad(float aspect);

const char* const getStringError(GLenum gl_error);

void set_texture_for_sampler(glsl_program* program, const char* sampler, int index, GLuint texID);

#define CHECK_GL_ERROR() \
{\
	GLenum error = glGetError();\
	const char* str_error = getStringError(error);\
	if(error) puts(str_error); \
	assert(!error); \
}




#endif // __RENDERER_H__