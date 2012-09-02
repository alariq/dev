#ifndef __DX_TEXT_DRIVER__
#define __DX_TEXT_DRIVER__


#include <d3dx9.h>
#include "dconsole/i_text_driver.h"

class D3DXTextDriver: public ITextDriver {

	LPD3DXFONT		pFont_;
	D3DXMATRIX		viewMat_;
	D3DXMATRIX		realviewMat_;
	int				fontHeight_;
	int				fontWidth_;

	int				scrWidth_;
	int				scrHeight_;


public:
					D3DXTextDriver();
					~D3DXTextDriver();

	virtual bool	beginDraw();
	virtual void	endDraw();

	virtual void	printString(float x, float y, const char* s, const unsigned char* color = 0);

	virtual size_t	getWidth(const char* s, size_t len = -1);


	virtual void	getScreenParams(int& width, int& height);
	virtual void	getFontParams(int& font_width, int& font_height);

			bool	init(int fontWidth, int screenWidth, int screenHeight);
			void	setScreenParams(int w, int h);
};

#endif // __OPENGL_TEXT_DRIVER__
