#ifndef __GRAPHIC_CONSOLE_H__
#define __GRAPHIC_CONSOLE_H__

#include "dconsole/console.h"
#include "dconsole/i_text_driver.h"

class GraphicsConsole: public dconsole::Console {
	ITextDriver* pTextDriver_;
public:
	virtual void Draw();
    static GraphicsConsole& Instance(void)
	{                                                            
		static GraphicsConsole console;
		return console;
	}
	void setTextDriver(ITextDriver* pTextDriver)
	{
		pTextDriver_ = pTextDriver;
	}

};



#endif // __GRAPHIC_CONSOLE_H__