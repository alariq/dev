#ifndef __I_TEXT_DRIVER__
#define __I_TEXT_DRIVER__

class ITextDriver {

public:
	virtual ~ITextDriver() {}

	virtual bool	beginDraw() = 0;
	virtual void	endDraw() = 0;

	virtual void	printString(float x, float y, const char* s, const unsigned char* color = 0) = 0;
	virtual size_t	getWidth(const char* s, size_t len = -1) = 0;
	virtual void	getScreenParams(int& width, int& height) = 0;
	virtual void	getFontParams(int& font_width, int& font_height) = 0;
};


#endif