#include "text_console.h"

#include <string>
#include <iostream>

// implement required function
namespace dconsole {
	Console& Instance() {
		return TextConsole::Instance();
	}
} // namespace dconsole 


void TextConsole::Draw()
{
	return;
}
TextConsole& TextConsole::Instance(void)
{                                                            
    static TextConsole console;
    return console;
}

void TextConsole::addLogRecord(const dconsole::Console::LogRecord r, bool addToCommandsBuffer)
{
	dconsole::Console::LogRecord::const_iterator it = r.begin();
	dconsole::Console::LogRecord::const_iterator end = r.end();
	for(;it!=r.end();++it)
	std::cout<<it->second;
	std::cout<<std::endl;
}

