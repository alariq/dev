#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <assert.h>

#include "renderer.h"
#include "shader_builder.h"

GLuint createRenderTexture(int w, int h, int fmt, int int_fmt, int type)
{
	GLuint color_tex;
	glGenTextures(1, &color_tex);

	// initialize color texture
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, int_fmt, type, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	return color_tex;
}

GLuint createRenderBuffer(int w, int h, int format)
{
	GLuint depth_rb;
	glGenRenderbuffers(1, &depth_rb);
	
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	
	int fmt = GL_DEPTH_COMPONENT24;
	if(format!=-1)
		fmt = format;

	glRenderbufferStorage(GL_RENDERBUFFER, fmt, w, h);



	return depth_rb;
}

void setRenderTexture(int tex, int index)
{
	int attach = GL_COLOR_ATTACHMENT0;

	if(index==-1)
		attach = GL_DEPTH_ATTACHMENT;
	else
		attach = GL_COLOR_ATTACHMENT0 + index;
	
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attach, GL_TEXTURE_2D, tex, 0);
}

void setRenderBuffer(int buffer, int index)
{
	int attach = GL_COLOR_ATTACHMENT0;

	if(index==-1)
		attach = GL_DEPTH_ATTACHMENT;
	else
		attach = GL_COLOR_ATTACHMENT0 + index;

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attach, GL_RENDERBUFFER, buffer);
}


GLuint createFrameBuffer()
{
	GLuint fb;
	glGenFramebuffers(1, &fb);
	return fb;
}

void setFrameBuffer(GLuint fb)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
}


///////////////////////////////////////////////////////////////////////////////
// check FBO completeness
///////////////////////////////////////////////////////////////////////////////
bool checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        //std::cout << "Framebuffer complete." << std::endl;
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
        return false;
/*
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
        std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
        return false;
*/
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
        return false;

    default:
        std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
        return false;
    }
}

const char* const getStringError(GLenum gl_error)
{
	const char* error = 0;
	switch(gl_error)
	{
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
            error = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
            break;
		case GL_INVALID_VALUE:
            error = "GL_INVALID_VALUE: A numeric argument is out of range.";
			break;
		case GL_INVALID_OPERATION:
            error = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
			break;
		case GL_STACK_OVERFLOW:
            error = " GL_STACK_OVERFLOW: This command would cause a stack overflow.";
			break;
		case GL_STACK_UNDERFLOW:
            error = "GL_STACK_UNDERFLOW: This command would cause a stack underflow.";
			break;
		case GL_OUT_OF_MEMORY:
            error = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
			break;
		case GL_TABLE_TOO_LARGE:
			error = "The specified table exceeds the implementation's maximum supported table size.";
			break;
	}
	return error;	
}

///////////////////////////////////////////////////////////////////////////////
// draw a textured cube with GL_TRIANGLES
///////////////////////////////////////////////////////////////////////////////
void draw(GLuint textureId)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glColor4f(1, 1, 1, 1);
	glBegin(GL_TRIANGLES);
	// front faces
	glNormal3f(0,0,1);
	// face v0-v1-v2
	glTexCoord2f(1,1);  glVertex3f(1,1,1);
	glTexCoord2f(0,1);  glVertex3f(-1,1,1);
	glTexCoord2f(0,0);  glVertex3f(-1,-1,1);
	// face v2-v3-v0
	glTexCoord2f(0,0);  glVertex3f(-1,-1,1);
	glTexCoord2f(1,0);  glVertex3f(1,-1,1);
	glTexCoord2f(1,1);  glVertex3f(1,1,1);

	// right faces
	glNormal3f(1,0,0);
	// face v0-v3-v4
	glTexCoord2f(0,1);  glVertex3f(1,1,1);
	glTexCoord2f(0,0);  glVertex3f(1,-1,1);
	glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
	// face v4-v5-v0
	glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
	glTexCoord2f(1,1);  glVertex3f(1,1,-1);
	glTexCoord2f(0,1);  glVertex3f(1,1,1);

	// top faces
	glNormal3f(0,1,0);
	// face v0-v5-v6
	glTexCoord2f(1,0);  glVertex3f(1,1,1);
	glTexCoord2f(1,1);  glVertex3f(1,1,-1);
	glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
	// face v6-v1-v0
	glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
	glTexCoord2f(0,0);  glVertex3f(-1,1,1);
	glTexCoord2f(1,0);  glVertex3f(1,1,1);

	// left faces
	glNormal3f(-1,0,0);
	// face  v1-v6-v7
	glTexCoord2f(1,1);  glVertex3f(-1,1,1);
	glTexCoord2f(0,1);  glVertex3f(-1,1,-1);
	glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
	// face v7-v2-v1
	glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
	glTexCoord2f(1,0);  glVertex3f(-1,-1,1);
	glTexCoord2f(1,1);  glVertex3f(-1,1,1);

	// bottom faces
	glNormal3f(0,-1,0);
	// face v7-v4-v3
	glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);
	glTexCoord2f(1,0);  glVertex3f(1,-1,-1);
	glTexCoord2f(1,1);  glVertex3f(1,-1,1);
	// face v3-v2-v7
	glTexCoord2f(1,1);  glVertex3f(1,-1,1);
	glTexCoord2f(0,1);  glVertex3f(-1,-1,1);
	glTexCoord2f(0,0);  glVertex3f(-1,-1,-1);

	// back faces
	glNormal3f(0,0,-1);
	// face v4-v7-v6
	glTexCoord2f(0,0);  glVertex3f(1,-1,-1);
	glTexCoord2f(1,0);  glVertex3f(-1,-1,-1);
	glTexCoord2f(1,1);  glVertex3f(-1,1,-1);
	// face v6-v5-v4
	glTexCoord2f(1,1);  glVertex3f(-1,1,-1);
	glTexCoord2f(0,1);  glVertex3f(1,1,-1);
	glTexCoord2f(0,0);  glVertex3f(1,-1,-1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}


void  draw_quad(float aspect)
{
	float r = aspect;

	glBegin(GL_QUADS);
		
		glTexCoord2f(0,0);
		glVertex2f(-1*r,-1);
		
		glTexCoord2f(1,0);
		glVertex2f(1*r, -1);
		
		glTexCoord2f(1,1);
		glVertex2f(1*r, 1);
		
		glTexCoord2f(0,1);
		glVertex2f(-1*r, 1);
	glEnd();
}




void  draw_quad(GLuint texture, float aspect)
{
	if(texture!=0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	draw_quad(aspect);

	if(texture!=0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
}


void set_texture_for_sampler(glsl_program* program, const char* sampler, int index, GLuint texID)
{
		assert(program->samplers_.count(sampler));
		{
			glActiveTexture(GL_TEXTURE0 + (index));
			glBindTexture(GL_TEXTURE_2D, (texID));
			glUniform1i(program->samplers_[sampler]->index_, (index));
			CHECK_GL_ERROR();
		}
}