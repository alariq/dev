/*
 * Text Console implementation example.
 * alariq@gmail.com
 * Distributed under terms of the LGPL.
 */


#include <stdio.h>
#include <tchar.h>
#include<iostream>
#include<assert.h>

#include "dconsole/console.h"
#include "dconsole/command.h"
#include "text_console.h"

static int f_add(int a, int b)
{
    return a+b;
}

int main(int argc, char* argv[])
{
	dconsole::test();

	dconsole::BaseCmd* pcmd = dconsole::createCmd("add", "an addition function", f_add);
	dconsole::Console& c = TextConsole::Instance();
	c.registerCommand(pcmd);
	c.setOpen(true);
	//c.setCmdLine("xdwdw");
	//c.process();

	assert (clamp(5, 1, 3) == 3);
	assert (clamp(5, 6, 8) == 6);
	assert (clamp(5, 1, 8) == 5);

	char buf [256] = {0};

	while(1)
	{
		std::string str;
		char* ptr = fgets(buf,255,stdin);
		size_t len = strlen(buf);
		if(len>1)
			buf[len-1]='\0';// substitute last character which will be '\n' by  '\0'
		if(strcmp(buf,"exit")==0)
			break;
		c.setCmdLine(buf);
		c.process();
	}

	return 0;
}

