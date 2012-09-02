#include <windows.h>

#include "dconsole/graphic_console.h"
#include "dconsole/console.h"

#include "MyConsole.h"

namespace dconsole {
	Console& Instance() {
		return GraphicsConsole::Instance();
	}
} // namespace dconsole


void dconsole::Console::onKbdEvent(void* pdata)
{
	dconsole::Console& console = dconsole::Instance();
	KeyEvent* pKeyEvent = (KeyEvent*)pdata;

	if('\t' == pKeyEvent->c)
	{
	   console.is_opened = ! console.is_opened;
	   return;
	}
	if(!console.is_opened)
	   return;

	unsigned char ch= pKeyEvent->c;
	if( ch>= 'A' && ch<='Z')
	{
		if(!(pKeyEvent->mod & VK_SHIFT))
			ch += 32;
		if(console.cmd_line.insertAt(ch, (unsigned int)console.token))
			console.token++;
	}
	else if(pKeyEvent->mod & VK_SHIFT)
	{
		bool catched = true;
		switch(ch)
		{
		case '0':		ch = ')'; break;
		case '1':		ch = '!'; break;
		case '2':		ch = '@'; break;
		case '3':		ch = '#'; break;
		case '4':		ch = '$'; break;
		case '5':		ch = '%'; break;
		case '6':		ch = '^'; break;
		case '7':		ch = '&'; break;
		case '8':		ch = '*'; break;
		case '9':		ch = '('; break;
		case '=':		ch = '+'; break;
		case '-':		ch = '_'; break;
		case ',':		ch = '<'; break;
		case '.':		ch = '>'; break;
		case '/':		ch = '?'; break;
		default:		catched	= false;	break;
		}

		if(catched)
		if(console.cmd_line.insertAt(ch, (unsigned int)console.token))
			console.token++;
	}
	else
	{
		if(console.cmd_line.insertAt(ch, (unsigned int)console.token))
			console.token++;
	}


}
