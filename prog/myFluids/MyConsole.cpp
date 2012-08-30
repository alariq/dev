#include "dconsole/graphic_console.h"
#include <SDL/SDL_keyboard.h>

namespace dconsole {
	Console& Instance() {
		return GraphicsConsole::Instance();
	}
} // namespace dconsole


void dconsole::Console::onKbdEvent(void* pdata)
{
	dconsole::Console& console = dconsole::Instance();
	SDL_keysym* keysym = (SDL_keysym*)pdata;

    if(SDLK_TAB == keysym->sym)
    {
        console.is_opened = ! console.is_opened;
        return;
    }
    if(!console.is_opened)
        return;

	// handle only non-unicode symbols
	unsigned int ch;
	if ( (keysym->unicode & 0xFF80) == 0 && keysym->unicode!=0)
	{
		ch = (char)(keysym->unicode & 0x7F);
	}
	else
	{
		ch = keysym->sym;
	}

	//GetKeyNameText
	if(ch >= SDLK_a && ch<=SDLK_z && keysym->mod & KMOD_SHIFT)
	{
		ch-=32;
	}
	else if(keysym->mod & KMOD_SHIFT)
	{
		switch(ch)
		{
		case SDLK_0:		ch = SDLK_RIGHTPAREN; break;
		case SDLK_1:		ch = SDLK_EXCLAIM; break;
		case SDLK_2:		ch = SDLK_AT; break;
		case SDLK_3:		ch = SDLK_HASH; break;
		case SDLK_4:		ch = SDLK_DOLLAR; break;
		case SDLK_5:		ch = 37; break; // %
		case SDLK_6:		ch = SDLK_COLON; break;
		case SDLK_7:		ch = SDLK_AMPERSAND; break;
		case SDLK_8:		ch = SDLK_ASTERISK; break;
		case SDLK_9:		ch = SDLK_LEFTPAREN; break;
		case SDLK_EQUALS:	ch = SDLK_PLUS; break;
		case SDLK_MINUS:	ch = SDLK_UNDERSCORE; break;
		case SDLK_COMMA:	ch = SDLK_LESS; break;
		case SDLK_PERIOD:	ch = SDLK_GREATER; break;
		case SDLK_SLASH:	ch = SDLK_QUESTION; break;
		default: break;
		}
	}

	if(ch >=SDLK_SPACE && ch<=SDLK_z)
		if(console.cmd_line.insertAt((unsigned char)ch, (unsigned int)console.token))
			console.token++;
}
