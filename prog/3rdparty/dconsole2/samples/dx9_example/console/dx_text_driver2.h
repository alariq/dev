#ifndef __DX_TEXT_DRIVER__
#define __DX_TEXT_DRIVER__


#include <d3dx9.h>
#include "dconsole/i_text_driver.h"

class D3DXTextDriver2: public ITextDriver {

	struct FontVertex_t {
		FLOAT x, y, z;
		FLOAT tu, tv;   // The texture coordinates
		DWORD color;
	};

	IDirect3DTexture9* pFontTexture_;
	D3DSURFACE_DESC fontTexDesc_;
	LPDIRECT3DVERTEXDECLARATION9 pFontVD_;
	LPDIRECT3DVERTEXBUFFER9	pVB_;
	IDirect3DVertexShader9*			pVertexShader_; // Vertex Shader
	IDirect3DPixelShader9*			pPixelShader_;  // Pixel Shader

	static const int		VB_SIZE	= 512;	// 512 chars
	int				num_chars_;
	FontVertex_t*	pVertices;

	D3DXMATRIX		matView_;
	D3DXMATRIX		matProj_;
	D3DXMATRIX		matWorld_;

	int				fontHeight_;
	int				fontWidth_;
	int				charStep_;

	int				scrWidth_;
	int				scrHeight_;

	static D3DVERTEXELEMENT9 fontVertDecl_[];

public:
					D3DXTextDriver2();
					~D3DXTextDriver2();

	virtual bool	beginDraw();
	virtual void	endDraw();

	virtual void	printString(float x, float y, const char* s, const unsigned char* color = 0);

	virtual size_t	getWidth(const char* s, size_t len = -1);


	virtual void	getScreenParams(int& width, int& height);
	virtual void	getFontParams(int& font_width, int& font_height);

			bool	init(const char* fontPath, int charStep, int fontWidth, int fontHeight, int screenWidth, int screenHeight);
			void	setScreenParams(int w, int h);
};

#endif // __OPENGL_TEXT_DRIVER__
