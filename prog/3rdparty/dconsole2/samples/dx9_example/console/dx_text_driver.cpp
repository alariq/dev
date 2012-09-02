#include <string>
#include <math.h>
#include "dx_text_driver.h"
extern LPDIRECT3DDEVICE9 GetDevice();

D3DXTextDriver::D3DXTextDriver():
fontHeight_(0),
fontWidth_(0),
scrWidth_(0),
scrHeight_(0)
{
}

D3DXTextDriver::~D3DXTextDriver()
{
	pFont_->Release();
}

size_t D3DXTextDriver::getWidth(const char* s, size_t len)
{
	if(!s) return 0;
	if(-1 == len) return strlen(s)*fontWidth_;

	// as we are using in this implementation fixed with font we can just return this
	return len*fontWidth_;
}

bool D3DXTextDriver::init(int fontWidth, int screenWidth, int screenHeight)
{
	HRESULT hr = E_FAIL;
	hr = D3DXCreateFont( GetDevice(), fontWidth, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, MONO_FONT | FF_DONTCARE, "Courier New", &pFont_);
	if(FAILED(hr))
		return false;

	TEXTMETRIC tm; 
	pFont_->GetTextMetrics(&tm);
	fontHeight_ = tm.tmHeight;
	fontWidth_ = tm.tmAveCharWidth;

	//create projection matrix for 2D elements
	D3DXMatrixOrthoLH(&viewMat_,(float)screenWidth, (float)screenHeight, 0.1f, 100);
	D3DXMATRIX tr_m, rot_m;
	D3DXMatrixRotationX(&rot_m, 3.14f);
	D3DXMatrixTranslation(&tr_m, 0, .5f*(float)screenHeight, 0);
	
	D3DXMatrixIdentity(&realviewMat_);
	//D3DXMatrixMultiply(&realviewMat_, &realviewMat_, &rot_m);
	D3DXMatrixMultiply(&realviewMat_, &realviewMat_, &tr_m);


	scrWidth_ = screenWidth;
	scrHeight_ = screenHeight;

	return true;
}

bool D3DXTextDriver::beginDraw()
{
	D3DXMATRIX identity;	
	D3DXMatrixIdentity(&identity);

	//GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	//GetDevice()->SetTransform(D3DTS_PROJECTION, &identity);
	
	//GetDevice()->SetTransform(D3DTS_VIEW, &realviewMat_);
	return true;
}

void D3DXTextDriver::endDraw()
{
	
}

void	D3DXTextDriver::printString(float x, float y, const char* s, const unsigned char* color)
{
	if(!s) return;
	size_t len = strlen(s);
	if(len==0) return;

	RECT r = {(LONG)x,(LONG)y,(LONG)x+800,(LONG)y+600};
	D3DXCOLOR c(1.0f,1.0f,1.0f,1.0f);
	if(color)
	{
		const float f = 1.0f / 255.0f;
		c.r = f*color[0];
		c.g = f*color[1];
		c.b = f*color[2];
		c.a = f*color[3];
	}

	pFont_->DrawText(NULL, s, -1, &r,DT_NOCLIP|DT_LEFT|DT_TOP, c);
}

void D3DXTextDriver::getScreenParams(int& width, int& height)
{
	width = scrWidth_;
	height = scrHeight_;
}

void D3DXTextDriver::setScreenParams(int w, int h)
{
	scrWidth_ = w;
	scrHeight_ = h;
	//create projection matrix for 2D elements
	D3DXMatrixOrthoLH(&viewMat_,(float)scrWidth_, (float)scrHeight_, 0.1f, 100);
}

void D3DXTextDriver::getFontParams(int& font_width, int& font_height) 
{
	font_width = fontWidth_;
	font_height = fontHeight_;
}

